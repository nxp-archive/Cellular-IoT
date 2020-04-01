/*
 * gm01q_tools.c
 *
 *  Created on: 27 juin 2019
 *      Author: nxf41880
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <event_groups.h>
#include "gm01q_tools.h"
#include "atcmd.h"
#include "fsl_usart.h"
#include "fsl_usart_freertos.h"
#include "nbiot_common.h"


#if DEBUG
#include "fsl_debug_console.h"
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/
//usart_rtos_handle_t g_uartRtosHandle;
//usart_handle_t g_uartHandle;
usart_transfer_t sendXfer;
usart_transfer_t receiveXfer;
uint8_t g_rxBuffer[AT_BUFFER_SIZE] __attribute__ ((section(".data.$SRAM4"))) = {0};
uint8_t g_txBuffer[AT_BUFFER_SIZE] __attribute__ ((section(".data.$SRAM4"))) = {0} ;
uint8_t g_rxRingBuffer[AT_BUFFER_SIZE] __attribute__ ((section(".data.$SRAM4"))) = {0}; /* RX ring buffer. */
uint8_t Requested_PeriodicTAU[10];
uint8_t Requested_ActiveTime[10];

volatile bool rxBufferEmpty            = true;
volatile bool txBufferFull             = false;
volatile bool txOnGoing                = false;
volatile bool rxOnGoing                = false;


/*********************************************************************************
* Tools Functions
*********************************************************************************/
/* USART user callback */
//void USART_UserCallback(USART_Type *base, usart_dma_handle_t *handle, status_t status, void *userData)
//{
//	userData = userData;
//
//	if (kStatus_USART_TxIdle == status)
//	{
//		txBufferFull = false;
//		txOnGoing    = false;
//	}
//
//	if (kStatus_USART_RxIdle == status)
//	{
//		rxBufferEmpty = false;
//		rxOnGoing     = false;
//	}
//}

/*!
 * brief Receives a buffer of data using an interrupt method - modified version of
 * USART_TransferReceiveNonBlocking API from fsl_usart.c file
 *
 * This function receives data using an interrupt method. This is a non-blocking function, which
 *  returns without waiting for all data to be received.
 * If the RX ring buffer is used and not empty, the data in the ring buffer is copied and
 * the parameter p receivedBytes shows how many bytes are copied from the ring buffer.
 * After copying, if the data in the ring buffer is not enough to read, the receive
 * request is saved by the USART driver. When the new data arrives, the receive request
 * is serviced first. When all data is received, the USART driver notifies the upper layer
 * through a callback function and passes the status parameter ref kStatus_USART_RxIdle.
 * For example, the upper layer needs 10 bytes but there are only 5 bytes in the ring buffer.
 * The 5 bytes are copied to the xfer->data and this function returns with the
 * parameter p receivedBytes set to 5. For the left 5 bytes, newly arrived data is
 * saved from the xfer->data[5]. When 5 bytes are received, the USART driver notifies the upper layer.
 * If the RX ring buffer is not enabled, this function enables the RX and RX interrupt
 * to receive data to the xfer->data. When all data is received, the upper layer is notified.
 *
 * param base USART peripheral base address.
 * param handle USART handle pointer.
 * param xfer USART transfer structure, see #usart_transfer_t.
 * param receivedBytes Bytes received from the ring buffer directly.
 * retval kStatus_Success Successfully queue the transfer into transmit queue.
 * retval kStatus_USART_RxBusy Previous receive request is not finished.
 * retval kStatus_InvalidArgument Invalid argument.
 */
//status_t USART_TransferReceiveNonBlocking(USART_Type *base,
//                                          usart_handle_t *handle,
//                                          usart_transfer_t *xfer,
//                                          size_t *receivedBytes)
//{
//    uint32_t i;
//    /* How many bytes to copy from ring buffer to user memory. */
//    size_t bytesToCopy = 0U;
//    /* How many bytes to receive. */
//    size_t bytesToReceive;
//    /* How many bytes currently have received. */
//    size_t bytesCurrentReceived;
//    uint32_t regPrimask = 0U;
//    /* Stop character of a GM01Q frame */
//    const uint8_t stopPatern = '\n';
//
//
//    /* Check arguments */
//    assert(!((NULL == base) || (NULL == handle) || (NULL == xfer)));
//    if ((NULL == base) || (NULL == handle) || (NULL == xfer))
//    {
//        return kStatus_InvalidArgument;
//    }
//    /* Check xfer members */
//    assert(!((0 == xfer->dataSize) || (NULL == xfer->data)));
//    if ((0 == xfer->dataSize) || (NULL == xfer->data))
//    {
//        return kStatus_InvalidArgument;
//    }
//
//    /* How to get data:
//       1. If RX ring buffer is not enabled, then save xfer->data and xfer->dataSize
//          to uart handle, enable interrupt to store received data to xfer->data. When
//          all data received, trigger callback.
//       2. If RX ring buffer is enabled and not empty, get data from ring buffer first.
//          If there are enough data in ring buffer, copy them to xfer->data and return.
//          If there are not enough data in ring buffer, copy all of them to xfer->data,
//          save the xfer->data remained empty space to uart handle, receive data
//          to this empty space and trigger callback when finished. */
//    if (kUSART_RxBusy == handle->rxState)
//    {
//        return kStatus_USART_RxBusy;
//    }
//    else
//    {
//        bytesToReceive       = xfer->dataSize;
//        bytesCurrentReceived = 0U;
//        /* If RX ring buffer is used. */
//        if (handle->rxRingBuffer)
//        {
//            /* Disable IRQ, protect ring buffer. */
//            regPrimask = DisableGlobalIRQ();
//            /* How many bytes in RX ring buffer currently. */
//            bytesToCopy = USART_TransferGetRxRingBufferLength(handle);
//            if (bytesToCopy)
//            {
//                bytesToCopy = MIN(bytesToReceive, bytesToCopy);
//                bytesToReceive -= bytesToCopy;
//                /* Copy data from ring buffer to user memory. */
//                for (i = 0U; i < bytesToCopy; i++)
//                {
//                    xfer->data[bytesCurrentReceived++] = handle->rxRingBuffer[handle->rxRingBufferTail];
//                    /* Wrap to 0. Not use modulo (%) because it might be large and slow. */
//                    if (handle->rxRingBufferTail + 1U == handle->rxRingBufferSize)
//                    {
//                        handle->rxRingBufferTail = 0U;
//                    }
//                    else
//                    {
//                        handle->rxRingBufferTail++;
//                    }
//                }
//            }
//            /* If ring buffer does not have enough data, still need to read more data. */
//            if (bytesToReceive)
//            {
//                /* No data in ring buffer, save the request to UART handle. */
//                handle->rxData        = xfer->data + bytesCurrentReceived;
//                handle->rxDataSize    = bytesToReceive;
//                handle->rxDataSizeAll = bytesToReceive;
//                handle->rxState       = kUSART_RxBusy;
//            }
//            /* Enable IRQ if previously enabled. */
//            EnableGlobalIRQ(regPrimask);
//            /* Call user callback since all data are received. */
//            if (0 == bytesToReceive)
//            {
//                if (handle->callback)
//                {
//                    handle->callback(base, handle, kStatus_USART_RxIdle, handle->userData);
//                }
//            }
//        }
//        /* Ring buffer not used. */
//        else
//        {
//            handle->rxData        = xfer->data + bytesCurrentReceived;
//            handle->rxDataSize    = bytesToReceive;
//            handle->rxDataSizeAll = bytesToReceive;
//            handle->rxState       = kUSART_RxBusy;
//
//            /* Enable RX interrupt. */
//            base->FIFOINTENSET |= USART_FIFOINTENSET_RXLVL_MASK;
//        }
//        /* Return the how many bytes have read. */
//        if (receivedBytes)
//        {
//            *receivedBytes = bytesCurrentReceived;
//        }
//    }
//    return kStatus_Success;
//}

//void gm01q_tools_Sleep(unsigned int ms)
//{
//	size_t i;
//	for(i=0; i<(ms * 10000); i++) asm("nop");
//}
//
//void gm01q_tools_CleanBuff(uint8_t* buff, size_t buff_size)
//{
//	/* Old style
//	size_t i = 0;
//
//    for(i=0; buff[i] != '\0'; i++)
//    {
//    	buff[i] = '\0';
//    }
//    */
//	memset(buff, 0, buff_size - 1);
//}
//
//size_t gm01q_tools_mySizeoff(const uint8_t* buff)
//{
//	size_t i = 0;
//	size_t size=0;
//
//    for(i=0; buff[i] != '\0'; i++)
//    {
//    	size++;
//    }
//
//    return size;
//}
//
//void gm01q_tools_Transformbin(uint8_t * binbuffer, uint8_t valuetotransform)
//{
//	uint8_t i = 0;
//	uint8_t bit = 0 ;
//	uint8_t mask = 1 ;
//	char temp[8] = {0};
//
//	/* transform the value in binary */
//	for (uint8_t i = 0 ; i < 8 ; ++i)
//	{
//		bit = (valuetotransform & mask) >> i ;
//		sprintf(temp+i,"%d", bit) ;
//		mask <<= 1 ;
//	}
//
//	/* inverse the binary result to be well disply in the command */
//	for (i = 0 ; i < 8 ; ++i)
//	{
//		binbuffer[i] = temp[7-i];
//	}
//
//	/* be sure the string is ended by "\0" */
//	sprintf((char*)binbuffer+i,"\0") ;
//}
//
//void gm01q_tools_ParamStringPrepare(const uint8_t * string, uint8_t * stringprepared)
//{
//	strcat((char*)stringprepared, "\"");
//	strcat((char*)stringprepared, (char*)string);
//	strcat((char*)stringprepared, "\"");;
//}
//
//void gm01q_tools_StartPrepareCommand(const uint8_t * cmd)
//{
//	gm01q_tools_CleanBuff(g_rxBuffer, (size_t)AT_BUFFER_SIZE);
//	strcat((char*)g_txBuffer, (char*)cmd);
//}
//
//void gm01q_tools_AddNumParam(size_t numparameter, GM01Q_PREPARE_CMD isnewparam)
//{
//	uint8_t stringparamer [stringparm_maxsize] = {0};
//
//	itoa(numparameter, (char*)stringparamer, 10);
//	gm01q_tools_AddStringParam(stringparamer, isnewparam);
//}
//
//void gm01q_tools_AddStringParam(uint8_t * parameter, GM01Q_PREPARE_CMD isnewparam)
//{
//	strcat((char*)g_txBuffer, (char*)parameter);
//
//	if (isnewparam == GM01Q_PREPARE_CMD_CONTINUE)
//	{
//		gm01q_tools_SeparateParam();
//	}
//	else
//	{
//		gm01q_tools_CloseCommand();
//	}
//}
//
//void gm01q_tools_SeparateParam(void)
//{
//	strcat((char*)g_txBuffer, ",");
//}
//
//void gm01q_tools_CloseCommand(void)
//{
//	strcat((char*)g_txBuffer, "\r");
//}
//
//
//
//bool gm01q_tools_ATRespCheck(uint8_t * response, const uint8_t * expected)
//{
//    int i = 0;
//
//    do
//    {
//        if (response[i] != expected[i])
//        {
//        	return false;
//        }
//        i++;
//    }
//    while (response[i-1] != 0);
//
//    return true;
//}
//
//
//
//
//bool gm01q_tools_ATFirstByteRespCheck(uint8_t * response, const uint8_t * expected, uint8_t FirstRespByte)
//{
//    int i = 0;
//
//    do
//    {
//        if (response[i] != expected[i])
//        {
//        	return false;
//        }
//        i++;
//    }
//    while (i != FirstRespByte);
//
//    return true;
//}
//
//
//uint8_t gm01q_tools_NetworkRegistrationUnsolicitedCheck(const uint8_t * NetworkRegNotif)
//{
//    uint8_t sizenotiftemplate = 0;
//    uint8_t networkregnotifvalue = 0;
//
//    sizenotiftemplate = sizeof(rxCREG)-1;
//    networkregnotifvalue = NetworkRegNotif[sizenotiftemplate];
//
//    // transform AscII value as num value
//    networkregnotifvalue = networkregnotifvalue - 0x30;
//
//    return networkregnotifvalue;
//}
//
//
/////*********************************************************************************
////* Configurations Functions
////*********************************************************************************/
////void gmO1q_atcmd_usart2_cfg(void)
////{
////	lpc55_usart2_cfg();
////}
////
/////*********************************************************************************
////* Hardware Reset Functions
////*********************************************************************************/
////
////
////void gm01q_tools_nReset_GPIO_Deasserted(void)
////{
////	lpc55_PIO0_18_GPIO_ON();
////}
////
////void gm01q_tools_nReset_GPIO_Asserted(void)
////{
////	lpc55_PIO0_18_GPIO_ON();
////}
////
////
////
////
/////*********************************************************************************
////* Sleep Mode Functions
////*********************************************************************************/
////void gmO1q_atcmd_RTS_FlowControl_cfg(void)
////{
////	lpc55_PIO1_27_usart2_RTS_Init();
////}
////
////void gmO1q_atcmd_RTS_GPIO_cfg(void)
////{
////	lpc55_PIO1_27_GPIO_Init();
////}
////
////void gm01q_tools_RTS_GPIO_Deasserted(void)
////{
////	lpc55_PIO1_27_GPIO_ON();
////}
////
////void gm01q_tools_RTS_GPIO_Asserted(void)
////{
////	lpc55_PIO1_27_GPIO_OFF();
////}
////
////void gmO1q_atcmd_SleepMode(void)
////{
////	/* configure USART2 RTS pin as GPIO -> LOGIC_GPIO_OFF */
////	gmO1q_atcmd_RTS_GPIO_cfg();
////	/* Set RTS pin in Hight level */
////	gm01q_tools_RTS_GPIO_Deasserted();
////}
////
////void gmO1q_atcmd_WakeUp(void)
////{
////	/* Set RTS pin in Low level */
////	gm01q_tools_RTS_GPIO_Asserted();
////
////	/* configure USART2 RTS pin as FlowControl -> default logic state at low level */
////	gmO1q_atcmd_RTS_FlowControl_cfg();
////}
//
//
///*************************************************************************************************************
// An ATEcho is the echo, coming from GM01Q, of the last command sent by the LPC55 ('\r' character included)
// *************************************************************************************************************/
//bool gm01q_tools_IsATCmdEcho(uint8_t * buff, size_t CmdSize)
//{
//	bool IsAck = false;
//
//	/* verify the last bayte received is well the end of the command: '\r' */
//	if (buff[CmdSize-1] == '\r')
//	{
//		IsAck = true;
//	}
//
//	return IsAck;
//}
//
//
///***************************************************************************************************
// An ATCertificateEcho is the echo, coming from GM01Q, of the certificate sent by the LPC55
// **************************************************************************************************/
//bool gm01q_tools_IsATCertificateEcho(uint8_t * buff, size_t CmdSize)
//{
//	bool IsAck = false;
//
//	/* the last byte is '-' */
//	if (buff[CmdSize-1] == '-')
//	{
//		IsAck = true;
//	}
//
//	return IsAck;
//}
//
//
///****************************************************************************************
// An AT Response start by '\r\n'
// *****************************************************************************************/
//bool gm01q_tools_IsATResp(uint8_t * buff)
//{
//	bool isatresp = false;
//
//	/* If buff is not empty */
//	if (buff[0] != '\0')
//	{
//		/* If the first byte of the buffer is '\r' then it could be a response */
//		if(buff[0] == '\r')
//		{
//			/* If the second byte of the buffer is '\n' then it is a response */
//			if(buff[1] == '\n')
//			{
//				isatresp = true;
//			}
//		}
//	}
//    return isatresp;
//}
//
//
///*********************************************************************************
//* Data Frame Functions
//*********************************************************************************/
//
//void gm01q_tools_SendATCmd(const uint8_t * buff, const size_t buff_size)
//{
//	//EventBits_t ev;
//	//gm01q_tools_CleanBuff(g_rxBuffer, (size_t)AT_BUFFER_SIZE);
//
//#if DEBUG
//	PRINTF((char *)buff);
//#endif
//
//    //ATCmdsize = gm01q_tools_mySizeoff(ATCmd);
//    //lpC55_WriteUARTBlocking(ATCmd, buff_size);
//	xSemaphoreTake( GET_DRIVER_CXT(p_Global_Context)->sendSemaphore, 0);
//	GET_DRIVER_CXT(p_Global_Context)->sendXfer.data = (uint8_t *)buff;
//	GET_DRIVER_CXT(p_Global_Context)->sendXfer.dataSize = buff_size;
//	xSemaphoreGive( GET_DRIVER_CXT(p_Global_Context)->sendSemaphore );
//	//USART_TransferSendNonBlocking(NBIOTSHIELD_USART, &NBIOTSHIELD_USART_HANDLE, &sendXfer);
//	//xQueueSend( GET_DRIVER_CXT(p_Global_Context)->q_txQueue, &sendXfer, 0);
//	xEventGroupSetBits( GET_DRIVER_CXT(p_Global_Context)->e_UsartEvent, (const EventBits_t)RTOS_USART_SEND_PENDING );
//}
//
///****** gm01q_CatchResponse catch response in the format \r\n<Response>\r\n   *****************/
//void gm01q_tools_CatchResponse(void)
//{
//    size_t index =0;
//    size_t dataread =0;
//    bool responcecomplete = false;
//    bool IsStartResponse = false;
//
//    gm01q_tools_CleanBuff(g_rxBuffer, (size_t)AT_BUFFER_SIZE);
//
//    /* Wait the start response \r\n<ResponseFirstByte> at least 3 bytes */
//	while (index < 2 )
//	{
//		/* Wait the start response \r\n */
//		//dataread = lpc55_ReadUART(g_rxBuffer+index);
//		index = index+dataread;
//	}
//
//	/* Verify it is well a start response */
//	IsStartResponse = gm01q_tools_IsATResp(g_rxBuffer);
//
//	if (IsStartResponse == true)
//	{
//		while(!responcecomplete)
//		{
//			/* Wait the end response \n */
//			//dataread = lpc55_ReadUART(g_rxBuffer+index);
//			index = index+dataread;
//
//			if((g_rxBuffer[index -1] == '\n') && (index > 3))
//			{
//				responcecomplete = true;
//			}
//		}
//	}
//	else
//	{
//		/* To Be Changed after PoC */
//		assert("gm01q_CatchResponse: : it is not a response from gm01q");
//	}
//
//#if DEBUG
//	PRINTF((char *) g_rxBuffer);
//#endif
//
//}
//
///****** gm01q_WaitPrompt wait prompt in the format '>_'   *****************/
//void gm01q_tools_WaitPrompt(void)
//{
//    size_t index =0;
//    size_t dataread =0;
//    bool ispromptreceived = false;
//    bool isstartresponse = false;
//
//	while (index < 2 )
//	{
//		/* Wait the start response \r\n<ResponseFirstByte> at least 3 bytes */
//		//dataread = lpc55_ReadUART(g_rxBuffer+index);
//		index = index+dataread;
//	}
//
//	/* Verify it is well a start response */
//	isstartresponse = gm01q_tools_IsATResp(g_rxBuffer);
//
//	if (isstartresponse == true)
//	{
//		while(!ispromptreceived)
//		{
//			/* Wait the prompt '> ' */
//			//dataread = lpc55_ReadUART(g_rxBuffer+index);
//			index = index+dataread;
//
//			if((g_rxBuffer[index -2] == '>') && (g_rxBuffer[index -1] == ' ') )
//			{
//				ispromptreceived = true;
//			}
//		}
//	}
//	else
//	{
//		/* To Be Changed after PoC */
//		assert("gm01q_WaitPrompt: it is not a response from gm01q");
//	}
//
//#if DEBUG
//	PRINTF((char *)g_rxBuffer);
//#endif
//}
//
//
//
///*T3412-Extended is expressed as a string of a binary number encoded on 8 bits: 3 bits for the unit, 5 bits for the value*/
//void gm01q_tools_T3412ExtendedValueSet(SQNS_T3412EXTUNIT unit, uint8_t timervalue)
//{
//	int T3412extendedvalue = 0;
//	uint8_t stringT3412extendedvalue[9]= {0};
//
//	if (timervalue > 31)
//	{
//		timervalue = 31;
//	}
//
//	T3412extendedvalue = timervalue | unit;
//
//	/* transform T3324value in binary and write the result in stringT3324value */
//	gm01q_tools_Transformbin(stringT3412extendedvalue, T3412extendedvalue);
//
//	/*Set the Requested_PeriodicTAU parameter */
//	strcat((char *)Requested_PeriodicTAU, "\"");
//	strcat((char *)Requested_PeriodicTAU, (char *)stringT3412extendedvalue);
//	strcat((char *)Requested_PeriodicTAU, "\"");
//}
//
///*T3324 is expressed as a string of a binary number encoded on 8 bits: 3 bits for the unit, 5 bits for the value*/
//void gm01q_tools_T3324ValueSet(SQNS_T3324UNIT unit, uint8_t timervalue)
//{
//	int T3324value = 0;
//	uint8_t stringT3324value[9]= {0};
//
//	if (timervalue > 31)
//	{
//		timervalue = 31;
//	}
//
//	T3324value = timervalue | unit;
//
//	/* transform T3324value in binary and write the result in stringT3324value */
//	gm01q_tools_Transformbin(stringT3324value, T3324value);
//
//	/*Set the Requested_ActiveTime parameter */
//	strcat((char *)Requested_ActiveTime, "\"");
//	strcat((char *)Requested_ActiveTime, (char *)stringT3324value);
//	strcat((char *)Requested_ActiveTime, "\"");
//}
//
//void gm01q_tools_CatchATFSMResp(void)
//{
//	size_t datareceived = 0;
//	size_t fsmrespindex = 0;
//	bool goout = false;
//
//    /* Wait AT!="fsm" is completely received - response = 1252 bytes*/
//    while ((fsmrespindex < 1252) && !goout)
//    {
//    	/*Read data and copy it in g_rxBuffer */
//    	//datareceived = lpc55_ReadUART(g_rxBuffer+fsmrespindex);
//    	fsmrespindex = fsmrespindex + datareceived;
//
//    	if((datareceived == 0) && (fsmrespindex > sizeof(rxOK)) && !goout)
//    	{
//    		goout = gm01q_tools_ATFirstByteRespCheck(g_rxBuffer+(fsmrespindex-sizeof(rxOK)+1), rxOK, sizeof(rxOK)-1);
//    	}
//    }
//
//#if DEBUG
//	PRINTF((const char *)g_rxBuffer);
//	gm01q_tools_Sleep(1000);
//#endif
//}
//
//void gm01q_tools_CatchATResponse(size_t ATCmdSize)
//{
//	if(gm01q_EchoModeActivated)
//	{
//		/* Catch the echo from GM01Q modem */
//		gm01q_tools_EchoCatchCmd(ATCmdSize);
//	}
//
//	/* Catch the Response from GM01Q modem */
//	gm01q_tools_CatchResponse();
//}
//
//void gm01q_tools_WaitATPrompt(size_t ATCmdSize)
//{
//	if(gm01q_EchoModeActivated)
//	{
//		/* Catch the ACK from GM01Q modem */
//		gm01q_tools_EchoCatchCmd(ATCmdSize);
//	}
//
//	gm01q_tools_WaitPrompt();
//}
//
//
//
///*********************************************************************************
//* Data Functions for Echo Mode
//*********************************************************************************/
//bool gm01q_tools_EchoCatchCmd(size_t ATCmdSize)
//{
//	bool isechocmd = false;
//	size_t echodatareceived = 0;
//	size_t echoindex = 0;
//
//    gm01q_tools_CleanBuff(g_rxBuffer, (size_t)AT_BUFFER_SIZE);
//
//    /* Wait AT Cmd echo is completely received */
//    while (echoindex < ATCmdSize)
//    {
//    	/*Read the echo of the AT command and copy it in g_rxBuffer */
//    	//echodatareceived = lpc55_ReadUART(g_rxBuffer+echoindex);
//    	echoindex = echoindex + echodatareceived;
//    }
//
//    /* Check the ACk is completed and well received */
//    isechocmd = gm01q_tools_IsATCmdEcho(g_rxBuffer, ATCmdSize);
//
//	if (isechocmd == false)
//	{
//		/* To Be Changed after PoC */
//		assert("It is not an echo");
//	}
//
//#if DEBUGECHO
//        PRINTF((const uint8_t *) g_rxBuffer);
//#endif
//
//    return isechocmd;
//}
//
//bool gm01q_tools_EchoCatchCertificate(size_t CertificateSize)
//{
//	bool isechocert = false;
//	size_t echodatareceived = 0;
//    size_t echoindex = 0;
//
//    /* Wait certificate or private key echo is completely received */
//    while (echoindex < CertificateSize)
//    {
//    	/*Read the echo of the certificate and copy it in g_rxBuffer */
//    	//echodatareceived = lpc55_ReadUART(g_rxBuffer+echoindex);
//    	echoindex = echoindex + echodatareceived;
//    }
//
//    /* Check the ACk is completed and well received */
//    isechocert = gm01q_tools_IsATCertificateEcho(g_rxBuffer, CertificateSize);
//
//	if (isechocert == false)
//	{
//		/* To Be Changed after PoC */
//		assert("it is not an Certificate or Private Key Echo");
//	}
//#if DEBUGECHO
//        PRINTF((const uint8_t *)g_rxBuffer);
//#endif
//
//    return isechocert;
//}
//
//
///*********************************************************************************
//* New Functions
//*********************************************************************************/
//
//#if 0
//bool gm01q_PingURL(void)
//{
//	bool modemresponseok = false;
//
//
//    gm01q_SendATCmd(CMD_ATPINGMYAWS);
//
//	gm01q_CatchATPing(sizeof(CMD_ATPINGMYAWS)-1);
//	//modemresponseok = gm01q_ATFirstByteRespCheck(AT_PING1, urcPING1, sizeof(urcPING1)-1);
//
//	gm01q_CatchResponse();
//	modemresponseok = gm01q_ATRespCheck(g_rxBuffer, rxOK);
//
//    return modemresponseok;
//}
//
//
//void gm01q_CatchATPing(size_t ATCmdSize)
//{
//	bool EchoReceived = false;
//
//	CleanAllBuffers();
//
//	if(gm01q_EchoModeActivated)
//	{
//		/* Catch the ACK from GM01Q modem */
//		EchoReceived = gm01q_EchoCatchCmd(ATCmdSize);
//	}
//
//	gm01q_CatchPing();
//}
//
//
///****** gm01q_CatchPing wait the 4 ping in format "\r\n ping ..."   *****************/
//void gm01q_CatchPing(void)
//{
//    size_t index =0;
//    size_t dataread =0;
//    bool ispromptreceived = false;
//    bool isstartresponse = false;
//    uint8_t pingreceived = 0;
//
//    while (pingreceived != 4)
//    {
//		while (index < 2 )
//		{
//			/* Wait the start response \r\n<ResponseFirstByte> at least 3 bytes */
//			dataread = lpc55_ReadUART(g_rxBuffer+index);
//			index = index+dataread;
//		}
//
//		/* Verify it is well a start response */
//		isstartresponse = IsATResp(g_rxBuffer);
//
//		if (isstartresponse == true)
//		{
//			while(!ispromptreceived)
//			{
//				/* Wait the prompt '> ' */
//				dataread = lpc55_ReadUART(g_rxBuffer+index);
//				index = index+dataread;
//
//				if((g_rxBuffer[index -2] == '>') && (g_rxBuffer[index -1] == ' ') )
//				{
//					ispromptreceived = true;
//				}
//			}
//		}
//		else
//		{
//			/* To Be Changed after PoC */
//			assert("gm01q_CatchPing: it is not a ping from gm01q");
//		}
//#if DEBUG
//        PRINTF((const uint8_t *)g_rxBuffer);
//#endif
//    }
//}
//#endif

