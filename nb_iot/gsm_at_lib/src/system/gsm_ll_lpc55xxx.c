/**
 * \file            gsm_ll_stm32.c
 * \brief           Generic STM32 driver, included in various STM32 driver variants
 */

/*
 * Copyright (c) 2019 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of GSM-AT library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v0.6.0
 */

/*
 * How it works
 *
 * On first call to \ref gsm_ll_init, new thread is created and processed in usart_ll_thread function.
 * USART is configured in RX DMA mode and any incoming bytes are processed inside thread function.
 * DMA and USART implement interrupt handlers to notify main thread about new data ready to send to upper layer.
 *
 * \ref GSM_CFG_INPUT_USE_PROCESS must be enabled in `gsm_config.h` to use this driver.
 */
#include "gsm/gsm.h"
#include "gsm/gsm_mem.h"
#include "gsm/gsm_input.h"
#include "system/gsm_ll.h"
#include "nbiot_common.h"
#include "aws_nbiot.h"
#include "gm01q_tools.h"
#include "fsl_usart_dma.h"
#include "FreeRTOS.h"
#include "fsl_mrt.h"
#include "fsl_debug_console.h"

#if !__DOXYGEN__

#if !GSM_CFG_INPUT_USE_PROCESS
#error "GSM_CFG_INPUT_USE_PROCESS must be enabled in `gsm_config.h` to use this driver."
#endif /* GSM_CFG_INPUT_USE_PROCESS */

#if !defined(GSM_USART_DMA_RX_BUFF_SIZE)
#define GSM_USART_DMA_RX_BUFF_SIZE      0x1000
#endif /* !defined(GSM_USART_DMA_RX_BUFF_SIZE) */

#if !defined(GSM_MEM_SIZE)
#define GSM_MEM_SIZE                    0x1000
#endif /* !defined(GSM_MEM_SIZE) */

#if !defined(GSM_USART_RDR_NAME)
#define GSM_USART_RDR_NAME              RDR
#endif /* !defined(GSM_USART_RDR_NAME) */

#define USART_TASK_PRIORITY ( (configMAX_PRIORITIES) - 1U )
#define USART_TASK_STACKSIZE 1024

#define USART_IRQHandler FLEXCOMM2_IRQHandler

/* USART memory */
//static uint8_t      usart_mem[GSM_USART_DMA_RX_BUFF_SIZE];
static uint8_t      is_running, initialized;
static size_t       old_pos;

/* USART thread */
void usart_ll_thread(void * arg);
static gsm_sys_thread_t usart_ll_thread_id = NULL;

/* Message queue */
static gsm_sys_mbox_t usart_ll_mbox_id;


static size_t old_pos;
volatile uint8_t * ptr_cur_buff;
volatile uint8_t * ptr_prv_buff;
volatile uint8_t buff_jump = 0;

/**
 * \brief           USART data processing
 */
void
usart_ll_thread(void * arg) {
	BaseType_t evt;
	uint8_t* mbox_data;
    size_t pos;

    GSM_UNUSED(arg);

    while (1) {
        /* Wait for the event message from DMA or USART */
        evt = gsm_sys_mbox_get(&usart_ll_mbox_id, (void **)&mbox_data, portMAX_DELAY);

        if (evt == pdFALSE)
        {
        	continue;
        }

        /* Calculate the new next available position in the RX buffer */
		pos = sizeof(NBIOTSHIELD_USART_RX_BUFFER) - 1 - DMA_GetRemainingBytes(NBIOTSHIELD_DMA_RXHANDLE.base, NBIOTSHIELD_DMA_RXHANDLE.channel);

        if (pos != old_pos && is_running) {
            if (pos > old_pos && buff_jump == 0) {
                gsm_input_process((void*)&ptr_cur_buff[old_pos], pos - old_pos);
            }
            else
            {
            	/* Check if the data filled by the DMA has overloaded the next buffer
            	 * Which results to come back to the original buffer
            	 * Hence counting the number of buffer changes
            	 */
            	if(buff_jump > 1)
            	{
            		gsm_input_process((void*)&ptr_cur_buff[old_pos], sizeof(NBIOTSHIELD_USART_RX_BUFFER) - 1 - old_pos);
            		gsm_input_process((void*)&ptr_prv_buff[0], sizeof(NBIOTSHIELD_USART_RX_BUFFER) - 1);
            	}
            	else
            	{
					gsm_input_process((void*)&ptr_prv_buff[old_pos], sizeof(NBIOTSHIELD_USART_RX_BUFFER) - 1 - old_pos);
            	}

				if (pos > 0) {
					gsm_input_process((void*)&ptr_cur_buff[0], pos);
				}
				buff_jump = 0;
            }
            old_pos = pos;
            if (old_pos == sizeof(NBIOTSHIELD_USART_RX_BUFFER)) {
                old_pos = 0;
            }
        }
    }
}

/**
 * \brief           Configure UART using DMA for receive in double buffer mode and IDLE line detection
 */
void
configure_uart(uint32_t baudrate) {
	uint8_t result;

	configPRINTF(("Starting NB-IoT Hardware configuration...\r\n"));

	result = (uint8_t)NBIOT_On(baudrate);
	if (result != eNbIoTSuccess)
	{
		configPRINTF(("Could not enable NB-IoT, reason %d.\r\n", result));
		return;
	}

	ptr_cur_buff = NBIOTSHIELD_USART_RX_BUFFER;
	ptr_prv_buff = NBIOTSHIELD_USART_RING_BUFFER;

	configPRINTF(("NB-IoT Hardware configuration initialized.\r\n"));

	old_pos = 0;
	is_running = 1;

	/* Create mbox and start thread */
	if (usart_ll_mbox_id == NULL) {
		result = gsm_sys_mbox_create(&usart_ll_mbox_id, 10U, sizeof(uint8_t));
		if (result != 1U)
		{
			configPRINTF(("Could not create mailbox for USART communication, reason %d.\r\n", result));
			return;
		}
	}
	if (usart_ll_thread_id == NULL) {
		result = gsm_sys_thread_create(&usart_ll_thread_id, "USART_Handler_Task", usart_ll_thread, NULL, USART_TASK_STACKSIZE, USART_TASK_PRIORITY);
		if (result != 1U)
		{
			configPRINTF(("Could not create task for USART communication, reason %d.\r\n", result));
			return;
		}
	}
}

#if defined(GSM_RESET_PIN)
/**
 * \brief           Hardware reset callback
 */
static uint8_t
reset_device(uint8_t state) {
    if (state) {                                /* Activate reset line */
        LL_GPIO_ResetOutputPin(GSM_RESET_PORT, GSM_RESET_PIN);
    } else {
        LL_GPIO_SetOutputPin(GSM_RESET_PORT, GSM_RESET_PIN);
    }
    return 1;
}
#endif /* defined(GSM_RESET_PIN) */

/**
 * \brief           Send data to GSM device
 * \param[in]       data: Pointer to data to send
 * \param[in]       len: Number of bytes to send
 * \return          Number of bytes sent
 */
static size_t
send_data(const void* data, size_t len) {
	usart_transfer_t sendXfer;
#if SERIAL_DEBUG
	char * ptx = (char *)data;
	for(int i=0;i<len;i++)
	{
		PRINTF("%c",*ptx);
		ptx++;
	}
#endif
	if (len >0)
	{
		do
		{
			if( len > 1024 )
			{
				sendXfer.data = (uint8_t *)data;
				sendXfer.dataSize = 1024;

				data += 1024;
				len -= 1024;
			}
			else
			{
				sendXfer.data = (uint8_t *)data;
				sendXfer.dataSize = len;
				len = 0;
			}

			while( USART_TransferSendDMA(NBIOTSHIELD_USART, &NBIOTSHIELD_DMA_HANDLE, &sendXfer) == kStatus_USART_TxBusy );
		}while( len > 0 );
	}
    return len;
}

/**
 * \brief           Callback function called from initialization process
 * \note            This function may be called multiple times if AT baudrate is changed from application
 * \param[in,out]   ll: Pointer to \ref gsm_ll_t structure to fill data for communication functions
 * \param[in]       baudrate: Baudrate to use on AT port
 * \return          Member of \ref gsmr_t enumeration
 */
gsmr_t
gsm_ll_init(gsm_ll_t* ll) {
#if !GSM_CFG_MEM_CUSTOM
    static uint8_t memory[GSM_MEM_SIZE];
    gsm_mem_region_t mem_regions[] = {
        { memory, sizeof(memory) }
    };

    if (!initialized) {
        gsm_mem_assignmemory(mem_regions, GSM_ARRAYSIZE(mem_regions));  /* Assign memory for allocations */
    }
#endif /* !GSM_CFG_MEM_CUSTOM */

    if (!initialized) {
        ll->send_fn = send_data;                /* Set callback function to send data */
#if defined(GSM_RESET_PIN)
        ll->reset_fn = reset_device;            /* Set callback for hardware reset */
#endif /* defined(GSM_RESET_PIN) */
    }

    configure_uart(ll->uart.baudrate);          /* Initialize UART for communication */
    initialized = 1;
    return gsmOK;
}

/**
 * \brief           Callback function to de-init low-level communication part
 * \param[in,out]   ll: Pointer to \ref gsm_ll_t structure to fill data for communication functions
 * \return          \ref gsmOK on success, member of \ref gsmr_t enumeration otherwise
 */
gsmr_t
gsm_ll_deinit(gsm_ll_t* ll) {
    if (usart_ll_mbox_id != NULL) {
    	gsm_sys_mbox_t tmp = usart_ll_mbox_id;
        usart_ll_mbox_id = NULL;
        gsm_sys_mbox_delete(&tmp);
    }
    if (usart_ll_thread_id != NULL) {
    	gsm_sys_thread_t tmp = usart_ll_thread_id;
        usart_ll_thread_id = NULL;
        gsm_sys_thread_terminate(&tmp);
    }
    initialized = 0;
    GSM_UNUSED(ll);
    return gsmOK;
}

/**
 * \brief           UART User Callback
 */

/* USART user callback */
void USART_UserCallback(USART_Type *base, usart_dma_handle_t *handle, status_t status, void *userData)
{
//	userData = userData;
//
//	if (kStatus_USART_TxIdle == status)
//	{
//		if (usart_ll_mbox_id != NULL)
//		{
//			uint8_t mbox_msg = 0;
//			gsm_sys_mbox_put(&usart_ll_mbox_id, &mbox_msg);
//		}
//	}
//
//	if (kStatus_USART_RxIdle == status)
//	{
//		if (usart_ll_mbox_id != NULL)
//		{
//			uint8_t mbox_msg = 0;
//			gsm_sys_mbox_put(&usart_ll_mbox_id, &mbox_msg);
//		}
//	}
}

/**
 * \brief           UART DMA stream/channel handler
 */

void DMA_Callback(dma_handle_t *handle, void *param, bool transferDone, uint32_t tcds)
{
    if (tcds == kDMA_IntA)
    {
    	/* Descriptor has expired so the current buffer has changed */
    	ptr_cur_buff = NBIOTSHIELD_USART_RING_BUFFER;
    	ptr_prv_buff = NBIOTSHIELD_USART_RX_BUFFER;
    }
    else if (tcds == kDMA_IntB)
	{
		/* Descriptor has expired so the current buffer has changed */
		ptr_cur_buff = NBIOTSHIELD_USART_RX_BUFFER;
		ptr_prv_buff = NBIOTSHIELD_USART_RING_BUFFER;
	}

    /* Follow the number of buffer changes */
    buff_jump++;
}

/**
 * \brief           UART global interrupt handler
 */

void USART_IRQHandler(void)
{
      /* Data has arrived since the rxWatermark is set to kUSART_RxFifo1 */
//	if (usart_ll_mbox_id != NULL)
//	{
//		uint8_t mbox_msg = 0;
//		//gsm_sys_mbox_put(&usart_ll_mbox_id, &mbox_msg);
//	    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//	    xQueueSendToBackFromISR(usart_ll_mbox_id, &mbox_msg, &xHigherPriorityTaskWoken); /* Put new message for ISR API */
//		/* Now the buffer is empty we can switch context if necessary. */
//		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//	}

/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void MRT0_IRQHandler(void)
{
    /* Clear interrupt flag.*/
    MRT_ClearStatusFlags(MRT0, kMRT_Channel_0, kMRT_TimerInterruptFlag);

	if (usart_ll_mbox_id != NULL)
	{
		size_t pos;
		/* Calculate the new next available position in the RX buffer */
		pos = sizeof(NBIOTSHIELD_USART_RX_BUFFER) - 1 - DMA_GetRemainingBytes(NBIOTSHIELD_DMA_RXHANDLE.base, NBIOTSHIELD_DMA_RXHANDLE.channel);

		if (pos != old_pos && is_running)
		{
			uint8_t mbox_msg = 0;
			BaseType_t xHigherPriorityTaskWoken = pdFALSE;
			xQueueSendToBackFromISR(usart_ll_mbox_id, &mbox_msg, &xHigherPriorityTaskWoken); /* Put new message for ISR API */
			/* Now the buffer is empty we can switch context if necessary. */
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
	}
}

void Timer_CallbackHandler( uint32_t flags )
{
	/* Stop the timer */
	CTIMER0->TCR &= ~CTIMER_TCR_CEN_MASK;
}

#endif /* !__DOXYGEN__ */
