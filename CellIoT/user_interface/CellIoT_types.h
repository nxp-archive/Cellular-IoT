/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ATCMD_H_
#define ATCMD_H_


/************************************************/
/*            List of typedef enum              */
/************************************************/
typedef enum {
SQNS_CREG_0 = 0,  			/*	disable network registration unsolicited result code													*/
SQNS_CREG_1 = 1,  			/*	enable network registration unsolicited result code														*/
SQNS_CREG_2 = 2,  			/*	enable network registration and location information unsolicited result code							*/
SQNS_CREG_3 = 3				/*	enable network registration, location information and cause value information unsolicited result code	*/
} SQNS_CREGINTEGER;

typedef enum {
SQNS_CREG_STATUS_0 = 0,		/*	not registered, MT is not currently searching an operator to register to	*/
SQNS_CREG_STATUS_1 = 1,		/*	registered, home network													*/
SQNS_CREG_STATUS_2 = 2,		/*	not registered, but MT is currently searching a new operator to register to	*/
SQNS_CREG_STATUS_3 = 3,		/*	registration denied															*/
SQNS_CREG_STATUS_4 = 4,		/*	unknown (e.g. out of GERAN/UTRAN/E-UTRAN coverage)							*/
SQNS_CREG_STATUS_5 = 5		/*	registered, roaming															*/
} SQNS_CREGSTATUS;

typedef enum {
SQNS_CFUN_0 = 0,  			/*	minimum functionality									*/
SQNS_CFUN_1 = 1,  			/*	full functionality										*/
SQNS_CFUN_2 = 2,  			/*	disable phone transmit RF circuits only					*/
SQNS_CFUN_3 = 3,			/*	disable phone receive RF circuits only					*/
SQNS_CFUN_4 = 4				/*	disable phone both transmit and receive RF circuits		*/
} SQNS_CFUNCODE;

typedef enum {
SQNS_PSM_MODE_0 = 0,  		/*	power saving is disabled.											*/
SQNS_PSM_MODE_1 = 1,  		/*	power saving is activated, driven by UART0 							*/
SQNS_PSM_MODE_2 = 2  		/*	power saving is activated, driven by on of the UART0\UART1\UART2	*/
} SQNS_PSM_MODE;

typedef enum {
SQNS_PSM_DISABLED 			= 0,  		/*	Disable the use of PSM										*/
SQNS_SPSM_ENABLED 			= 1,  		/*	Enable the use of PSM 										*/
SQNS_SPSM_RESETDEFAULTVALUES = 2  		/*	Disable the use of PSM and discard all parameters for PSM	*/
} SQNS_PSM_STATUS;

typedef enum {
SQNS_MQTT_CERTIFICATE = 0,
SQNS_MQTT_PRIVATEKEY = 1
} SQNS_MQTT_CERTORKEY;

typedef enum {
SQNS_MQTT_ERR_SUCCESS		=  0,
SQNS_MQTT_ERR_NOMEM			= -1,
SQNS_MQTT_ERR_PROTOCOL		= -2,
SQNS_MQTT_ERR_INVAL			= -3,
SQNS_MQTT_ERR_NO_CONN		= -4,
SQNS_MQTT_ERR_CONN_REFUSED	= -5,
SQNS_MQTT_ERR_NOT_FOUND		= -6,
SQNS_MQTT_ERR_CONN_LOST		= -7,
SQNS_MQTT_ERR_TLS			= -8,
SQNS_MQTT_ERR_PAYLOAD_SIZE	= -9,
SQNS_MQTT_ERR_NOT_SUPPORTED	= -10,
SQNS_MQTT_ERR_AUTH			= -11,
SQNS_MQTT_ERR_ACL_DENIED	= -12,
SQNS_MQTT_ERR_UNKNOWN		= -13,
SQNS_MQTT_ERR_ERRNO			= -14,
SQNS_MQTT_ERR_EAI			= -15,
SQNS_MQTT_ERR_PROXY			= -16,
SQNS_MQTT_ERR_UNAVAILABLE	= -17
} SQNS_MQTT_ERR;


#endif /* ATCMD_H_ */
