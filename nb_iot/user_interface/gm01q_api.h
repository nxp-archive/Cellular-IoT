/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef GM01Q_API_H_
#define GM01Q_API_H_

#include "atcmd.h"
#include "gm01q_tools.h"
#include "gsm/gsm_typedefs.h"
#include "gsm/gsm_private.h"

typedef struct ST_RXDATAPENDING_TAG
{
	uint8_t waitingForData;
	uint32_t BytesPending;
	uint32_t connid;
	char RxBuffer[1800];
	char * ptr_start;
	char * ptr_end;
}st_RXData;

extern st_RXData sRXData[GSM_CFG_MAX_CONNS][11];
extern uint8_t ongoingRxNtf;
extern int8_t rcv_ring;
extern uint8_t rxCircularBufferPos;
extern uint8_t prev_rxCircularBufferPos;
extern uint8_t pendingRead;

int32_t SOCKETS_SetCfgExt(void);
int32_t SOCKETS_SetCfg(void);
int32_t SOCKETS_SetSockSecurity(void);
int32_t SOCKETS_SetTLSSecurityCfg(void);

bool gm01q_api_ActivateATCmd(void);
bool gm01q_api_ActivateEchoMode(void);

bool gm01q_api_SetNetworkRegistrationUnsolicited(SQNS_CREGINTEGER creginteger);
uint8_t gm01q_api_ConfigureTheFullFunctionalityLevel(SQNS_CFUNCODE functionalitycode, bool WaitNetworkRegistrationUnsolicited);
bool gm01q_api_RequestUSIMState(void);


bool gm01q_api_PowerSavingModeSettingWrite(SQNS_PSM_STATUS mode,SQNS_T3412EXTUNIT T3412extunit, uint8_t T3412exttimervalue, SQNS_T3324UNIT T3324unit, uint8_t T3324timervalue);
bool gm01q_api_PowerSavingModeSettingRead(void);

bool gm01q_api_UARTInterfacePowerSavingConfigurationWrite(SQNS_PSM_MODE powersavingmode, uint16_t timeout);
bool gm01q_api_UARTInterfacePowerSavingConfigurationRead(void);
void gm01q_api_PowerSavingModeUnitaryTest(uint16_t Duration);

bool gm01q_api_ResetModem(void);

bool gm01q_api_ConformanceTestMode(void);

/**********************************************************************************:
 *    Non-Volatile Memory
 **********************************************************************************/
gsmr_t gm01q_api_WriteCertKeyInNVM(const char * certkey, SQNS_MQTT_CERTORKEY type, uint8_t index, size_t certkeysize, const gsm_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
bool gm01q_api_ReadCertKeyInNVM(SQNS_MQTT_CERTORKEY type, uint8_t index);
bool gm01q_api_DeleteCertKeyInNVM(SQNS_MQTT_CERTORKEY type, uint8_t index);

gsmr_t gm01q_api_getHostIP(const char * hostName, const gsm_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
gsmr_t gm01q_api_socketDial(uint8_t connId, uint8_t txProt, uint16_t rHostPort, const char* ip, uint8_t closureType, uint8_t lPort, uint8_t connMode, uint8_t acceptAnyRemote, const gsm_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
gsmr_t gm01q_api_socketSend( uint8_t connId, const unsigned char * pTX , uint32_t sTx );
uint32_t gm01q_api_socketRecv( uint8_t connId );
uint32_t gm01q_api_socketReadData( uint8_t connId, unsigned char * pRX , uint16_t rcvlen );
void gm01q_api_socketStoreRXData_Pending_Info( uint32_t connId, uint32_t BP );
uint32_t gm01q_api_socketUpdateRXData_Pending_Info( uint32_t connId, uint32_t BP );												   
gsmr_t gm01q_api_setSocketSecurity(uint8_t spId, uint8_t connId, uint8_t enable, const gsm_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
gsmr_t gm01q_api_setTLSSecurityProfileCfg(	uint8_t spId,
									uint8_t version,
									const char * cipherSpecs,
									uint8_t certValidLevel,
									uint8_t caCertificateID,
									uint8_t clientCertificateID,
									uint8_t clientPrivateKeyID,
									const char * psk,
									const gsm_api_cmd_evt_fn evt_fn,
									void* const evt_arg,
									const uint32_t blocking);
gsmr_t gm01q_api_setSocketCfgExt(uint8_t connId, uint8_t srMode, uint8_t recvDataMode, uint8_t keepalive, uint8_t listenAutoRsp, uint8_t sendDataMode, const gsm_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
gsmr_t gm01q_api_setSocketCfg(uint8_t connId, uint8_t cid, uint16_t pktsize, uint16_t maxto, uint32_t connto , uint32_t txTo , const uint32_t blocking);
gsmr_t gm01q_api_socketClose( uint32_t connId);
gsmr_t gm01q_api_setLogInModule(void);
gsmr_t gm01q_api_readConfTestMode(void);

/********************************************************************************/
/*            MQTT 																*/
/********************************************************************************/
bool gm01q_api_MQTTInitiateClientConfigurationSet(uint8_t index, const uint8_t * IOTThingName);
bool gm01q_api_MQTTInitiateConnectionBroker(uint8_t index, const uint8_t * BrokerEndPoint, size_t BrokerPort);
bool gm01q_api_MQTTInitiateClientConfigurationGet(void);

bool gm01q_api_PLMNSelectionFormatSet(SQNS_COPS_MODE copsmode, SQNS_COPS_FORMAT copsformat);
bool gm01q_api_PLMNSelectionGet(void);
void gm01q_api_DebugPrintNetworkStatus(void);

bool gm01q_api_DebugWriteUARTDebugConsoleID(uint8_t * type, uint8_t * uart);
bool gm01q_api_DebugReadUARTDebugConsoleID(void);


#endif /* GM01Q_API_H_ */
