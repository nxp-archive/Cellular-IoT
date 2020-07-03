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


#define BUFFER_POLLS_NB	11U
/* Max possible amount of data coming from the module is 1500 bytes
 * but since a byte is sent via 2 ASCII characters, 1500 bytes
 * will represented by 3000 ASCII characters
 */
//#define CELLULAR_BUFFER_SIZE 1500U * 2U
#define CELLULAR_BUFFER_SIZE 1800U

typedef struct ST_RXDATAPENDING_TAG
{
	uint32_t BytesPending;					/*!< Number of Bytes pending to be read by the Application */
	uint32_t connid;						/*!< Connection ID of the received message */
	char RxBuffer[CELLULAR_BUFFER_SIZE];	/*!< RX Buffer poll */
	char * ptr_start;						/*!< Pointer of the next character to read */
	char * ptr_end;							/*!< Pointer of the last character to read */
} st_RXData;

typedef struct
{
	uint32_t BytesPending;					/*!< Number of Bytes pending to be read by the Application */
	uint32_t connid;						/*!< Connection ID of the received message */
} st_RingList;

extern st_RXData sRXData[GSM_CFG_MAX_CONNS][BUFFER_POLLS_NB];
extern st_RingList RingList[BUFFER_POLLS_NB*2];
extern uint8_t u8_nextFreeBufferPool;
extern uint8_t u8_nextBufferPoolForData;

int32_t SOCKETS_SetCfgExt(void);
int32_t SOCKETS_SetCfg(void);
int32_t SOCKETS_SetSockSecurity(void);
int32_t SOCKETS_SetTLSSecurityCfg(void);

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
gsmr_t gm01q_api_setConfTestMode(const char * ctm, const gsm_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);

#endif /* GM01Q_API_H_ */
