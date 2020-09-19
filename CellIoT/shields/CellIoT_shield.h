/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __CELLIOT_SHIELD_H__
#define __CELLIOT_SHIELD_H__

#include "FreeRTOS.h"
#include "CellIoT_shield_common.h"

/* Select specific shield support */
#ifndef CELLIOTSHIELD_IS
#define CELLIOTSHIELD_IS CELLULAR_IOT_SHIELD_IS_MONARCHGO_SHIELD
#endif

/* Include shields support */
#if defined(CELLIOTSHIELD_IS)
#   if (CELLIOTSHIELD_IS) == (CELLULAR_IOT_SHIELD_IS_SKYWIRE)
#       include "CellIoT_shield_skywire.h"
#   elif (CELLIOTSHIELD_IS) == (CELLULAR_IOT_SHIELD_IS_MONARCHGO_SHIELD)
#       include "CellIoT_shield_MonarchGoArduinoShield.h"
#   else
#       error "Unsupported shield selected !"
#   endif
#else
#   error "No shield is selected !"
#endif

#include "CellIoT_env_port.h"

/* define IRQ priority level */
#ifndef CELLIOTSHIELD_UART_IRQ_PRIORITY
#   define CELLIOTSHIELD_UART_IRQ_PRIORITY (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1)
#endif
#ifndef CELLIOTSHIELD_DMA_IRQ_PRIORITY
#   define CELLIOTSHIELD_DMA_IRQ_PRIORITY (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1)
#endif
#ifndef CELLIOTSHIELD_WLAN_IRQ_PRIORITY
#   define CELLIOTSHIELD_WLAN_IRQ_PRIORITY (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1)
#endif

/* Fn prototypes, which need to be implemented */
int CELLIOTSHIELD_Init( uint32_t baudrate );
int CELLIOTSHIELD_USARTConfig( uint32_t baudrate );
int CELLIOTSHIELD_DMAConfig( void );
int CELLIOTSHIELD_TimerConfig( void );

#endif
