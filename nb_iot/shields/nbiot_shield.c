/*
 * NXP Header file To be Defined
 */

/**
 * @file nbiot_shield.c
 * @brief NB-IoT Shield Interface.
 */
#include "nbiot_shield.h"
#include "gm01q_tools.h"
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
const uint32_t timerRegisterValues[2] = { /*0x80000100*/ 0x800F4240 , 0x11000000 };

uint32_t testtimerRegisterValues[2] = { 0x00000000, 0x00000000};
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
int NBIOTSHIELD_Init( uint32_t baudrate )
{

	/* Use 12 MHz clock for some of the Ctimers */
	//CLOCK_AttachClk(kFRO_HF_to_CTIMER0);

	/* Initialise the DMA configuration */
	if (kStatus_Success != NBIOTSHIELD_TimerConfig())
		return kStatus_InvalidArgument;

    /* Initialize pinmux */
#ifdef NBIOTSHIELD_PINMUX_INIT
    NBIOTSHIELD_PINMUX_INIT();
#endif

    /* Enable clocks for USART */
	CLOCK_AttachClk(NBIOTSHIELD_USART_CLK_CONNECT);
	RESET_PeripheralReset(NBIOTSHIELD_USART_IP_RESET);

	/* Set NVIC priority if is required by Freertos */
	//NVIC_SetPriority(NBIOTSHIELD_USART_IRQ, NBIOTSHIELD_UART_IRQ_PRIORITY);

    /* Initialise the USART configuration */
    if (kStatus_Success != NBIOTSHIELD_USARTConfig(baudrate))
    	return kStatus_InvalidArgument;

    /* Initialise the DMA configuration */
	if (kStatus_Success != NBIOTSHIELD_DMAConfig())
		return kStatus_InvalidArgument;

    //USART_EnableCTS(NBIOTSHIELD_USART, true);
    /* Enable NVIC interrupt for USART 2 */
    //USART_TransferCreateHandle(NBIOTSHIELD_USART, &NBIOTSHIELD_USART_HANDLE, USART_UserCallback, NULL); /* Link Callback and enable IRQ */
	//USART_TransferStartRingBuffer(NBIOTSHIELD_USART, &NBIOTSHIELD_USART_HANDLE, NBIOTSHIELD_USART_RING_BUFFER, AT_BUFFER_SIZE);	/* Set the RX Ring Buffer and the IRQs masks */
    //EnableIRQ(NBIOTSHIELD_USART_IRQ);

    /* Enable RX interrupt UART2 */
	//USART_EnableInterrupts(NBIOTSHIELD_USART, NBIOTSHIELD_USART_IRQ_MASK);

	/* Test to see if the USART is well configured */
	//USART_RTOS_Send(&NBIOTSHIELD_USART_RTOS_HANDLE, (uint8_t *)to_send, strlen(to_send));

    return kStatus_Success;
}

/**
 * @brief Configure the USART for the NB-IoT module.
 */
int NBIOTSHIELD_USARTConfig( uint32_t baudrate )
{
	//struct rtos_usart_config usart_config;
	usart_config_t usart_config;

	/* Initialize the UART. */
	/*
	 * config.baudRate_Bps = 115200U;
	 * config.parityMode = kUART_ParityDisabled;
	 * config.stopBitCount = kUART_OneStopBit;
	 * config.txFifoWatermark = 0;
	 * config.rxFifoWatermark = 1;
	 * config.enableTx = false;
	 * config.enableRx = false;
	 */
	USART_GetDefaultConfig(&usart_config);
//	usart_config.base 		 = NBIOTSHIELD_USART;
//	usart_config.srcclk		 = CLOCK_GetFreq(NBIOTSHIELD_USART_CLK);
//	usart_config.baudrate    = NBIOTSHIELD_USART_BAUDRATE;
//	usart_config.parity      = kUSART_ParityDisabled;
//	usart_config.stopbits    = kUSART_OneStopBit;
	//usart_config.buffer      = NBIOTSHIELD_USART_RING_BUFFER;
	//usart_config.buffer_size = sizeof(NBIOTSHIELD_USART_RING_BUFFER);

	usart_config.baudRate_Bps = baudrate;
	usart_config.enableTx     = true;
	usart_config.enableRx     = true;

	USART_Init(NBIOTSHIELD_USART, &usart_config, CLOCK_GetFreq(NBIOTSHIELD_USART_CLK));

	USART_EnableCTS(NBIOTSHIELD_USART, true);

	/* Enable DMA request from rxFIFO */
	USART_EnableRxDMA(NBIOTSHIELD_USART, true);

	/* Enable RX interrupt. */
	//NVIC_SetPriority((IRQn_Type)NBIOTSHIELD_USART_IRQ, NBIOTSHIELD_USART_IRQ_PRIORITY);
	//NVIC_SetPriority(NBIOTSHIELD_USART_IRQ, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0x07, 0x00));
	//USART_EnableInterrupts(NBIOTSHIELD_USART, NBIOTSHIELD_USART_IRQ_MASK);
	//EnableIRQ(NBIOTSHIELD_USART_IRQ);
	//DisableIRQ(NBIOTSHIELD_USART_IRQ);

	//return USART_RTOS_Init(&NBIOTSHIELD_USART_RTOS_HANDLE, &NBIOTSHIELD_USART_HANDLE, &usart_config);
	return kStatus_Success;
}

/**
 * @brief Configure the DMA for the USART communication with the NB-IoT module.
 */
int NBIOTSHIELD_DMAConfig( void )
{
	dma_transfer_config_t transferConfig[2];
	dma_channel_trigger_t channel0HwTrg = { kDMA_FallingEdgeTrigger, kDMA_EdgeBurstTransfer1, kDMA_SrcWrap};
	dma_channel_trigger_t channel10HwTrg = { kDMA_FallingEdgeTrigger, kDMA_EdgeBurstTransfer1, kDMA_NoWrap};

	/* Configure DMA. */
	INPUTMUX_Init(INPUTMUX);
	DMA_Init(NBIOTSHIELD_DMA);
	DMA_EnableChannel(NBIOTSHIELD_DMA, NBIOTSHIELD_DMA_TX_CH);
	DMA_EnableChannel(NBIOTSHIELD_DMA, NBIOTSHIELD_DMA_RX_CH);
	DMA_EnableChannel(NBIOTSHIELD_DMA, NBIOTSHIELD_DMA_SPARE_CH);

	NVIC_SetPriority((IRQn_Type)NBIOTSHILED_DMA_IRQ, NBIOTSHIELD_DMA_IRQ_PRIORITY);
	//NVIC_SetPriority(NBIOTSHILED_DMA_IRQ, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0x07, 0x00));
	DMA_CreateHandle(&g_uartTxDmaHandle, NBIOTSHIELD_DMA, NBIOTSHIELD_DMA_TX_CH);
	DMA_CreateHandle(&g_uartRxDmaHandle, NBIOTSHIELD_DMA, NBIOTSHIELD_DMA_RX_CH);
	DMA_CreateHandle(&g_timerTransferHandle, NBIOTSHIELD_DMA, NBIOTSHIELD_DMA_SPARE_CH);

	DMA_DisableChannelInterrupts(NBIOTSHIELD_DMA, NBIOTSHIELD_DMA_RX_CH);
	//DMA_DisableChannelInterrupts(NBIOTSHIELD_DMA, NBIOTSHIELD_DMA_SPARE_CH);
	//DisableIRQ(NBIOTSHILED_DMA_IRQ);

	/* Create UART DMA handle. */
	USART_TransferCreateHandleDMA(NBIOTSHIELD_USART, &g_uartDmaHandle, USART_UserCallback, NULL, &g_uartTxDmaHandle,
								  NULL/*&g_uartRxDmaHandle*/);

	/* Set RX DMA Callback */
	//DMA_SetCallback(&g_uartRxDmaHandle, DMA_Callback, NULL);
	DMA_SetCallback(&g_timerTransferHandle, DMA_Callback, NULL);

	/* Prepare transfer. */
	/*DMA_PrepareTransfer(&transferConfig, ((void *)((uint32_t)&NBIOTSHIELD_USART->FIFORD)), NBIOTSHIELD_USART_RX_BUFFER, sizeof(uint8_t),
					sizeof(NBIOTSHIELD_USART_RX_BUFFER) - 1, kDMA_PeripheralToMemory, &g_pingpong_desc);
	*/
	DMA_PrepareTransfer(&transferConfig[0], (void *) timerRegisterValues, ((void *)((uint32_t)&MRT0->CHANNEL[0].INTVAL)), sizeof(uint32_t),
						sizeof(uint32_t), kDMA_MemoryToMemory, &g_pingpong_desc[0]);

	transferConfig[0].isPeriph= true;
	transferConfig[0].xfercfg.clrtrig = true;

	DMA_SubmitTransfer(&g_uartRxDmaHandle, &transferConfig[0]);

	DMA_SetChannelConfig(NBIOTSHIELD_DMA, NBIOTSHIELD_DMA_RX_CH, &channel10HwTrg, true);

	//transferConfig[0].xfercfg.intA = false;
	//transferConfig[0].xfercfg.intB = false;

    DMA_CreateDescriptor(&g_pingpong_desc[0], &transferConfig[0].xfercfg, (void *)timerRegisterValues,  ((void *)((uint32_t)&MRT0->CHANNEL[0].INTVAL)),
						 &g_pingpong_desc[0]);

    //DMA_SetChannelConfig(NBIOTSHIELD_DMA, NBIOTSHIELD_DMA_RX_CH, &channel10HwTrg, true);

	/* Attach the output of DMA0 Ch 10 to be DMA Output Trigger 0 */
	INPUTMUX_AttachSignal(INPUTMUX, 0U, kINPUTMUX_Dma0Flexcomm2RxTrigoutToTriginChannels);

	/* Attach the input of DMA0 Ch 0 to be DMA Output Trigger 0 */
	INPUTMUX_AttachSignal(INPUTMUX, NBIOTSHIELD_DMA_SPARE_CH, kINPUTMUX_Otrig0ToDma0);

	INPUTMUX_AttachSignal(INPUTMUX, NBIOTSHIELD_DMA_RX_CH, kINPUTMUX_Otrig0ToDma0);

	DMA_PrepareTransfer(&transferConfig[1], ((void *)((uint32_t)&NBIOTSHIELD_USART->FIFORD)), NBIOTSHIELD_USART_RX_BUFFER, sizeof(uint8_t),
						sizeof(NBIOTSHIELD_USART_RX_BUFFER) - 1, kDMA_PeripheralToMemory, &g_pingpong_desc[2]);

	transferConfig[1].isPeriph= false;
	//transferConfig[1].xfercfg.clrtrig = true;

	DMA_SubmitTransfer(&g_timerTransferHandle, &transferConfig[1]);

	DMA_SetChannelConfig(NBIOTSHIELD_DMA, NBIOTSHIELD_DMA_SPARE_CH, &channel0HwTrg, false);

	transferConfig[1].xfercfg.intA = true;
	transferConfig[1].xfercfg.intB = false;
	DMA_CreateDescriptor(&g_pingpong_desc[1], &transferConfig[1].xfercfg, (void *)&NBIOTSHIELD_USART->FIFORD, NBIOTSHIELD_USART_RX_BUFFER,
						 &g_pingpong_desc[2]);

	transferConfig[1].xfercfg.intA = false;
	transferConfig[1].xfercfg.intB = true;
	DMA_CreateDescriptor(&g_pingpong_desc[2], &transferConfig[1].xfercfg, (void *)&NBIOTSHIELD_USART->FIFORD, NBIOTSHIELD_USART_RING_BUFFER,
						 &g_pingpong_desc[1]);

	/* Attach the output of DMA0 Ch 0 to be DMA Output Trigger 1 */
	//INPUTMUX_AttachSignal(INPUTMUX, 1U, kINPUTMUX_Dma0Hash0TxTrigoutToTriginChannels);

	/* Attach the input of DMA0 Ch 0 to be DMA Output Trigger 0 */
	//INPUTMUX_AttachSignal(INPUTMUX, NBIOTSHIELD_DMA_RX_CH, kINPUTMUX_Otrig1ToDma0);

	/* Set the Software Trigger of DMA0 Ch 10
	 * The Trigger of DMA0 Ch 1 is an Hardware trigger
	 * Which is the output of DMA0 Ch 10
	 */
	DMA_EnableChannel(NBIOTSHIELD_DMA, NBIOTSHIELD_DMA_SPARE_CH);
	DMA_EnableChannel(NBIOTSHIELD_DMA, NBIOTSHIELD_DMA_RX_CH);

	DMA_DoChannelSoftwareTrigger(NBIOTSHIELD_DMA, NBIOTSHIELD_DMA_RX_CH);

	return kStatus_Success;
}

int NBIOTSHIELD_TimerConfig( void )
{
	/*
	ctimer_config_t timer_config;
	ctimer_match_config_t timerConfig;
	//static ctimer_callback_t * fncPtr = {Timer_CallbackHandler};

	//fncPtr = Timer_CallbackHandler;
	CTIMER_GetDefaultConfig(&timer_config);

	CTIMER_Init(CTIMER0, (const ctimer_config_t *) &timer_config);

	timerConfig.enableCounterReset = true;
	timerConfig.enableCounterStop  = false;
	timerConfig.matchValue         = CLOCK_GetFreq(kCLOCK_CTmier0) / 50;
	timerConfig.outControl         = kCTIMER_Output_NoAction;
	timerConfig.outPinInitState    = false;
	timerConfig.enableInterrupt    = true;

	CTIMER_RegisterCallBack(CTIMER0, ctimer_callback_table, kCTIMER_SingleCallback);
	CTIMER_SetupMatch(CTIMER0, kCTIMER_Match_0, &timerConfig);

	//CTIMER_StartTimer(CTIMER0);
*/
    mrt_config_t mrtConfig;

    /* mrtConfig.enableMultiTask = false; */
    MRT_GetDefaultConfig(&mrtConfig);

   /* Init mrt module */
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
bool NBIOTSHIELD_PowerUp(uint32_t enable)
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
