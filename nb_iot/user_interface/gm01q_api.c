/*
 * gm01q_api.c
 *
 *  Created on: 19 juin 2019
 *      Author: nxf41880
 */


#include "gm01q_tools.h"
#include "gm01q_api.h"
#include "atcmd.h"
#include "aws_clientcredential.h"

#ifdef DEBUG_NAMING
#include "fsl_debug_console.h"
#endif

#define PREPARED_BUFF_SIZE 32

volatile bool  gm01q_EchoModeActivated = false;

st_RXData sRXData[GSM_CFG_MAX_CONNS][11] __attribute__ ((section(".data.$SRAMX_16"))) = {
											{
												{	.waitingForData = 0,
													.BytesPending = 0,
													.connid = 0,
													.RxBuffer = {0},
													.ptr_start = sRXData[0][0].RxBuffer ,
													.ptr_end = sRXData[0][0].RxBuffer
												},
												{	.waitingForData = 0,
													.BytesPending = 0,
													.connid = 0,
													.RxBuffer = {0},
													.ptr_start = sRXData[0][1].RxBuffer ,
													.ptr_end = sRXData[0][1].RxBuffer
												},
												{	.waitingForData = 0,
													.BytesPending = 0,
													.connid = 0,
													.RxBuffer = {0},
													.ptr_start = sRXData[0][2].RxBuffer ,
													.ptr_end = sRXData[0][2].RxBuffer
												},
												{	.waitingForData = 0,
													.BytesPending = 0,
													.connid = 0,
													.RxBuffer = {0},
													.ptr_start = sRXData[0][3].RxBuffer ,
													.ptr_end = sRXData[0][3].RxBuffer
												},
												{	.waitingForData = 0,
													.BytesPending = 0,
													.connid = 0,
													.RxBuffer = {0},
													.ptr_start = sRXData[0][4].RxBuffer ,
													.ptr_end = sRXData[0][4].RxBuffer
												},
												{	.waitingForData = 0,
													.BytesPending = 0,
													.connid = 0,
													.RxBuffer = {0},
													.ptr_start = sRXData[0][5].RxBuffer ,
													.ptr_end = sRXData[0][5].RxBuffer
												},
												{	.waitingForData = 0,
													.BytesPending = 0,
													.connid = 0,
													.RxBuffer = {0},
													.ptr_start = sRXData[0][6].RxBuffer ,
													.ptr_end = sRXData[0][6].RxBuffer
												},
												{	.waitingForData = 0,
													.BytesPending = 0,
													.connid = 0,
													.RxBuffer = {0},
													.ptr_start = sRXData[0][7].RxBuffer ,
													.ptr_end = sRXData[0][7].RxBuffer
												},
												{	.waitingForData = 0,
													.BytesPending = 0,
													.connid = 0,
													.RxBuffer = {0},
													.ptr_start = sRXData[0][8].RxBuffer ,
													.ptr_end = sRXData[0][8].RxBuffer
												},
												{	.waitingForData = 0,
													.BytesPending = 0,
													.connid = 0,
													.RxBuffer = {0},
													.ptr_start = sRXData[0][9].RxBuffer ,
													.ptr_end = sRXData[0][9].RxBuffer
												},
												{	.waitingForData = 0,
													.BytesPending = 0,
													.connid = 0,
													.RxBuffer = {0},
													.ptr_start = sRXData[0][10].RxBuffer ,
													.ptr_end = sRXData[0][10].RxBuffer
												},
//												{	.waitingForData = 0,
//													.BytesPending = 0,
//													.connid = 0,
//													.RxBuffer = {0},
//													.ptr_start = sRXData[0][11].RxBuffer ,
//													.ptr_end = sRXData[0][11].RxBuffer
//												},
//												{	.waitingForData = 0,
//													.BytesPending = 0,
//													.connid = 0,
//													.RxBuffer = {0},
//													.ptr_start = sRXData[0][12].RxBuffer ,
//													.ptr_end = sRXData[0][12].RxBuffer
//												},
//												{	.waitingForData = 0,
//													.BytesPending = 0,
//													.connid = 0,
//													.RxBuffer = {0},
//													.ptr_start = sRXData[0][13].RxBuffer ,
//													.ptr_end = sRXData[0][13].RxBuffer
//												},
//												{	.waitingForData = 0,
//													.BytesPending = 0,
//													.connid = 0,
//													.RxBuffer = {0},
//													.ptr_start = sRXData[0][14].RxBuffer ,
//													.ptr_end = sRXData[0][14].RxBuffer
//												},
//												{	.waitingForData = 0,
//													.BytesPending = 0,
//													.connid = 0,
//													.RxBuffer = {0},
//													.ptr_start = sRXData[0][15].RxBuffer ,
//													.ptr_end = sRXData[0][15].RxBuffer
//												},
//												{	.waitingForData = 0,
//													.BytesPending = 0,
//													.connid = 0,
//													.RxBuffer = {0},
//													.ptr_start = sRXData[0][16].RxBuffer ,
//													.ptr_end = sRXData[0][16].RxBuffer
//												},
//												{	.waitingForData = 0,
//													.BytesPending = 0,
//													.connid = 0,
//													.RxBuffer = {0},
//													.ptr_start = sRXData[0][17].RxBuffer ,
//													.ptr_end = sRXData[0][17].RxBuffer
//												},
//												{	.waitingForData = 0,
//													.BytesPending = 0,
//													.connid = 0,
//													.RxBuffer = {0},
//													.ptr_start = sRXData[0][18].RxBuffer ,
//													.ptr_end = sRXData[0][18].RxBuffer
//												},
//												{	.waitingForData = 0,
//													.BytesPending = 0,
//													.connid = 0,
//													.RxBuffer = {0},
//													.ptr_start = sRXData[0][19].RxBuffer ,
//													.ptr_end = sRXData[0][19].RxBuffer
//												}

											}
};
uint8_t ongoingRxNtf = 0;
int8_t rcv_ring = 0;
uint8_t rxCircularBufferPos = 0;
uint8_t prev_rxCircularBufferPos = 0;
uint8_t pendingRead = 0;

///********************************************************************************
//  *
//  * @fn bool gm01q_api_ActivateATCmd
//  *
//  */
// /**
//  *
//  *
//  * @brief The function verify connection between LPC55 and GM01Q
//  *
//  * @return bool
//  *
//  * @note AT command : AT  <br>
//  * description : DAEC: Display Avbl Endpts and Cltrs  <br>
//  *
//  *
// ********************************************************************************/
//bool gm01q_api_ActivateATCmd(void)
//{
//	bool modemresponseok = false;
//
//#ifdef DEBUG_NAMING
//	gm01q_tools_Sleep(DEBUG_Naming_Sleep);
//	PRINTF("\r\n\r\nVerify connection between LPC55 and GM01Q \r\n");
//	PRINTF("================================================= \r\n");
//#endif
//
//    gm01q_tools_SendATCmd(CMD_AT, sizeof(CMD_AT) - 1);
//
//    /* Echo mode not activated then check response directly*/
//    gm01q_tools_CatchResponse();
//    modemresponseok = gm01q_tools_ATRespCheck(g_rxBuffer, rxOK);
//
//    return modemresponseok;
//}
//
///**********************************************************************************:
// *   The function enables Echo mode in the modem
// **********************************************************************************/
//bool gm01q_api_ActivateEchoMode(void)
//{
//	bool modemresponseok = false;
//
//    gm01q_tools_SendATCmd(CMD_ATE, sizeof(CMD_ATE) - 1);
//
//    gm01q_tools_CatchATResponse(sizeof(CMD_ATE)-1);
//    modemresponseok = gm01q_tools_ATRespCheck(g_rxBuffer, rxOK);
//
//    if (modemresponseok)
//    {
//    	gm01q_EchoModeActivated = true;
//    }
//
//    return modemresponseok;
//}
//
//
///**********************************************************************************:
// *   The function controls the presentation of an unsolicited result code +CREG: X
// **********************************************************************************/
//bool gm01q_api_SetNetworkRegistrationUnsolicited(SQNS_CREGINTEGER creginteger)
//{
//	bool modemresponseok = false;
//
//	gm01q_tools_StartPrepareCommand(CMD_ATCREG);
//	gm01q_tools_AddNumParam(creginteger,GM01Q_PREPARE_CMD_CLOSE);
//
//#ifdef DEBUG_NAMING
//	gm01q_tools_Sleep(DEBUG_Naming_Sleep);
//	PRINTF("\r\n\r\nEnable Network Registration Notification\r\n");
//	PRINTF("=============================================== \r\n");
//#endif
//
//	gm01q_tools_SendATCmd(g_txBuffer, sizeof(g_txBuffer) - 1);
//    gm01q_tools_CatchATResponse(sizeof(g_txBuffer)-1);
//    modemresponseok = gm01q_tools_ATRespCheck(g_rxBuffer, rxOK);
//
//    return modemresponseok;
//}
//
//
///**********************************************************************************:
// *   The function selects the level of functionality in the MT
// **********************************************************************************/
//uint8_t gm01q_api_ConfigureTheFullFunctionalityLevel(SQNS_CFUNCODE functionalitycode, bool WaitNetworkRegistrationUnsolicited)
//{
//	bool modemattached = false;
//	uint8_t networkregnotif = 0xFF;
//
//	gm01q_tools_StartPrepareCommand(CMD_ATCFUN);
//	gm01q_tools_AddNumParam(functionalitycode,GM01Q_PREPARE_CMD_CLOSE);
//
//#ifdef DEBUG_NAMING
//	gm01q_tools_Sleep(DEBUG_Naming_Sleep);
//	PRINTF("\r\n\r\ngm01q_api_ConfigureTheFullFunctionalityLevel\r\n");
//	PRINTF("==================================================== \r\n");
//#endif
//
//	gm01q_tools_SendATCmd(g_txBuffer, sizeof(g_txBuffer) - 1);
//    gm01q_tools_CatchATResponse(sizeof(g_txBuffer)-1);
//    gm01q_tools_ATRespCheck(g_rxBuffer, rxOK);
//
//	if (WaitNetworkRegistrationUnsolicited)
//	{
//#ifdef DEBUG_NAMING
//	PRINTF("\r\nWait Network Registration Notification\r\n");
//#endif
//		while (!modemattached)
//		{
//			gm01q_tools_CatchResponse();
//			/* Wait for the Modem to be accroched */
//#if 1
//			modemattached = gm01q_tools_ATRespCheck(g_rxBuffer, rxCREG5);
//
//			if (modemattached)
//			{
//#ifdef DEBUG_NAMING
//				PRINTF("\r\nnetworkregnotif=5\r\n");
//#endif
//				networkregnotif=5;
//			}
//			/*else
//			{
//				gm01q_api_DebugPrintNetworkStatus();
//			}*/
//#else		/* Wait for the Modem to send any response */
//			modemattached = gm01q_tools_ATFirstByteRespCheck(g_rxBuffer, rxCREG, sizeof(rxCREG)-1);
//			if (modemattached)
//			{
//				networkregnotif = gm01q_tools_NetworkRegistrationUnsolicitedCheck(g_rxBuffer);
//			}
//#endif
//		}
//	}
//
//	return networkregnotif;
//}
//
//
///**********************************************************************************:
// *   The function provides USIM Status
// **********************************************************************************/
//bool gm01q_api_RequestUSIMState(void)
//{
//	bool modemresponseok = false;
//
//#ifdef DEBUG_NAMING
//	gm01q_tools_Sleep(DEBUG_Naming_Sleep);
//	PRINTF("\r\n\r\nRequest USIM state\r\n");
//	PRINTF("========================== \r\n");
//#endif
//
//    gm01q_tools_SendATCmd(REQ_ATCPIN, sizeof(REQ_ATCPIN) - 1);
//
//    gm01q_tools_CatchATResponse(sizeof(REQ_ATCPIN)-1);
//    modemresponseok = gm01q_tools_ATRespCheck(g_rxBuffer, urcATCPIN);
//
//    gm01q_tools_CatchResponse();
//    modemresponseok = gm01q_tools_ATRespCheck(g_rxBuffer, rxOK);
//
//#ifdef DEBUG_NAMING
//	gm01q_tools_Sleep(DEBUG_Naming_Sleep);
//#endif
//    return modemresponseok;
//}
//
//
///********************************************************************************************:
// *   The function controls the setting of the UEs power saving mode (PSM) parameters.
// *   "AT+CPSMS=1,,,\"10000110\",\"00000000\"\r"
// ********************************************************************************************/
//bool gm01q_api_PowerSavingModeSettingWrite(SQNS_PSM_STATUS mode,SQNS_T3412EXTUNIT T3412extunit, uint8_t T3412exttimervalue, SQNS_T3324UNIT T3324unit, uint8_t T3324timervalue)
//{
//	bool modemresponseok = false;
//
//	/*Set Requested_PeriodicTAU */
//    gm01q_tools_T3412ExtendedValueSet(T3412extunit, T3412exttimervalue);
//
//    /* Set Requested_ActiveTime */
//    gm01q_tools_T3324ValueSet(T3324unit, T3324timervalue);
//
//	gm01q_tools_StartPrepareCommand(CMD_ATCPSMS);
//	gm01q_tools_AddNumParam(mode,GM01Q_PREPARE_CMD_CONTINUE);
//
//	/* optional parameter not used */
//	gm01q_tools_SeparateParam();
//
//	/* optional parameter not used */
//	gm01q_tools_SeparateParam();
//
//	gm01q_tools_AddStringParam(Requested_PeriodicTAU,GM01Q_PREPARE_CMD_CONTINUE);
//	gm01q_tools_AddStringParam(Requested_ActiveTime,GM01Q_PREPARE_CMD_CLOSE);
//
//#ifdef DEBUG_NAMING
//	gm01q_tools_Sleep(DEBUG_Naming_Sleep);
//	PRINTF("\r\n\r\nWrite the setting of the UEs power saving mode\r\n");
//	PRINTF("===================================================== \r\n");
//#endif
//
//	gm01q_tools_SendATCmd(g_txBuffer, sizeof(g_txBuffer) - 1);
//    gm01q_tools_CatchATResponse(sizeof(g_txBuffer)-1);
//    modemresponseok = gm01q_tools_ATRespCheck(g_rxBuffer, rxOK);
//
//    return modemresponseok;
//}
//
//
//bool gm01q_api_PowerSavingModeSettingRead(void)
//{
//	bool modemresponseok = false;
//
//#ifdef DEBUG_NAMING
//	gm01q_tools_Sleep(DEBUG_Naming_Sleep);
//	PRINTF("\r\n\r\nRead the setting of the UEs power saving mode\r\n");
//	PRINTF("===================================================== \r\n");
//#endif
//
//	gm01q_tools_SendATCmd(REQ_ATCPSMS, sizeof(REQ_ATCPSMS) - 1);
//    gm01q_tools_CatchATResponse(sizeof(REQ_ATCPSMS)-1);
//
//    /* Read the Power Saving Mode setting */
//    modemresponseok = gm01q_tools_ATFirstByteRespCheck(g_rxBuffer, urcCPSMS, sizeof(urcCPSMS)-1);
//
//    /* Wait the acknowledgment of the response */
//    gm01q_tools_CatchResponse();
//    modemresponseok = gm01q_tools_ATRespCheck(g_rxBuffer, rxOK);
//
//    return modemresponseok;
//}
//
///**********************************************************************************:
// *   Write command sets the UARTs power saving configuration
// *   When no activity on UART, CTS line will be set to OFF state (driven high level)
// *   <timeout> milliseconds (100ms to 10s, default 5s) after the last sent
// *   character, then module will go to sleep mode as soon as DTE set RTS line
// *   to OFF state (driver high level).
// **********************************************************************************/
//bool gm01q_api_UARTInterfacePowerSavingConfigurationWrite(SQNS_PSM_MODE powersavingmode, uint16_t timeout)
//{
//	bool modemresponseok = false;
//
//	/* If timeout smaller than 100ms */
//	if (timeout < 100)
//	{
//		/* timeout = 100ms */
//		timeout = 100;
//	}
//
//	/* If timeout bigger than 10s */
//	if (timeout > 10000)
//	{
//		/* timeout = 10s */
//		timeout = 10000;
//	}
//
//	gm01q_tools_StartPrepareCommand(CMD_ATSQNIPSCFG);
//	gm01q_tools_AddNumParam(powersavingmode,GM01Q_PREPARE_CMD_CONTINUE);
//	gm01q_tools_AddNumParam(timeout,GM01Q_PREPARE_CMD_CLOSE);
//
//#ifdef DEBUG_NAMING
//	gm01q_tools_Sleep(DEBUG_Naming_Sleep);
//	PRINTF("\r\n\r\nSet the UART Interface Power Saving Configuration\r\n");
//	PRINTF("========================================================= \r\n");
//#endif
//
//	gm01q_tools_SendATCmd(g_txBuffer, sizeof(g_txBuffer) - 1);
//
//	gm01q_tools_CatchATResponse(sizeof(g_txBuffer)-1);
//    modemresponseok = gm01q_tools_ATRespCheck(g_rxBuffer, rxOK);
//
//    return modemresponseok;
//}
//
//
///**********************************************************************************:
// *   Read the UARTs power saving configuration
// **********************************************************************************/
//bool gm01q_api_UARTInterfacePowerSavingConfigurationRead(void)
//{
//	bool modemresponseok = false;
//
//#ifdef DEBUG_NAMING
//	gm01q_tools_Sleep(DEBUG_Naming_Sleep);
//	PRINTF("\r\nGet the UART Interface Power Saving Configuration\r\n");
//	PRINTF("==================================================== \r\n");
//#endif
//
//    gm01q_tools_SendATCmd(REQ_ATSQNIPSCFG, sizeof(REQ_ATSQNIPSCFG) - 1);
//
//    gm01q_tools_CatchATResponse(sizeof(REQ_ATSQNIPSCFG)-1);
//    modemresponseok = gm01q_tools_ATFirstByteRespCheck(g_rxBuffer, urcSQNIPSCFG, sizeof(urcSQNIPSCFG)-1);
//
//    gm01q_tools_CatchResponse();
//    modemresponseok = gm01q_tools_ATRespCheck(g_rxBuffer, rxOK);
//
//    return modemresponseok;
//}
//
//void gm01q_api_PSMUnitaryTest(uint16_t Duration)
//{
//	gmO1q_atcmd_SleepMode();
//	gm01q_tools_Sleep(Duration);
//	gmO1q_atcmd_WakeUp();
//}

int32_t SOCKETS_SetCfgExt(void)
{
	return gm01q_api_setSocketCfgExt(gsm.m.conn_val_id + 1, 1/*2*/, 1, 0, 1, 1, NULL, NULL, 1);
}

int32_t SOCKETS_SetCfg(void)
{
	return gm01q_api_setSocketCfg(gsm.m.conn_val_id + 1, 1, 1450, 0, 600, 50, 1);
}

int32_t SOCKETS_SetSockSecurity(void)
{
    return gm01q_api_setSocketSecurity(gsm.m.conn_val_id + 1, gsm.m.conn_val_id + 1, 0, NULL, NULL, 1); // taylan
}

int32_t SOCKETS_SetTLSSecurityCfg(void)
{
	return gm01q_api_setTLSSecurityProfileCfg(gsm.m.conn_val_id + 1, 2, "0x3C", 0, 0, 0, 0, NULL, NULL, NULL, 1);
}


/**
 * \brief           Write a Certificate or a private Key in Non-Volatile Memory
 * \param[in]       certkey: Access to the certificate or private key
 * \param[in]       type: Specify if certificate or private key will be used
 * \param[in]       index: Memory slot
 * \param[in]       certkeysize: Size of the certificate or private key
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref gsmOK on success, member of \ref gsmr_t enumeration otherwise
 */
gsmr_t
gm01q_api_WriteCertKeyInNVM(const char * certkey, SQNS_MQTT_CERTORKEY type, uint8_t index, size_t certkeysize, const gsm_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking)
{
    GSM_MSG_VAR_DEFINE(msg);

    //GSM_ASSERT("certkey != NULL", certkey != NULL);

    GSM_MSG_VAR_ALLOC(msg, blocking);
    GSM_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    GSM_MSG_VAR_REF(msg).cmd_def = GSM_CMD_SQNSNVW_W;
    GSM_MSG_VAR_REF(msg).msg.modem_memory.cert_key_ptr = certkey;
    GSM_MSG_VAR_REF(msg).msg.modem_memory.data_type = type;
    GSM_MSG_VAR_REF(msg).msg.modem_memory.index = index;
    GSM_MSG_VAR_REF(msg).msg.modem_memory.certkeysize = certkeysize;

    return gsmi_send_msg_to_producer_mbox(&GSM_MSG_VAR_REF(msg), gsmi_initiate_cmd, 30000);
}

/**
 * \brief           Query to DNS server to resolve the host name into an IP address
 * \param[in]       hostName: URL Host name
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref gsmOK on success, member of \ref gsmr_t enumeration otherwise
 */
gsmr_t
gm01q_api_getHostIP(const char * hostName, const gsm_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking)
{
    GSM_MSG_VAR_DEFINE(msg);

    GSM_ASSERT("hostName != NULL", hostName != NULL);

    GSM_MSG_VAR_ALLOC(msg, blocking);
    GSM_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    GSM_MSG_VAR_REF(msg).cmd_def = GSM_CMD_SQNDNSLKUP;
    GSM_MSG_VAR_REF(msg).msg.host_ip_config.hostName = hostName;

    return gsmi_send_msg_to_producer_mbox(&GSM_MSG_VAR_REF(msg), gsmi_initiate_cmd, 30000);
}

/**
 * \brief           Opens a remote connection via socket
 * \param[in]       connId: Connection ID, must be between 1 and GSM_CFG_MAX_CONNS
 * \param[in]       txProt: Specify if certificate or private key will be used
 * \param[in]       rHostPort: Memory slot
 * \param[in]       ip: Host IP Address
 * \param[in]       closureType: Socket closure behaviour for TCP, has no effect for UDP connections
 * 						0: Local host closes immediately when remote host has closed (default)
 * 						255: Local host closes after an escape sequence (+++)
 * \param[in]       lPort: UDP connection local port, has no effect for TCP connections.
 * \param[in]       connMode: Connection mode
 * 						0: Online mode connection (default)
 * 						1: Command mode connection
 * \param[in]       acceptAnyRemote: Size of the certificate or private key
 * 						0: Disabled (default)
 * 						1: Enabled
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref gsmOK on success, member of \ref gsmr_t enumeration otherwise
 */
gsmr_t
gm01q_api_socketDial(	uint8_t connId,
						uint8_t txProt,
						uint16_t rHostPort,
						const char* ip,
						uint8_t closureType,
						uint8_t lPort,
						uint8_t connMode,
						uint8_t acceptAnyRemote,
						const gsm_api_cmd_evt_fn evt_fn,
						void* const evt_arg,
						const uint32_t blocking)
{
    GSM_MSG_VAR_DEFINE(msg);

    GSM_ASSERT("ip != NULL", ip != NULL);

    GSM_MSG_VAR_ALLOC(msg, blocking);
    GSM_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    GSM_MSG_VAR_REF(msg).cmd_def = GSM_CMD_SQNSD;
    GSM_MSG_VAR_REF(msg).msg.socket_dial.connId = connId;
    GSM_MSG_VAR_REF(msg).msg.socket_dial.txProt = txProt;
    GSM_MSG_VAR_REF(msg).msg.socket_dial.rHostPort = rHostPort;
    GSM_MSG_VAR_REF(msg).msg.socket_dial.ip = ip;
    GSM_MSG_VAR_REF(msg).msg.socket_dial.closureType = closureType;
    GSM_MSG_VAR_REF(msg).msg.socket_dial.lPort = lPort;
    GSM_MSG_VAR_REF(msg).msg.socket_dial.connMode = connMode;
    GSM_MSG_VAR_REF(msg).msg.socket_dial.acceptAnyRemote = acceptAnyRemote;

    return gsmi_send_msg_to_producer_mbox(&GSM_MSG_VAR_REF(msg), gsmi_initiate_cmd, 60000);
}

/**
 * \brief           Send data over a socket which was established before
 * \param[in]       connId: Connection ID, must be between 1 and GSM_CFG_MAX_CONNS
 * \param[in]       pointer to the data to be sent
 * \param[in]       number of bytes to be sent
 * \return          \ref gsmOK on success, member of \ref gsmr_t enumeration otherwise
 */
gsmr_t
gm01q_api_socketSend( uint8_t connId, const unsigned char * pTX , uint32_t sTx )
{
    GSM_MSG_VAR_DEFINE(msg);

    GSM_MSG_VAR_ALLOC(msg, 1);// blocking
    GSM_MSG_VAR_REF(msg).cmd_def = GSM_CMD_SQNSSENDEXT;
    GSM_MSG_VAR_REF(msg).msg.tx_data.connId = connId;
    GSM_MSG_VAR_REF(msg).msg.tx_data.ptrTx = pTX;
    GSM_MSG_VAR_REF(msg).msg.tx_data.Txsize = sTx;

    return gsmi_send_msg_to_producer_mbox(&GSM_MSG_VAR_REF(msg), gsmi_initiate_cmd, 60000);
}

/**
 * \brief           Receive data
 * \param[in]       connId: Connection ID, must be between 1 and GSM_CFG_MAX_CONNS
 * \return          \ref gsmOK on success, member of \ref gsmr_t enumeration otherwise
 */
uint32_t
gm01q_api_socketRecv( uint8_t connId )
{
	uint8_t ret =  gsmERR;

	/* Check if data is waiting to be read because +SQNSRING has been received */
	if(sRXData[connId - 1][prev_rxCircularBufferPos].waitingForData)
	{

		GSM_MSG_VAR_DEFINE(msg);

		GSM_MSG_VAR_ALLOC(msg, 1);// blocking
		GSM_MSG_VAR_REF(msg).cmd_def = GSM_CMD_SQNS_RECV;
		GSM_MSG_VAR_REF(msg).msg.rx_data.connId = connId;
		GSM_MSG_VAR_REF(msg).msg.rx_data.Rxsize = sRXData[connId - 1][prev_rxCircularBufferPos].BytesPending;

		ret = gsmi_send_msg_to_producer_mbox(&GSM_MSG_VAR_REF(msg), gsmi_initiate_cmd, 60000);
	}
	else
	{
		ret = gsmERR;		/* No data pending */
	}


    return ret;
}


/**
 * \brief           Receive data
 * \param[in]       connId: Connection ID, must be between 1 and GSM_CFG_MAX_CONNS
 * \param[in]       pointer to the data to be read
 * \param[in]       number of bytes to be read
 * \return          \ref gsmOK on success, member of \ref gsmr_t enumeration otherwise
 */
uint32_t
gm01q_api_socketReadData( uint8_t connId, unsigned char * pRX , uint16_t rcvlen )
{
	uint32_t ret = 0;
	/* Check if data needs to be read because +SQNSRING occurred */
	if( rcv_ring > 0)
	{
		if( gm01q_api_socketRecv(1) == gsmOK )
		{
			rcv_ring--;
		}
	}
	//uint16_t datareq = 0;

//	for( int i = 0 ; i < 200 ;  i++ )
//	{
//		if( sRXData[connId - 1].BytesPending == 0 )
//		{
//			vTaskDelay(pdMS_TO_TICKS(10) );
//		}
//		else
//		{
//			break;
//		}
//	}

	if( sRXData[connId - 1][prev_rxCircularBufferPos].BytesPending == 0 )
	{
		return ret;   // simulate time out
	}

	if( rcvlen >= sRXData[connId - 1][prev_rxCircularBufferPos].BytesPending ) // taylan
	{
		memcpy(pRX,sRXData[connId - 1][prev_rxCircularBufferPos].ptr_start,sRXData[connId - 1][prev_rxCircularBufferPos].BytesPending);
		ret = sRXData[connId - 1][prev_rxCircularBufferPos].BytesPending;
		sRXData[connId - 1][prev_rxCircularBufferPos].BytesPending = 0;
		sRXData[connId - 1][prev_rxCircularBufferPos].ptr_start += ret;
		pendingRead--;
		prev_rxCircularBufferPos == 10 ? prev_rxCircularBufferPos = 0 : prev_rxCircularBufferPos++;
	}
	else
	{
		memcpy(pRX,sRXData[connId - 1][prev_rxCircularBufferPos].ptr_start,rcvlen);
		sRXData[connId - 1][prev_rxCircularBufferPos].BytesPending -= rcvlen;
		ret = rcvlen;
		sRXData[connId - 1][prev_rxCircularBufferPos].ptr_start += ret;
	}

    return ret;
}


/**
 * \brief           Initialise the RX structure for data to be received
 * \param[in]       connId: Connection ID, must be between 1 and GSM_CFG_MAX_CONNS
 * \param[in]		BP: Number of Bytes Pending to be read
 * \return          \ref gsmOK on success, member of \ref gsmr_t enumeration otherwise
 */
#include "task.h"
void
gm01q_api_socketStoreRXData_Pending_Info( uint32_t connId, uint32_t BP )
{
	taskENTER_CRITICAL();
	sRXData[connId - 1][rxCircularBufferPos].waitingForData = 1;
	sRXData[connId - 1][rxCircularBufferPos].connid = connId;
	sRXData[connId - 1][rxCircularBufferPos].BytesPending = BP;
	rxCircularBufferPos == 10 ? rxCircularBufferPos = 0 : rxCircularBufferPos++;

	taskEXIT_CRITICAL();
//	ongoingRxNtf==GSM_CFG_MAX_CONNS? ongoingRxNtf=0 : ongoingRxNtf++;
}


/**
 * \brief           Update the RX structure for data to be received
 * \param[in]       connId: Connection ID, must be between 1 and GSM_CFG_MAX_CONNS
 * \param[in]		BP: Number of Bytes Pending to be read
 * \return          \ref gsmOK on success, member of \ref gsmr_t enumeration otherwise
 */
uint32_t
gm01q_api_socketUpdateRXData_Pending_Info( uint32_t connId, uint32_t BP )
{
	uint32_t ret = gsmERR;

	if(sRXData[connId - 1][prev_rxCircularBufferPos].connid == connId /*&& sRXData[connId - 1].BytesPending == BP*/ )
	{
		sRXData[connId - 1][prev_rxCircularBufferPos].waitingForData = 0;
		ret = gsmOK;
	}

	return ret;
}

/**
 * \brief           Enables or disables the use of SSL/TLS connection on a TCP or UDP socket
 * \param[in]       spId: Security Profile ID (must be defined between 1 and 6)
 * \param[in]       connId: Connection ID, must be between 1 and GSM_CFG_MAX_CONNS
 * \param[in]       enable: Enable/Disable security on socket
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref gsmOK on success, member of \ref gsmr_t enumeration otherwise
 */
gsmr_t
gm01q_api_setSocketSecurity(uint8_t spId, uint8_t connId, uint8_t enable, const gsm_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking)
{
    GSM_MSG_VAR_DEFINE(msg);

    GSM_MSG_VAR_ALLOC(msg, blocking);
    GSM_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    GSM_MSG_VAR_REF(msg).cmd_def = GSM_CMD_SQNSSCFG;
    GSM_MSG_VAR_REF(msg).msg.secure_socket_cfg.connId = connId;
    GSM_MSG_VAR_REF(msg).msg.secure_socket_cfg.enable = enable;
    GSM_MSG_VAR_REF(msg).msg.secure_socket_cfg.spId = spId;

    return gsmi_send_msg_to_producer_mbox(&GSM_MSG_VAR_REF(msg), gsmi_initiate_cmd, 30000);
}

/**
 * \brief           Sets the security profile parameters required to configure the following SSL/TLS connections properties
 * \param[in]       spId: Security Profile ID (must be defined between 1 and 6)
 * \param[in]       version: TLS Profile version
 * \param[in]       cipherSpecs: Exact list of cipher suite to be used, 8-bit hexadecimal "0x" prefixed IANA numbers, semicolon delimited
 * \param[in]       certValidLevel: Server certificate validation 8-bit field
 * \param[in]       caCertificateID: Trusted Certificate Authority certificate ID, integer in range [0-19]
 * \param[in]       clientCertificateID: Client certificate ID, integer in range [0-19]
 * \param[in]       clientPrivateKeyID: Client private key ID, integer in range [0-19]
 * \param[in]       psk: Pre-shared key used for connection (when a TLS_PSK_* cipher suite is used)
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref gsmOK on success, member of \ref gsmr_t enumeration otherwise
 */
gsmr_t
gm01q_api_setTLSSecurityProfileCfg(	uint8_t spId,
									uint8_t version,
									const char * cipherSpecs,
									uint8_t certValidLevel,
									uint8_t caCertificateID,
									uint8_t clientCertificateID,
									uint8_t clientPrivateKeyID,
									const char * psk,
									const gsm_api_cmd_evt_fn evt_fn,
									void* const evt_arg,
									const uint32_t blocking)
{
	GSM_MSG_VAR_DEFINE(msg);

	GSM_MSG_VAR_ALLOC(msg, blocking);
	GSM_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
	GSM_MSG_VAR_REF(msg).cmd_def = GSM_CMD_SQNSPCFG;
	GSM_MSG_VAR_REF(msg).msg.tls_security_profile_cfg.spId = spId;
	GSM_MSG_VAR_REF(msg).msg.tls_security_profile_cfg.version = version;
	GSM_MSG_VAR_REF(msg).msg.tls_security_profile_cfg.cipherSpecs = cipherSpecs;
	GSM_MSG_VAR_REF(msg).msg.tls_security_profile_cfg.certValidLevel = certValidLevel;
	GSM_MSG_VAR_REF(msg).msg.tls_security_profile_cfg.caCertificateID = caCertificateID;
	GSM_MSG_VAR_REF(msg).msg.tls_security_profile_cfg.clientCertificateID = clientCertificateID;
	GSM_MSG_VAR_REF(msg).msg.tls_security_profile_cfg.clientPrivateKeyID = clientPrivateKeyID;
	GSM_MSG_VAR_REF(msg).msg.tls_security_profile_cfg.psk = psk;

	return gsmi_send_msg_to_producer_mbox(&GSM_MSG_VAR_REF(msg), gsmi_initiate_cmd, 30000);
}

/**
 * \brief           Sets the socket configuration extended parameters
 * \param[in]       connId: Connection ID, must be between 1 and GSM_CFG_MAX_CONNS
 * \param[in]       srMode: SQNSRING URC mode
 * \param[in]       recvDataMode: Received data view mode presentation format [0:text format OR 1:hexadecimal format]
 * \param[in]       keepalive: Currently unused
 * \param[in]       listenAutoRsp: Listen auto-response mode�, that affects AT+SQNSL command
 * \param[in]       sendDataMode: Sent data view mode presentation format
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref gsmOK on success, member of \ref gsmr_t enumeration otherwise
 */
gsmr_t
gm01q_api_setSocketCfgExt(uint8_t connId, uint8_t srMode, uint8_t recvDataMode, uint8_t keepalive, uint8_t listenAutoRsp, uint8_t sendDataMode, const gsm_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking)
{
    GSM_MSG_VAR_DEFINE(msg);

    GSM_MSG_VAR_ALLOC(msg, blocking);
    GSM_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    GSM_MSG_VAR_REF(msg).cmd_def = GSM_CMD_SQNSCFGEXT;
    GSM_MSG_VAR_REF(msg).msg.socket_cfg_ext.connId = connId;
    GSM_MSG_VAR_REF(msg).msg.socket_cfg_ext.srMode = srMode;
    GSM_MSG_VAR_REF(msg).msg.socket_cfg_ext.recvDataMode = recvDataMode;
    GSM_MSG_VAR_REF(msg).msg.socket_cfg_ext.keepalive = keepalive;
    GSM_MSG_VAR_REF(msg).msg.socket_cfg_ext.listenAutoRsp = listenAutoRsp;
    GSM_MSG_VAR_REF(msg).msg.socket_cfg_ext.sendDataMode = sendDataMode;

    return gsmi_send_msg_to_producer_mbox(&GSM_MSG_VAR_REF(msg), gsmi_initiate_cmd, 30000);
}


/**
 * \brief           Sets the socket configuration extended parameters
 * \param[in]       connId: Connection ID, must be between 1 and GSM_CFG_MAX_CONNS
 * \param[in]       cid
 * \param[in]       pktsz
 * \param[in]       maxto
 * \param[in]       connto
 * \param[in]       txto
 * \return          \ref gsmOK on success, member of \ref gsmr_t enumeration otherwise
 */
gsmr_t
gm01q_api_setSocketCfg(uint8_t connId, uint8_t cid, uint16_t pktsize, uint16_t maxto, uint32_t connto , uint32_t txTo , const uint32_t blocking)
{
    GSM_MSG_VAR_DEFINE(msg);

    GSM_MSG_VAR_ALLOC(msg, blocking);
    GSM_MSG_VAR_REF(msg).cmd_def = GSM_CMD_SQNSCFG;
    GSM_MSG_VAR_REF(msg).msg.socket_cfg.connId = connId;
    GSM_MSG_VAR_REF(msg).msg.socket_cfg.cid = cid;
    GSM_MSG_VAR_REF(msg).msg.socket_cfg.pktSz = pktsize;
    GSM_MSG_VAR_REF(msg).msg.socket_cfg.maxTo = maxto;
    GSM_MSG_VAR_REF(msg).msg.socket_cfg.connTo = connto;
    GSM_MSG_VAR_REF(msg).msg.socket_cfg.txTo = txTo;

    return gsmi_send_msg_to_producer_mbox(&GSM_MSG_VAR_REF(msg), gsmi_initiate_cmd, 30000);
}

/**
 * \brief           Close a socket connection
 * \param[in]       connId: Connection ID, must be between 1 and GSM_CFG_MAX_CONNS
 * \return          \ref gsmOK on success, member of \ref gsmr_t enumeration otherwise
 */
gsmr_t
gm01q_api_socketClose(uint32_t connId)
{
    GSM_MSG_VAR_DEFINE(msg);

    GSM_MSG_VAR_ALLOC(msg, 1);// blocking
    GSM_MSG_VAR_REF(msg).cmd_def = GSM_CMD_SQNSH;
    GSM_MSG_VAR_REF(msg).msg.socket_dial.connId = connId;

    return gsmi_send_msg_to_producer_mbox(&GSM_MSG_VAR_REF(msg), gsmi_initiate_cmd, 60000);
}

gsmr_t
gm01q_api_setLogInModule(void)
{
//	GSM_MSG_VAR_DEFINE(msg);
//
//	GSM_MSG_VAR_ALLOC(msg, 1);
//	GSM_MSG_VAR_REF(msg).cmd_def = GSM_CMD_SQNSLG;
//
//	gsmi_send_msg_to_producer_mbox(&GSM_MSG_VAR_REF(msg), gsmi_initiate_cmd, 60000);
//
//	GSM_MSG_VAR_DEFINE(msg_2);
//
//	GSM_MSG_VAR_ALLOC(msg_2, 1);
//	GSM_MSG_VAR_REF(msg_2).cmd_def = GSM_CMD_SQNPLG;
//	return gsmi_send_msg_to_producer_mbox(&GSM_MSG_VAR_REF(msg_2), gsmi_initiate_cmd, 60000);

	GSM_MSG_VAR_DEFINE(msg);

	GSM_MSG_VAR_ALLOC(msg, 1);
	GSM_MSG_VAR_REF(msg).cmd_def = GSM_CMD_SQNDISHPPLMN;

	return gsmi_send_msg_to_producer_mbox(&GSM_MSG_VAR_REF(msg), gsmi_initiate_cmd, 60000);
}

/**
 * \brief           Read the current conformance test mode
 * \return          \ref gsmOK on success, member of \ref gsmr_t enumeration otherwise
 */
gsmr_t
gm01q_api_readConfTestMode(void)
{
    GSM_MSG_VAR_DEFINE(msg);

    GSM_MSG_VAR_ALLOC(msg, 1);// blocking
    GSM_MSG_VAR_REF(msg).cmd_def = GSM_CMD_SQNCTM_INFO;

    return gsmi_send_msg_to_producer_mbox(&GSM_MSG_VAR_REF(msg), gsmi_initiate_cmd, 60000);
}
//
///**********************************************************************************:
// *   Read a Certificate or a private Key in Non-Volatile Memory
// **********************************************************************************/
//bool  gm01q_api_ReadCertKeyInNVM(SQNS_MQTT_CERTORKEY type, uint8_t index)
//{
//	bool modemresponseok = false;
//	uint8_t certorkey[PREPARED_BUFF_SIZE] = {0};
//
//	gm01q_tools_StartPrepareCommand(CMD_ATSQNSNVR);
//
//	switch (type)
//	{
//		case SQNS_MQTT_PRIVATEKEY:
//			/*Copy in  g_txBuffer the template AT command to write a certificate in GM01Q */
//			gm01q_tools_ParamStringPrepare(str_privatekey, certorkey);
//			gm01q_tools_AddStringParam(certorkey,GM01Q_PREPARE_CMD_CONTINUE);
//			break;
//
//		case SQNS_MQTT_CERTIFICATE:
//		default:
//			/*Copy in  g_txBuffer the template AT command to write a certificate in GM01Q */
//			gm01q_tools_ParamStringPrepare(str_certificate, certorkey);
//			gm01q_tools_AddStringParam(certorkey,GM01Q_PREPARE_CMD_CONTINUE);
//			break;
//	}
//
//	gm01q_tools_AddNumParam(index,GM01Q_PREPARE_CMD_CLOSE);
//
//#ifdef DEBUG_NAMING
//	gm01q_tools_Sleep(DEBUG_Naming_Sleep);
//	if(type == SQNS_MQTT_PRIVATEKEY)
//	{
//		PRINTF("\r\n\r\nRead Private Key in the non-volatile memory\r\n");
//		PRINTF("===================================================\r\n");
//	}
//	else
//	{
//		PRINTF("\r\n\r\nRead Certificate in the non-volatile memory\r\n");
//		PRINTF("===================================================\r\n");
//	}
//#endif
//
//    gm01q_tools_SendATCmd(g_txBuffer, sizeof(g_txBuffer) - 1);
//
//	gm01q_tools_CatchATResponse(sizeof(g_txBuffer)-1);
//	modemresponseok = gm01q_tools_ATFirstByteRespCheck(g_rxBuffer, urcSQNSNVRCERT, sizeof(urcSQNSNVRCERT)-1);
//
//	gm01q_tools_CatchResponse();
//	modemresponseok = gm01q_tools_ATRespCheck(g_rxBuffer, rxOK);
//
//    return modemresponseok;
//}
//
///**********************************************************************************:
// *   Delete a Certificate or a private Key in Non-Volatile Memory
// **********************************************************************************/
//bool gm01q_api_DeleteCertKeyInNVM(SQNS_MQTT_CERTORKEY type, uint8_t index)
//{
//	bool modemresponseok = false;
//	uint8_t certorkey[PREPARED_BUFF_SIZE] = {0};
//
//	gm01q_tools_StartPrepareCommand(CMD_ATSQNSNVW);
//
//	switch (type)
//	{
//		case SQNS_MQTT_PRIVATEKEY:
//			/*Copy in  g_txBuffer the template AT command to write a certificate in GM01Q */
//			gm01q_tools_ParamStringPrepare(str_privatekey, certorkey);
//			gm01q_tools_AddStringParam(certorkey,GM01Q_PREPARE_CMD_CONTINUE);
//			break;
//
//		case SQNS_MQTT_CERTIFICATE:
//		default:
//			/*Copy in  g_txBuffer the template AT command to write a certificate in GM01Q */
//			gm01q_tools_ParamStringPrepare(str_certificate, certorkey);
//			gm01q_tools_AddStringParam(certorkey,GM01Q_PREPARE_CMD_CONTINUE);
//			break;
//	}
//
//	gm01q_tools_AddNumParam(index,GM01Q_PREPARE_CMD_CONTINUE);
//
//	/* Write 0 to delete */
//	gm01q_tools_AddNumParam(0,GM01Q_PREPARE_CMD_CLOSE);
//
//#ifdef DEBUG_NAMING
//	gm01q_tools_Sleep(DEBUG_Naming_Sleep);
//	if(type == SQNS_MQTT_PRIVATEKEY)
//	{
//		PRINTF("\r\n\r\nDelete Private Key in the non-volatile memory\r\n");
//		PRINTF("=====================================================\r\n");
//	}
//	else
//	{
//		PRINTF("\r\n\r\nDelete Certificate in the non-volatile memory\r\n");
//		PRINTF("=====================================================\r\n");
//	}
//#endif
//
//    gm01q_tools_SendATCmd(g_txBuffer, sizeof(g_txBuffer) - 1);
//	gm01q_tools_CatchATResponse(sizeof(g_txBuffer)-1);
//	modemresponseok = gm01q_tools_ATRespCheck(g_rxBuffer, rxOK);
//
//    return modemresponseok;
//}
//
///**********************************************************************************:
// *   Configure the MQTT broker Connection
// **********************************************************************************/
//bool gm01q_api_MQTTInitiateClientConfigurationSet(uint8_t index, const uint8_t * IOTThingName)
//{
//	bool modemresponseok = false;
//	uint8_t iotthingnameprepared[PREPARED_BUFF_SIZE] = {0};
//
//	gm01q_tools_ParamStringPrepare(IOTThingName, iotthingnameprepared);
//
//	gm01q_tools_StartPrepareCommand(CMD_ATSQNSMQTTCLIENTCFG);
//	gm01q_tools_AddNumParam(index,GM01Q_PREPARE_CMD_CONTINUE);
//	gm01q_tools_AddStringParam(iotthingnameprepared,GM01Q_PREPARE_CMD_CLOSE);
//#if 0
//	gm01q_tools_AddStringParam(iotthingnameprepared,GM01Q_PREPARE_CMD_CONTINUE);
//	/* parameter optional not used */
//	gm01q_tools_SeparateParam();
//	/* parameter optional not used */
//	gm01q_tools_SeparateParam();
//
//	/* Add Root Certificate Index = 0 */
//	//gm01q_tools_AddNumParam(0,GM01Q_PREPARE_CMD_CONTINUE);
//	gm01q_tools_AddNumParam(0,GM01Q_PREPARE_CMD_CLOSE);
//	/* Add Client Certificate Index = 1 */
//	//gm01q_tools_AddNumParam(1,GM01Q_PREPARE_CMD_CONTINUE);
//	/* Add Private key Index = 2 */
//	//gm01q_tools_AddNumParam(2,GM01Q_PREPARE_CMD_CLOSE);
//#endif
//
//#ifdef DEBUG_NAMING
//	gm01q_tools_Sleep(DEBUG_Naming_Sleep);
//		PRINTF("\r\n\r\ngm01q_api_MQTTInitiateClientConfigurationSet\r\n");
//		PRINTF("===================================================\r\n");
//#endif
//
//	gm01q_tools_SendATCmd(g_txBuffer, sizeof(g_txBuffer) - 1);
//
//	gm01q_tools_CatchATResponse(sizeof(g_txBuffer)-1);
//	modemresponseok = gm01q_tools_ATRespCheck(g_rxBuffer, rxOK);
//
//    return modemresponseok;
//}
//
//
//bool gm01q_api_MQTTInitiateClientConfigurationGet(void)
//{
//	bool modemresponseok = false;
//
//#ifdef DEBUG_NAMING
//	gm01q_tools_Sleep(DEBUG_Naming_Sleep);
//	PRINTF("\r\n\r\ngm01q_api_MQTTInitiateClientConfigurationGet\r\n");
//	PRINTF("====================================================\r\n");
//#endif
//
//	gm01q_tools_SendATCmd(REQ_ATSQNSMQTTCLIENTCFG, sizeof(REQ_ATSQNSMQTTCLIENTCFG) - 1);
//
//	gm01q_tools_CatchATResponse(sizeof(REQ_ATSQNSMQTTCLIENTCFG)-1);
//	modemresponseok = gm01q_tools_ATFirstByteRespCheck(g_rxBuffer, urcSQNSMQTTCLIENTCFG, sizeof(urcSQNSMQTTCLIENTCFG)-1);
//
//	gm01q_tools_CatchResponse();
//	modemresponseok = gm01q_tools_ATRespCheck(g_rxBuffer, rxOK);
//
//	return modemresponseok;
//}
//
///**********************************************************************************:
// *   Create a MQTT broker Connection
// **********************************************************************************/
//bool gm01q_api_MQTTInitiateConnectionBroker(uint8_t index, const uint8_t * BrokerEndPoint, size_t BrokerPort)
//{
//		bool modemresponseok = false;
//		uint8_t brokerendpointprepared[PREPARED_BUFF_SIZE] = {0};
//
//		gm01q_tools_ParamStringPrepare(BrokerEndPoint, brokerendpointprepared);
//
//		gm01q_tools_StartPrepareCommand(CMD_ATSQNSMQTTCLIENTCONNECT);
//		gm01q_tools_AddNumParam(index,GM01Q_PREPARE_CMD_CONTINUE);
//		gm01q_tools_AddStringParam(brokerendpointprepared,GM01Q_PREPARE_CMD_CONTINUE);
//		gm01q_tools_AddNumParam(BrokerPort,GM01Q_PREPARE_CMD_CLOSE);
//
//
//	#ifdef DEBUG_NAMING
//		gm01q_tools_Sleep(DEBUG_Naming_Sleep);
//		PRINTF("\r\n\r\ngm01q_api_MQTTInitiateConnectionBroker\r\n");
//		PRINTF("==============================================\r\n");
//
//	#endif
//
//		gm01q_tools_SendATCmd(g_txBuffer, sizeof(g_txBuffer) - 1);
//
//		gm01q_tools_CatchATResponse(sizeof(g_txBuffer)-1);
//		modemresponseok = gm01q_tools_ATRespCheck(g_rxBuffer, rxOK);
//
//	    return modemresponseok;
//}
//
///********************************************************************************
//  *
//  * @fn bool gm01q_api_ResetModem
//  *
//  */
// /**
//  *
//  *
//  * @brief Reset the modem
//  *
//  * @return bool
//  *
//  * @note AT command : AT^RESET  <br>
//  * description :
//  *
//  *
// ********************************************************************************/
//bool gm01q_api_ResetModem(void)
//{
//	bool reset = false;
//
//    gm01q_tools_SendATCmd(CMD_ATRESET, sizeof(CMD_ATRESET) - 1);
//
//    return reset;
//
//    gm01q_tools_CatchATResponse(sizeof(CMD_ATRESET)-1);
//    reset = gm01q_tools_ATRespCheck(g_rxBuffer, rxOK);
//
//    if (reset)
//    {
//        gm01q_tools_CatchResponse();
//        reset = gm01q_tools_ATRespCheck(g_rxBuffer, urcSHUTDOWN);
//
//        if (reset)
//        {
//            gm01q_tools_CatchResponse();
//            reset = gm01q_tools_ATRespCheck(g_rxBuffer, urcSYSTART);
//        }
//    }
//    return reset;
//}
//
///**********************************************************************************:
// *   Check the Conformance Test Mode
// **********************************************************************************/
//bool gm01q_api_ConformanceTestMode(void)
//{
//	bool modemresponseok = false;
//
//#ifdef DEBUG_NAMING
//	gm01q_tools_Sleep(DEBUG_Naming_Sleep);
//	PRINTF("\r\n\r\nCheck the Conformance Test Mode\r\n");
//	PRINTF("========================================== \r\n");
//#endif
//
//    gm01q_tools_SendATCmd(REQ_ATSQNCTM, sizeof(REQ_ATSQNCTM) - 1);
//
//    gm01q_tools_CatchATResponse(sizeof(REQ_ATSQNCTM)-1);
//    modemresponseok = gm01q_tools_ATRespCheck(g_rxBuffer, urcATSQNCTM);
//
//    gm01q_tools_CatchResponse();
//    modemresponseok = gm01q_tools_ATRespCheck(g_rxBuffer, rxOK);
//
//#ifdef DEBUG_NAMING
//	gm01q_tools_Sleep(DEBUG_Naming_Sleep);
//#endif
//    return modemresponseok;
//}
//
//
//
///* Set the configuration to display network name */
//bool gm01q_api_PLMNSelectionFormatSet(SQNS_COPS_MODE copsmode, SQNS_COPS_FORMAT copsformat)
//{
//	bool modemresponseok = false;
//
//	gm01q_tools_StartPrepareCommand(CMD_ATCOPS);
//	gm01q_tools_AddNumParam(copsmode,GM01Q_PREPARE_CMD_CONTINUE);
//	gm01q_tools_AddNumParam(copsformat,GM01Q_PREPARE_CMD_CLOSE);
//
//#ifdef DEBUG_NAMING
//	gm01q_tools_Sleep(DEBUG_Naming_Sleep);
//	PRINTF("\r\n\r\nSet the PLMN Selection Format\r\n");
//	PRINTF("========================================\r\n");
//#endif
//
//    gm01q_tools_SendATCmd(g_txBuffer, sizeof(g_txBuffer) - 1);
//
//    gm01q_tools_CatchATResponse(sizeof(CMD_ATCOPS01)-1);
//    modemresponseok = gm01q_tools_ATRespCheck(g_rxBuffer, rxOK);
//
//    return modemresponseok;
//}
//
///* Display the network name */
//bool gm01q_api_PLMNSelectionGet(void)
//{
//	bool modemresponseok = false;
//
//#ifdef DEBUG_NAMING
//	gm01q_tools_Sleep(DEBUG_Naming_Sleep);
//	PRINTF("\r\n\r\nGet the PLMN Selected \r\n");
//	PRINTF("==============================\r\n");
//#endif
//
//    gm01q_tools_SendATCmd(REQ_ATCOPS, sizeof(REQ_ATCOPS) - 1);
//
//	gm01q_tools_CatchATResponse(sizeof(REQ_ATCOPS)-1);
//	modemresponseok = gm01q_tools_ATFirstByteRespCheck(g_rxBuffer, urcATCOPS, sizeof(urcATCOPS)-1);
//
//	gm01q_tools_CatchResponse();
//	modemresponseok = gm01q_tools_ATRespCheck(g_rxBuffer, rxOK);
//
//    return modemresponseok;
//}
//
///* For Debug usage ONLY- display an array with network debug values in debug console (TeraTerm) */
//void gm01q_api_DebugPrintNetworkStatus(void)
//{
//
//#ifdef DEBUG_NAMING
//	gm01q_tools_Sleep(DEBUG_Naming_Sleep);
//	PRINTF("\r\n\r\nPrint Network Status\r\n");
//	PRINTF("============================\r\n");
//#endif
//
//	gm01q_tools_SendATCmd(REQ_ATFSM, sizeof(REQ_ATFSM) - 1);
//	gm01q_tools_CatchATFSMResp();
//	gm01q_tools_CleanBuff(g_rxBuffer, (size_t)AT_BUFFER_SIZE);
//}
//
///* Change the debug console output */
//bool gm01q_api_DebugWriteUARTDebugConsoleID(uint8_t * type, uint8_t * uart)
//{
//	bool modemresponseok = false;
//	uint8_t stringrpepared[PREPARED_BUFF_SIZE] = {0};
//
//	gm01q_tools_StartPrepareCommand(CMD_ATBIND);
//	gm01q_tools_ParamStringPrepare(stringrpepared, type);
//	gm01q_tools_AddStringParam(stringrpepared,GM01Q_PREPARE_CMD_CONTINUE);
//	gm01q_tools_ParamStringPrepare(stringrpepared, uart);
//	gm01q_tools_AddStringParam(stringrpepared,GM01Q_PREPARE_CMD_CLOSE);
//
//#ifdef DEBUG_NAMING
//	gm01q_tools_Sleep(DEBUG_Naming_Sleep);
//	PRINTF("\r\n\r\nSet the UART console selected \r\n");
//	PRINTF("==============================\r\n");
//#endif
//
//    gm01q_tools_SendATCmd(g_txBuffer, sizeof(g_txBuffer) - 1);
//
//	gm01q_tools_CatchResponse();
//	modemresponseok = gm01q_tools_ATRespCheck(g_rxBuffer, rxOK);
//
//    return modemresponseok;
//}
//
///* Check the debug console output */
//bool gm01q_api_DebugReadUARTDebugConsoleID(void)
//{
//	bool modemresponseok = false;
//
//#ifdef DEBUG_NAMING
//	gm01q_tools_Sleep(DEBUG_Naming_Sleep);
//	PRINTF("\r\n\r\nGet the UART console selected \r\n");
//	PRINTF("==============================\r\n");
//#endif
//
//    gm01q_tools_SendATCmd(REQ_ATBIND, sizeof(REQ_ATBIND) - 1);
//
//	gm01q_tools_CatchATResponse(sizeof(REQ_ATBIND)-1);
//	modemresponseok = gm01q_tools_ATFirstByteRespCheck(g_rxBuffer, urc_ATBIND, sizeof(urc_ATBIND)-1);
//
//	gm01q_tools_CatchResponse();
//	modemresponseok = gm01q_tools_ATRespCheck(g_rxBuffer, rxOK);
//
//    return modemresponseok;
//}
