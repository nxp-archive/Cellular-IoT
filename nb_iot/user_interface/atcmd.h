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


/************************************************/
/*                  Status Code                 */
/************************************************/
static const uint8_t rxOK[] = "\r\nOK\r\n";
static const uint8_t rxERROR[] = "\r\nERROR\r\n";
static const uint8_t rxPROMPT[] = "\r\n> ";


/************************************************/
/*               Response Expected              */
/************************************************/
static const uint8_t rxCEREG0[] =	"\r\n+CEREG: 0\r\n";
static const uint8_t rxCREG0[] =	"\r\n+CREG: 0\r\n";
static const uint8_t rxCEREG1[] =	"\r\n+CEREG: 1\r\n";
static const uint8_t rxCREG1[] =	"\r\n+CREG: 1\r\n";
static const uint8_t rxCEREG2[] =	"\r\n+CEREG: 2\r\n";
static const uint8_t rxCREG2[] =	"\r\n+CREG: 2\r\n";
static const uint8_t rxCEREG3[] =	"\r\n+CEREG: 3\r\n";
static const uint8_t rxCREG3[] =	"\r\n+CREG: 3\r\n";
static const uint8_t rxCEREG4[] =	"\r\n+CEREG: 4\r\n";
static const uint8_t rxCREG4[] =	"\r\n+CREG: 4\r\n";
static const uint8_t rxCEREG5[] =	"\r\n+CEREG: 5\r\n";
static const uint8_t rxCREG5[] =	"\r\n+CREG: 5\r\n";

static const uint8_t rxCREG[] =	"\r\n+CREG: ";

/************************************************/
/*                  AT Commands                 */
/************************************************/

static const uint8_t CMD_ATRESET[]		= "AT^RESET\r";				/* Performs a SW reset */
static const uint8_t urcSHUTDOWN[]		= "\r\n+SHUTDOWN\r\n";		/* This indicates that the ME has completed the reset procedure and will restart. */
static const uint8_t urcSYSTART[]		= "\r\n+SYSSTART\r\n";

static const uint8_t CMD_ATSQNSSHDN[]	= "AT+SQNSSHDN\r";		/* Device detach from the network and shutdown */
static const uint8_t urcSQNSSHDN[]		= "\r\n+SQNSSHDN\r\n";  /* This indicates that the power supply of the device can be safely cut. */

static const uint8_t CMD_AT[]			= "AT\r";
static const uint8_t CMD_ATE[] 		= "ATE\r";

static const uint8_t CMD_ATCREG[]		= "AT+CREG=";
static const uint8_t CMD_ATCFUN[]		= "AT+CFUN=";

static const uint8_t REQ_ATCPIN[]		= "AT+CPIN?\r";
static const uint8_t urcATCPIN[]		= "\r\n+CPIN: ";

/*UART Interface Power Saving Configuration*/
static const uint8_t CMD_ATSQNIPSCFG[]		= "AT+SQNIPSCFG=";
static const uint8_t REQ_ATSQNIPSCFG[]		= "AT+SQNIPSCFG?\r";
static const uint8_t REQ_ATSQNIPSCFG2[]	= "AT+SQNIPSCFG=?\r";
static const uint8_t urcSQNIPSCFG[]		= "\r\n+SQNIPSCFG:";

/*Modem State CMD*/
static const uint8_t CMD_ATCPSMS[]		= "AT+CPSMS=";
static const uint8_t REQ_ATCPSMS[]		= "AT+CPSMS?\r";
static const uint8_t urcCPSMS[]		= "\r\n+CPSMS:";

/* PING address */
static const uint8_t CMD_ATPING[]		= "AT+PING=";
static const uint8_t urcPING1[]		= "\r\n+PING: 1";

/*old Ping Command*/
static const uint8_t CMD_ATPINGMYAWS[] 	= "AT+PING=\"a33uj8le8muz2-ats.iot.us-east-1.amazonaws.com\"\r";
static const uint8_t CMD_ATPINGSNWAWS[]	= "AT+PING=\"sequans.com\"\r";

/* Conformance Test Mode */
static const uint8_t CMD_ATSQNCTM[]		= "AT+SQNCTM=";
static const uint8_t REQ_ATSQNCTM[]		= "AT+SQNCTM?\r";
static const uint8_t urcATSQNCTM[]			= "\r\n+SQNCTM:";

/* Force the modem on band 20 */
#ifdef USE_TRUPHONE
static const uint8_t REQ_ATBAND[] 		= "AT!=\"uci:set sqnmm_operator.truphone.bands 20\"\r";
#else
static const uint8_t REQ_ATBAND[]       = "AT!=\"uci:set sqnmm_operator.verizon.bands 20\"\r";
#endif /* USE_TRUPHONE */

/************************************************/
/*               AWS MQTT Commands              */
/************************************************/
static const uint8_t CMD_ATSQNSNVW[]="AT+SQNSNVW=";
static const uint8_t REQ_ATSQNSNVWCERT[]="AT+SQNSNVW=?\r";

static const uint8_t CMD_ATSQNSNVR[]="AT+SQNSNVR=";
static const uint8_t urcSQNSNVRCERT[]="\r\n+SQNSNVR:";

static const uint8_t CMD_ATSQNSMQTTCLIENTCFG[]="AT+SQNSMQTTCLIENTCFG=";
static const uint8_t REQ_ATSQNSMQTTCLIENTCFG[]="AT+SQNSMQTTCLIENTCFG=?\r";
static const uint8_t urcSQNSMQTTCLIENTCFG[]="\r\n+SQNSMQTTCLIENTCFG:";

static const uint8_t CMD_ATSQNSMQTTCLIENTCONNECT[]="AT+SQNSMQTTCLIENTCONNECT=";
static const uint8_t REQ_ATSQNSMQTTCLIENTCONNECT[]="AT+SQNSMQTTCLIENTCONNECT=?\r";
static const uint8_t urcSQNSMQTTCLIENTCONNECT[]="\r\n+SQNSMQTTCLIENTONCONNECT:";

/* This command used to disconnect from a broker.*/
static const uint8_t CMD_ATSQNSMQTTCLIENTDISCONNECT[]="AT+SQNSMQTTCLIENTDISCONNECT=";
static const uint8_t urcSQNSMQTTCLIENTDISCONNECT[]="\r\n+SQNSMQTTCLIENTDISCONNECT:";


/* This command is used to publish a payload into a topic on a broker host.*/
static const uint8_t CMD_ATSQNSMQTTCLIENTPUBLISH[]="AT+SQNSMQTTCLIENTPUBLISH=";
static const uint8_t urcSQNSMQTTCLIENTPUBLISH[]="\r\n+SQNSMQTTCLIENTPUBLISH:";


/* This command used to receive a message by id*/
static const uint8_t CMD_ATSQNSMQTTCLIENTRCVMESSAGE[]	= "AT+SQNSMQTTCLIENTRCVMESSAGE=";
static const uint8_t urcSQNSMQTTCLIENTRCVMESSAGE[]		= "\r\n+SQNSMQTTCLIENTRCVMESSAGE:";


/************************************************/
/*             Debug Console 		            */
/************************************************/
//static const uint8_t CMD_ATBIND[]	= "AT+BIND=\"console\",\"UART2\"\r";
static const uint8_t CMD_ATBIND[]	= "AT+BIND=";
static const uint8_t REQ_ATBIND[]	= "AT+BIND\r";
static const uint8_t urc_ATBIND[]	= "+BIND: ";

/************************************************/
/*             Network Status		            */
/************************************************/
static const uint8_t REQ_ATFSM[] 		= "AT!=\"fsm\"\r";
static const uint8_t CMD_ATCOPS01[] 	= "AT+COPS=0,1\r";
static const uint8_t CMD_ATCOPS[]		= "AT+COPS=";
static const uint8_t REQ_ATCOPS[]		= "AT+COPS?\r";
static const uint8_t urcATCOPS[]		= "\r\n+COPS: ";


#endif /* ATCMD_H_ */
