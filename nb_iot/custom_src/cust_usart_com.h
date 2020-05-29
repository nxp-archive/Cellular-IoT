/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CUSTOM_SRC_CUST_USART_COM_H_
#define CUSTOM_SRC_CUST_USART_COM_H_

#include "sqns_structs.h"

SQNS_STATUS NBIOT_USART_Ressources_Init(void);
SQNS_STATUS NBIOT_USART_Context_Init(void);
SQNS_STATUS NBIOT_USART_Handler_Init(void);
void prvUSARTHandlerTask(void *pvParameters);
SQNS_STATUS NBIOT_USART_Context_Deinit(void);

#endif /* CUSTOM_SRC_CUST_USART_COM_H_ */
