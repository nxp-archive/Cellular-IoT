/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file aws_CellIoT.h
 * @brief Cellular-IoT Interface.
 */

#ifndef _AWS_CELLIOT_H_
#define _AWS_CELLIOT_H_

/**
 * @brief Return code denoting API status.
 *
 * @note Codes other than eCellIoTSuccess are failure codes.
 */
typedef enum
{
    eCellIoTSuccess = 0,      /**< Success. */
    eCellIoTFailure = 1,      /**< Failure. */
    eCellIoTTimeout = 2,      /**< Timeout. */
    eCellIoTNotSupported = 3, /**< Not supported. */
} CELLIOTReturnCode_t;

CELLIOTReturnCode_t CELLIOT_On( uint32_t baudrate );

#endif
