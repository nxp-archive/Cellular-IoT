/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file CellIoT_shield.c
 * @brief NB-IoT Shield Interface.
 */
#include "CellIoT_shield.h"
#include "CellIoT_tools.h"
#include "fsl_usart.h"
#include "fsl_ctimer.h"
#include "fsl_inputmux.h"
#include "fsl_mrt.h"

//usart_rtos_handle_t g_uartRtosHandle;
//usart_handle_t g_uartHandle;
usart_dma_handle_t g_uartDmaHandle;
dma_handle_t g_uartTxDmaHandle;
dma_handle_t g_uartRxDmaHandle;
dma_handle_t g_timerTransferHandle;
/* MRT Register value that will be pushed to MRT INTVAL register when USART RX will pace DMA CH10 */
const uint32_t timerRegisterValue = /*0x80000100*/ 0x800F4240;

ctimer_callback_t ctimer_callback_table[] = { Timer_CallbackHandler };

/*! @brief Static table of descriptors */
#if defined(__ICCARM__)
#pragma data_alignment              = 16
dma_descriptor_t g_pingpong_desc;
#elif defined(__CC_ARM) || defined(__ARMCC_VERSION)
__attribute__((aligned(16))) dma_descriptor_t g_pingpong_desc;
#elif defined(__GNUC__)
__attribute__((aligned(16))) dma_descriptor_t g_pingpong_desc[3];
#endif

/*!
 * @brief Low level initialization, RTOS does not have to run yet
 */
int CELLIOTSHIELD_Init( uint32_t baudrate )
{
	/* Initialise the Multi-Rate Timer configuration */
	if (kStatus_Success != CELLIOTSHIELD_TimerConfig())
		return kStatus_InvalidArgument;

    /* Initialize pinmux */
#ifdef CELLIOTSHIELD_PINMUX_INIT
    CELLIOTSHIELD_PINMUX_INIT();
#endif

    /* Enable clocks for USART */
	CLOCK_AttachClk(CELLIOTSHIELD_USART_CLK_CONNECT);
	RESET_PeripheralReset(CELLIOTSHIELD_USART_IP_RESET);

    /* Initialise the USART configuration */
    if (kStatus_Success != CELLIOTSHIELD_USARTConfig(baudrate))
    	return kStatus_InvalidArgument;

    /* Initialise the DMA configuration */
	if (kStatus_Success != CELLIOTSHIELD_DMAConfig())
		return kStatus_InvalidArgument;

    return kStatus_Success;
}

/**
 * @brief Configure the USART for the NB-IoT module.
 */
int CELLIOTSHIELD_USARTConfig( uint32_t baudrate )
{
	usart_config_t usart_config;

	/* Initialize the UART. */
	/*
	 * config.baudRate_Bps = baudrate (Supposed to be 921600);
	 * config.parityMode = kUART_ParityDisabled;
	 * config.stopBitCount = kUART_OneStopBit;
	 * config.txFifoWatermark = 0;
	 * config.rxFifoWatermark = 1;
	 * config.enableTx = true;
	 * config.enableRx = true;
	 */
	USART_GetDefaultConfig(&usart_config);

	usart_config.baudRate_Bps = baudrate;
	usart_config.enableTx     = true;
	usart_config.enableRx     = true;

	USART_Init(CELLIOTSHIELD_USART, &usart_config, CLOCK_GetFlexCommClkFreq(USART_GetInstance(CELLIOTSHIELD_USART)));

	USART_EnableCTS(CELLIOTSHIELD_USART, true);

	/* Enable DMA request from rxFIFO */
	USART_EnableRxDMA(CELLIOTSHIELD_USART, true);

	return kStatus_Success;
}

/**
 * @brief Configure the DMA for the USART communication with the NB-IoT module.
 */
int CELLIOTSHIELD_DMAConfig( void )
{
	dma_transfer_config_t transferConfig[2];
	dma_channel_trigger_t channel0HwTrg = { kDMA_FallingEdgeTrigger, kDMA_EdgeBurstTransfer1, kDMA_SrcWrap };
	dma_channel_trigger_t channel10HwTrg = { kDMA_FallingEdgeTrigger, kDMA_EdgeBurstTransfer1, kDMA_NoWrap };

	/* Initialize DMA and Channels */
	INPUTMUX_Init(INPUTMUX);
	DMA_Init(CELLIOTSHIELD_DMA);
	DMA_EnableChannel(CELLIOTSHIELD_DMA, CELLIOTSHIELD_DMA_TX_CH);
	DMA_EnableChannel(CELLIOTSHIELD_DMA, CELLIOTSHIELD_DMA_RX_CH);
	DMA_EnableChannel(CELLIOTSHIELD_DMA, CELLIOTSHIELD_DMA_SPARE_CH);

	/* Set the DMA interrupt level to the high priority level in the system */
	NVIC_SetPriority((IRQn_Type)CELLIOTSHILED_DMA_IRQ, CELLIOTSHIELD_DMA_IRQ_PRIORITY);

	/* Create DMA Channel Handles & enable associated NVIC and channels interrupts */
	DMA_CreateHandle(&g_uartTxDmaHandle, CELLIOTSHIELD_DMA, CELLIOTSHIELD_DMA_TX_CH);
	DMA_CreateHandle(&g_uartRxDmaHandle, CELLIOTSHIELD_DMA, CELLIOTSHIELD_DMA_RX_CH);
	DMA_CreateHandle(&g_timerTransferHandle, CELLIOTSHIELD_DMA, CELLIOTSHIELD_DMA_SPARE_CH);

	/* Disable DMA Channel 10 Interrupts for USART RX since
	 * it is the MRT Timer Interrupts that will notice
	 * the application that data has arrived
	 */
	DMA_DisableChannelInterrupts(CELLIOTSHIELD_DMA, CELLIOTSHIELD_DMA_RX_CH);

	/* Create USART DMA handle */
	USART_TransferCreateHandleDMA(CELLIOTSHIELD_USART, &g_uartDmaHandle, NULL, NULL, &g_uartTxDmaHandle, NULL);

	/* Set Channel 0 DMA Callback */
	DMA_SetCallback(&g_timerTransferHandle, DMA_Callback, NULL);

	/* Prepare transfer of DMA Channel 10
	 * --> Memory to Memory transfer
	 * --> Write the timerRegisterValue array to MRT registers
	 * --> Transfer configuration hold by transferConfig[0]
	 * --> Next descriptor hold by g_pingpong_desc[0]
	 */
	DMA_PrepareTransfer(&transferConfig[0], (void *)&timerRegisterValue, ((void *)((uint32_t)&MRT0->CHANNEL[0].INTVAL)), sizeof(uint32_t),
						sizeof(uint32_t), kDMA_MemoryToMemory, &g_pingpong_desc[0]);

	/* Load the DMA Channel 10 configuration
	 * - Even if DMA Channel 10 is going to do a Memory to Memory transfer,
	 *   the DMA channel request is still paced by the RX of USART
	 *   so PERIPHREQEN needs to be set to keep this request enabled
	 *
	 * - We also need to specify that CLRTRIG will clear the channel 10 trigger
	 *   because the 1st trigger of DMA Channel 10 will be a Software trigger
	 *   After the 1st descriptor exhaustion, the DMA Channel 10 input trigger
	 *   will be a hardware trigger
	 */
	transferConfig[0].isPeriph= true;
	transferConfig[0].xfercfg.clrtrig = true;
	DMA_SubmitTransfer(&g_uartRxDmaHandle, &transferConfig[0]);

	/* Configure the DMA Channel 10 Configuration Register (CFG[10])
	 * channel10HwTrg = { kDMA_FallingEdgeTrigger, kDMA_EdgeBurstTransfer1, kDMA_NoWrap };
	 * --> PERIPHREQEN = True = 1
	 * --> HWTRIGEN = kDMA_FallingEdgeTrigger = 1
	 * --> TRIGPOL = kDMA_FallingEdgeTrigger = 0
	 * --> TRIGTYPE = kDMA_FallingEdgeTrigger = 0
	 * --> TRIGBURST = kDMA_EdgeBurstTransfer1 = 1
	 * --> BURSTPOWER = kDMA_EdgeBurstTransfer1 = 0
	 * --> SRCBURSTWRAP = kDMA_NoWrap = 0
	 * --> DSTBURSTWRAP = kDMA_NoWrap = 0
	 */
	DMA_SetChannelConfig(CELLIOTSHIELD_DMA, CELLIOTSHIELD_DMA_RX_CH, &channel10HwTrg, true);

	/* Configure the DMA Channel 10 Descriptor */
    DMA_CreateDescriptor(&g_pingpong_desc[0], &transferConfig[0].xfercfg, (void *)&timerRegisterValue, ((void *)((uint32_t)&MRT0->CHANNEL[0].INTVAL)),
						 &g_pingpong_desc[0]);

	/* Attach the trigger output of DMA0 Channel 10 to be DMA Output Trigger 0 (DMA0_OTRIG_INMUX[0])
	 * INP = 10
	 */
	INPUTMUX_AttachSignal(INPUTMUX, 0U, kINPUTMUX_Dma0Flexcomm2RxTrigoutToTriginChannels);

	/* Attach the trigger input of DMA0 Channel 0 to be DMA Output Trigger 0 (DMA0_ITRIG_INMUX[0])
	 * INP = 15
	 */
	INPUTMUX_AttachSignal(INPUTMUX, CELLIOTSHIELD_DMA_SPARE_CH, kINPUTMUX_Otrig0ToDma0);

	/* Attach the trigger input of DMA0 Channel 10 to be DMA Output Trigger 0 (DMA0_ITRIG_INMUX[10])
	 * INP = 15
	 */
	INPUTMUX_AttachSignal(INPUTMUX, CELLIOTSHIELD_DMA_RX_CH, kINPUTMUX_Otrig0ToDma0);

	/* Prepare transfer of DMA Channel 0
	 * --> Peripheral to Memory transfer
	 * --> Write the content of RX FIFO to USART RX buffers
	 * --> Transfer configuration hold by transferConfig[1]
	 * --> Next descriptor hold by g_pingpong_desc[2]
	 */
	DMA_PrepareTransfer(&transferConfig[1], ((void *)((uint32_t)&CELLIOTSHIELD_USART->FIFORD)), CELLIOTSHIELD_USART_RX_BUFFER, sizeof(uint8_t),
						sizeof(CELLIOTSHIELD_USART_RX_BUFFER) - 1, kDMA_PeripheralToMemory, &g_pingpong_desc[2]);

	/* Load the DMA0 Channel 0 configuration
	 * - Even if DMA Channel 0 is going to do a Peripheral to Memory transfer,
	 *   the DMA channel request is not paced by the Hash-Crypt DMA request
	 *   and will not be paced by any requests, only the DMA Channel 0 input trigger
	 *   is going to start the transfer
	 *   so PERIPHREQEN needs to be clear to keep the channel 0 request disabled
	 */
	transferConfig[1].isPeriph= false;
	DMA_SubmitTransfer(&g_timerTransferHandle, &transferConfig[1]);

	/* Configure the DMA Channel 0 Configuration Register (CFG[0])
	 * channel0HwTrg = { kDMA_FallingEdgeTrigger, kDMA_EdgeBurstTransfer1, kDMA_SrcWrap };
	 * --> PERIPHREQEN = False = 0
	 * --> HWTRIGEN = kDMA_FallingEdgeTrigger = 1
	 * --> TRIGPOL = kDMA_FallingEdgeTrigger = 0
	 * --> TRIGTYPE = kDMA_FallingEdgeTrigger = 0
	 * --> TRIGBURST = kDMA_EdgeBurstTransfer1 = 1
	 * --> BURSTPOWER = kDMA_EdgeBurstTransfer1 = 0
	 * --> SRCBURSTWRAP = kDMA_SrcWrap = 1
	 * --> DSTBURSTWRAP = kDMA_SrcWrap = 0
	 */
	DMA_SetChannelConfig(CELLIOTSHIELD_DMA, CELLIOTSHIELD_DMA_SPARE_CH, &channel0HwTrg, false);

	/* Configure the DMA Channel 0 1st Descriptor */
	transferConfig[1].xfercfg.intA = true;
	transferConfig[1].xfercfg.intB = false;
	DMA_CreateDescriptor(&g_pingpong_desc[1], &transferConfig[1].xfercfg, (void *)&CELLIOTSHIELD_USART->FIFORD, CELLIOTSHIELD_USART_RX_BUFFER,
						 &g_pingpong_desc[2]);

	/* Configure the DMA Channel 0 2nd Descriptor */
	transferConfig[1].xfercfg.intA = false;
	transferConfig[1].xfercfg.intB = true;
	DMA_CreateDescriptor(&g_pingpong_desc[2], &transferConfig[1].xfercfg, (void *)&CELLIOTSHIELD_USART->FIFORD, CELLIOTSHIELD_USART_RING_BUFFER,
						 &g_pingpong_desc[1]);

	/* Set the Software Trigger of DMA Channel 10
	 * DMA Channel 10 needs a software trigger to initiate a 1st trigger
	 * After the 1st descriptor exhaustion, the DMA Channel 10 input trigger
	 * will be a hardware trigger which is the DMA Channel 10 output trigger
	 */
	DMA_DoChannelSoftwareTrigger(CELLIOTSHIELD_DMA, CELLIOTSHIELD_DMA_RX_CH);

	return kStatus_Success;
}

int CELLIOTSHIELD_TimerConfig( void )
{
	mrt_config_t mrtConfig;

	/* Get default config */
	MRT_GetDefaultConfig(&mrtConfig);

	/* Init MRT module & enable MRT clock */
	MRT_Init(MRT0, &mrtConfig);

	/* Setup Channel 0 to be repeated */
	MRT_SetupChannelMode(MRT0, kMRT_Channel_0, kMRT_OneShotMode);

	/* Enable timer interrupts for channel 0 */
	MRT_EnableInterrupts(MRT0, kMRT_Channel_0, kMRT_TimerInterruptEnable);

	/* Enable at the NVIC */
	EnableIRQ(MRT0_IRQn);

	return kStatus_Success;
}

/*!
 * @brief Power up WiFi shield, RTOS does not have to run yet
 */
bool CELLIOTSHIELD_PowerUp(uint32_t enable)
{
//    if (enable)
//    {
//        GPIO_PinWrite(WIFISHIELD_WLAN_PWRON_GPIO, WIFISHIELD_WLAN_PWRON_PORT, WIFISHIELD_WLAN_PWRON_PIN, 1);
//    }
//    else
//    {
//        GPIO_PinWrite(WIFISHIELD_WLAN_PWRON_GPIO, WIFISHIELD_WLAN_PWRON_PORT, WIFISHIELD_WLAN_PWRON_PIN, 0);
//    }
      return true;
}
