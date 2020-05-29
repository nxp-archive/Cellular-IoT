/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef GM01Q_TOOLS_H_
#define GM01Q_TOOLS_H_

#include "fsl_usart.h"
#include "fsl_usart_freertos.h"
#include "fsl_usart_dma.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define stringparm_maxsize 10
#define AT_BUFFER_SIZE (0x400U)//(0x10U)
#define str_certificate	((uint8_t *)"certificate")
#define str_privatekey	((uint8_t *)"privatekey")

typedef enum {
GM01Q_PREPARE_CMD_CLOSE  = 0,
GM01Q_PREPARE_CMD_CONTINUE= 1
} GM01Q_PREPARE_CMD;

typedef enum {
	SQNS_T3412_UNIT_10MIN			= 0x00,	/*	000x xxxx	*/
	SQNS_T3412_UNIT_1H				= 0x20,	/*	001x xxxx		*/
	SQNS_T3412_UNIT_10H				= 0x40,	/*	010x xxxx		*/
	SQNS_T3412_UNIT_2SEC			= 0x60,	/*	011x xxxx		*/
	SQNS_T3412_UNIT_30SEC			= 0x80,	/*	100x xxxx		*/
	SQNS_T3412_UNIT_1MIN			= 0xA0,	/*	101x xxxx		*/
	SQNS_T3412_UNIT_TIMERDISABLED	= 0XE0	/*	111x xxxx		*/
} SQNS_T3412EXTUNIT;

typedef enum {
	SQNS_T3324_UNIT_2SEC			= 0x00,	/*	000x xxxx		*/
	SQNS_T3324_UNIT_1MIN			= 0x20,	/*	001x xxxx		*/
	SQNS_T3324_UNIT_6MIN			= 0x40,	/*	020x xxxx		*/
	SQNS_T3324_UNIT_TIMERDISABLED	= 0XE0	/*	111x xxxx		*/
} SQNS_T3324UNIT;

typedef enum {
	SQNS_COPS_MODE_AUTOMATIC		= 0,
	SQNS_COPS_MODE_MANUAL			= 1,
	SQNS_COPS_MODE_DEREGISTER		= 2,
	SQNS_COPS_MODE_SETONLYFORMAT	= 3,
	SQNS_COPS_MODE_MANUAL_AUTOMATIC	= 4
} SQNS_COPS_MODE;

typedef enum {
	SQNS_COPS_FORMAT_LONG			= 0,	/*	long format alphanumeric	*/
	SQNS_COPS_FORMAT_SHORT			= 1,	/*	short format alphanumeric	*/
	SQNS_COPS_FORMAT_NUMERIC		= 2		/*	numeric						*/
} SQNS_COPS_FORMAT;

/*******************************************************************************
 * External Resources
 ******************************************************************************/
//extern usart_rtos_handle_t g_uartRtosHandle;
//extern usart_handle_t g_uartHandle;
extern usart_transfer_t sendXfer;
extern usart_transfer_t receiveXfer;
extern uint8_t g_rxBuffer[AT_BUFFER_SIZE];
extern uint8_t g_txBuffer[AT_BUFFER_SIZE];
extern uint8_t g_rxRingBuffer[AT_BUFFER_SIZE];
extern uint8_t Requested_PeriodicTAU[10];
extern uint8_t Requested_ActiveTime[10];
extern volatile bool  gm01q_EchoModeActivated;

/*******************************************************************************
 * API
 ******************************************************************************/
void USART_UserCallback(USART_Type *base, usart_dma_handle_t *handle, status_t status, void *userData);
void DMA_Callback(dma_handle_t *handle, void *param, bool transferDone, uint32_t tcds);
void Timer_CallbackHandler( uint32_t flags );
bool gm01q_tools_ATRespCheck(uint8_t * response, const uint8_t * expected);
bool gm01q_tools_ATFirstByteRespCheck(uint8_t * response, const uint8_t * expected, uint8_t FirstRespByte);
uint8_t gm01q_tools_NetworkRegistrationUnsolicitedCheck(const uint8_t * NetworkRegNotif);

/*********************************************************************************
* Configurations Functions
*********************************************************************************/
void gmO1q_atcmd_usart2_cfg(void);

/*********************************************************************************
* Hardware Reset Functions
*********************************************************************************/
void gmO1q_atcmd_HWReset(void);

/*********************************************************************************
* Sleep Mode Tools Functions
*********************************************************************************/
void gmO1q_atcmd_RTS_FlowControl_cfg(void);
void gmO1q_atcmd_RTS_GPIO_cfg(void);
void gm01q_tools_RTS_GPIO_Deasserted(void);
void gm01q_tools_RTS_GPIO_Asserted(void);


/*********************************************************************************
* Sleep Mode Functions
*********************************************************************************/
void gmO1q_atcmd_SleepMode(void);
void gmO1q_atcmd_WakeUp(void);
void gm01q_tools_T3412ExtendedValueSet(SQNS_T3412EXTUNIT unit, uint8_t timervalue);
void gm01q_tools_T3324ValueSet(SQNS_T3324UNIT unit, uint8_t timervalue);

/*********************************************************************************
*  Functions
*********************************************************************************/
bool gm01q_tools_AttachNetwork(void);


/*********************************************************************************
* Data Frame Functions
*********************************************************************************/
void gm01q_tools_SendATCmd(const uint8_t * buff, const size_t buff_size);
void gm01q_tools_CatchATResponse(size_t ATCmdSent);
void gm01q_tools_WaitATPrompt(size_t ATCmdSent);
void gm01q_tools_CatchATFSMResp(void);

void gm01q_tools_CatchResponse(void);
void gm01q_tools_WaitPrompt(void);


/*********************************************************************************
* Echo Data Frame Functions
*********************************************************************************/
bool gm01q_tools_EchoCatchCmd(size_t CmdSize);
bool gm01q_tools_EchoCatchCertificate(size_t CertificateSize);



void gm01q_tools_Sleep(unsigned int ms);
void gm01q_tools_CleanBuff(uint8_t* buff, size_t buff_size);
size_t gm01q_tools_mySizeoff(const uint8_t * buff);
void gm01q_tools_Transformbin(uint8_t * binbuffer, uint8_t valuetotransform);

void gm01q_tools_ParamStringPrepare(const uint8_t * string, uint8_t * stringprepared);
void gm01q_tools_StartPrepareCommand(const uint8_t * cmd);
void gm01q_tools_AddNumParam(size_t numparameter, GM01Q_PREPARE_CMD isnewparam);
void gm01q_tools_AddStringParam(uint8_t * parameter, GM01Q_PREPARE_CMD isnewparam);
void gm01q_tools_SeparateParam(void);
void gm01q_tools_CloseCommand(void);

#endif /* GM01Q_TOOLS_H_ */
