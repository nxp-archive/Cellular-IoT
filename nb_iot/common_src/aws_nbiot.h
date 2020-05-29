/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file aws_nbiot.h
 * @brief NB-IoT Interface.
 */

#ifndef _AWS_NBIOT_H_
#define _AWS_NBIOT_H_

/**
 * @brief Return code denoting API status.
 *
 * @note Codes other than eNbIoTSuccess are failure codes.
 */
typedef enum
{
    eNbIoTSuccess = 0,      /**< Success. */
    eNbIoTFailure = 1,      /**< Failure. */
    eNbIoTTimeout = 2,      /**< Timeout. */
    eNbIoTNotSupported = 3, /**< Not supported. */
} NBIOTReturnCode_t;

NBIOTReturnCode_t NBIOT_On( uint32_t baudrate );

#endif
