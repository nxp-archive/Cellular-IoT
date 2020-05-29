/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "nbiot_common.h"
#include "cust_usart_com.h"
#include "gm01q_api.h"
#include <FreeRTOS.h>
#include <event_groups.h>

#define USART_TASK_PRIORITY ((configMAX_PRIORITIES)-2)
#define USART_TASK_STACKSIZE 1024U

// This is A_CUSTOM_DRIVER_CONTEXT, the top-level driver context.
// It contains pCommonCxt which points to the internal (aka "common")
// driver context (of type A_DRIVER_CONTEXT)
void *p_Global_Context = NULL;

SQNS_STATUS NBIOT_USART_Ressources_Init(void)
{
	SQNS_STATUS status = SQNS_SUCCESS;

//	do
//	{
//		status = NBIOT_USART_Context_Init();
//		if(SQNS_SUCCESS != status)
//		{
//			status = SQNS_ERROR;
//			break;
//		}
//
//			break;
//			status = SQNS_ERROR;
//		{
//		}
//		if(SQNS_SUCCESS != status)
//		status = NBIOT_USART_Handler_Init();
//
//	}while(0);

	return status;
}

SQNS_STATUS NBIOT_USART_Context_Init(void)
{
	SQNS_STATUS status = SQNS_SUCCESS;

	do
	{
		/* allocate the driver context and assign it to the qca_ptr mac_param */
		if (NULL == (p_Global_Context = (void *)A_MALLOC(sizeof(A_CUSTOM_DRIVER_CONTEXT))))
		{
			status = SQNS_ERROR;
			break;
		}

		memset(p_Global_Context, 0, sizeof(A_CUSTOM_DRIVER_CONTEXT));

		if (NULL == (GET_DRIVER_CXT(p_Global_Context)->sendSemaphore = xSemaphoreCreateMutex()))
		{
			status = SQNS_ERROR;
			break;
		}

		if (NULL == (GET_DRIVER_CXT(p_Global_Context)->receiveSemaphore = xSemaphoreCreateMutex()))
		{
			vSemaphoreDelete(GET_DRIVER_CXT(p_Global_Context)->sendSemaphore);
			status = SQNS_ERROR;
			break;
		}

		if (NULL == (GET_DRIVER_CXT(p_Global_Context)->q_txQueue = xQueueCreate((UBaseType_t)NBIOTSHIELD_USART_QUEUE_SIZE, (UBaseType_t)NBIOTSHIELD_USART_BUFFER_SIZE)))
		{
			vSemaphoreDelete(GET_DRIVER_CXT(p_Global_Context)->sendSemaphore);
			vSemaphoreDelete(GET_DRIVER_CXT(p_Global_Context)->receiveSemaphore);
			status = SQNS_ERROR;
			break;
		}

		if (NULL == (GET_DRIVER_CXT(p_Global_Context)->q_rxQueue = xQueueCreate((UBaseType_t)NBIOTSHIELD_USART_QUEUE_SIZE, (UBaseType_t)NBIOTSHIELD_USART_BUFFER_SIZE)))
		{
			vSemaphoreDelete(GET_DRIVER_CXT(p_Global_Context)->sendSemaphore);
			vSemaphoreDelete(GET_DRIVER_CXT(p_Global_Context)->receiveSemaphore);
			vQueueDelete(GET_DRIVER_CXT(p_Global_Context)->q_txQueue);
			status = SQNS_ERROR;
			break;
		}
		if (NULL == (GET_DRIVER_CXT(p_Global_Context)->e_UsartEvent = xEventGroupCreate()))
		{
			vSemaphoreDelete(GET_DRIVER_CXT(p_Global_Context)->sendSemaphore);
			vSemaphoreDelete(GET_DRIVER_CXT(p_Global_Context)->receiveSemaphore);
			vQueueDelete(GET_DRIVER_CXT(p_Global_Context)->q_txQueue);
			vQueueDelete(GET_DRIVER_CXT(p_Global_Context)->q_rxQueue);
			status = SQNS_ERROR;
			break;
		}
		//GET_DRIVER_CXT(p_Global_Context)->p_UsartHandle = &NBIOTSHIELD_USART_RTOS_HANDLE;

	}while(0);

	return status;
}

SQNS_STATUS NBIOT_USART_Handler_Init(void)
{
	BaseType_t status;

	status = xTaskCreate(prvUSARTHandlerTask, "USART_Handler_Task", USART_TASK_STACKSIZE, NULL, USART_TASK_PRIORITY, NULL);
	return pdPASS == status ? SQNS_SUCCESS : SQNS_ERROR;
}

void prvUSARTHandlerTask(void *pvParameters)
{
	(void)pvParameters;
//	const TickType_t xTicksToWait = pdMS_TO_TICKS( 100 );
	const TickType_t xTicksToWaitForEvent = pdMS_TO_TICKS( 1000 );
//	BaseType_t xStatus;
//	EventBits_t ev = 0;

	for( ;; )
	{
		/* Wait for any data to process or for the next time round (1s) */
//		ev = xEventGroupWaitBits(GET_DRIVER_CXT(p_Global_Context)->e_UsartEvent, RTOS_USART_SEND_PENDING || RTOS_USART_RECEIVE_PENDING, pdTRUE, pdFALSE, xTicksToWaitForEvent);

		/* Check if data needs to be transmitted */
		//xStatus = xQueueReceive( GET_DRIVER_CXT(p_Global_Context)->q_txQueue, &GET_DRIVER_CXT(p_Global_Context)->sendXfer, xTicksToWait );
		/* If the queue is not empty, send the data over the USART */
		//if(pdPASS == xStatus)
//		if(RTOS_USART_SEND_PENDING == ( ev & RTOS_USART_SEND_PENDING) )
//		{
//			xSemaphoreTake( GET_DRIVER_CXT(p_Global_Context)->sendSemaphore, 0);
//			while (kStatus_Success!= USART_RTOS_Send(&NBIOTSHIELD_USART_RTOS_HANDLE, GET_DRIVER_CXT(p_Global_Context)->sendXfer.data, GET_DRIVER_CXT(p_Global_Context)->sendXfer.dataSize))
//			{}
//			xSemaphoreGive( GET_DRIVER_CXT(p_Global_Context)->sendSemaphore );
//		}

		/* Check if data needs to be read */
//		xStatus = xQueueReceive( GET_DRIVER_CXT(p_Global_Context)->q_rxQueue, &GET_DRIVER_CXT(p_Global_Context)->receiveXfer, xTicksToWait );
//		/* If the queue is not empty, send the data over the USART */
//		if(pdPASS == xStatus)
//		{
//			//USART_RTOS_Receive(&NBIOTSHIELD_USART_RTOS_HANDLE, NBIOTSHIELD_USART_RX_BUFFER, 2, &n);
//		}
//		if(RTOS_USART_SEND_PENDING == ( ev & RTOS_USART_SEND_PENDING) )
//		{
//			xSemaphoreTake( GET_DRIVER_CXT(p_Global_Context)->receiveSemaphore, 0);
//			while (kStatus_Success!= USART_RTOS_Receive(&NBIOTSHIELD_USART_RTOS_HANDLE, GET_DRIVER_CXT(p_Global_Context)->sendXfer.data, GET_DRIVER_CXT(p_Global_Context)->sendXfer.dataSize))
//			{}
//			xSemaphoreGive( GET_DRIVER_CXT(p_Global_Context)->receiveSemaphore );
//		}
		vTaskDelay(xTicksToWaitForEvent);
	}
	USART_RTOS_Deinit(&NBIOTSHIELD_USART_RTOS_HANDLE);
	NBIOT_USART_Context_Deinit();
	vTaskDelete( NULL );
}

SQNS_STATUS NBIOT_USART_Context_Deinit(void)
{
	vSemaphoreDelete(GET_DRIVER_CXT(p_Global_Context)->sendSemaphore);
	vSemaphoreDelete(GET_DRIVER_CXT(p_Global_Context)->receiveSemaphore);
	vQueueDelete(GET_DRIVER_CXT(p_Global_Context)->q_txQueue);
	vQueueDelete(GET_DRIVER_CXT(p_Global_Context)->q_rxQueue);
	vEventGroupDelete(GET_DRIVER_CXT(p_Global_Context)->e_UsartEvent);
	//GET_DRIVER_CXT(p_Global_Context)->p_UsartHandle = NULL;

	return SQNS_SUCCESS;
}
