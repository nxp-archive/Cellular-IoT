/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file aws_CellIoT.c
 * @brief Cellular-IoT Interface.
 */

#include <stdio.h>
#include "FreeRTOS.h"
#include "aws_CellIoT.h"
#include "CellIoT_common.h"
#include "CellIoT_lib.h"
#include "semphr.h"
//#include "CellIoT_env_port.h"
#include "cust_usart_com.h"

/* This is here because the maximum DNS name length is defined in aws_secure_sockets.h.
 * Cellular-IoT must not have a dependency on aws_secure_sockets.h
 */
#define CellIoTMAX_DNS_NAME_LENGTH 253

/* Only 1 Cellular-IoT module is present at the time */
//static uint8_t g_devid = 0;

/* Semaphore for connection */
static SemaphoreHandle_t g_connect_semaph;

/* Protect API */
static SemaphoreHandle_t g_CellIoT_semaph;

/* Cellular-IoT status - turned on */
static uint8_t g_CellIoT_is_on = 0;

/* Cellular-IoT status - turned on */
//static uint8_t g_connected = 0;

/**
 * @brief Initializes the Cellular-IoT module.
 *
 * This function must be called exactly once before any other
 * Cellular-IoT functions (including socket functions) can be used.
 *
 * @return eCellIoTSuccess if everything succeeds, eCellIoTFailure otherwise.
 */
CELLIOTReturnCode_t CELLIOT_On( uint32_t baudrate )
{
    bool result;

    /* Prevent re-initialization. Cellular-IoT is already on this is successful. */
    if (g_CellIoT_is_on)
        return eCellIoTSuccess;

    /* Initialize Cellular-IoT shield communication */
    result = CELLIOTSHIELD_Init(baudrate);
    if (kStatus_Success != result)
        return eCellIoTFailure;

    /* Power off the module and wait 30ms */
    //CELLIOTSHIELD_PowerUp(false);	/*TODO:Later, have a look at this */
    vTaskDelay(MSEC_TO_TICK(30));

    /* Create a on_CellIoT semaphore, */
    g_CellIoT_semaph = xSemaphoreCreateBinary();
    if (NULL == g_CellIoT_semaph)
        return eCellIoTFailure;
    xSemaphoreGive(g_CellIoT_semaph);

    /* Create a on_connect semaphore, */
    g_connect_semaph = xSemaphoreCreateBinary();
    if (NULL == g_connect_semaph)
        return eCellIoTFailure;

    /* Wait for Cellular-IoT */
    vTaskDelay(MSEC_TO_TICK(100));
    g_CellIoT_is_on = 1;

    return eCellIoTSuccess;
}
