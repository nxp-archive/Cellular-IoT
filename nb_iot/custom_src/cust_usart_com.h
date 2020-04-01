/*
 * cust_usart_com.h
 *
 *  Created on: 17 oct. 2019
 *      Author: nxf37702
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
