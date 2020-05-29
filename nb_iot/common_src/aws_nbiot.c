/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file aws_nbiot.c
 * @brief NB-IoT Interface.
 */

#include <stdio.h>
#include "FreeRTOS.h"
#include "aws_nbiot.h"
#include "nbiot_common.h"
#include "gm01q_api.h"
#include "semphr.h"
//#include "nbiot_env_port.h"
#include "cust_usart_com.h"

/* This is here because the maximum DNS name length is defined in aws_secure_sockets.h.
 * NB-IoT must not have a dependency on aws_secure_sockets.h
 */
#define nbiotMAX_DNS_NAME_LENGTH 253

/* Only 1 NB-IoT module is present at the time */
//static uint8_t g_devid = 0;

/* Semaphore for connection */
static SemaphoreHandle_t g_connect_semaph;

/* Protect API */
static SemaphoreHandle_t g_nbiot_semaph;

/* NB-IoT status - turned on */
static uint8_t g_nbiot_is_on = 0;

/* NB-IoT status - turned on */
//static uint8_t g_connected = 0;

/**
 * @brief Initializes the NB-IoT module.
 *
 * This function must be called exactly once before any other
 * NB-IoT functions (including socket functions) can be used.
 *
 * @return eNbIoTSuccess if everything succeeds, eNbIoTFailure otherwise.
 */
NBIOTReturnCode_t NBIOT_On( uint32_t baudrate )
{
    bool result;

    /* Prevent re-initialization. NB-IoT is already on this is successful. */
    if (g_nbiot_is_on)
        return eNbIoTSuccess;

    /* Initialize NB-IoT shield communication */
    result = NBIOTSHIELD_Init(baudrate);
    if (kStatus_Success != result)
        return eNbIoTFailure;

    /* Power off the module and wait 30ms */
    //NBIOTSHIELD_PowerUp(false);	/*TODO:Later, have a look at this */
    vTaskDelay(MSEC_TO_TICK(30));

    /* Create a on_nbiot semaphore, */
    g_nbiot_semaph = xSemaphoreCreateBinary();
    if (NULL == g_nbiot_semaph)
        return eNbIoTFailure;
    xSemaphoreGive(g_nbiot_semaph);

    /* Create a on_connect semaphore, */
    g_connect_semaph = xSemaphoreCreateBinary();
    if (NULL == g_connect_semaph)
        return eNbIoTFailure;

    /* Wait for NB-IoT */
    vTaskDelay(MSEC_TO_TICK(100));
    g_nbiot_is_on = 1;

    return eNbIoTSuccess;
}
