/*
 * Copyright 2019-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MSFT_AZURE_C_
#define MSFT_AZURE_C_

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "msft_Azure_IoT.h"
#include "msft_Azure_IoT_clientcredential.h"
#include "iot_mqtt_agent.h"
#include "iot_mqtt_types.h"
#include "event_groups.h"
#include "board.h"
#include "iot_init.h"
#include "azure_default_root_certificates.h"
#include "azure_iotc_utils.h"
#include "iotc_json.h"
#include "gsm_private.h"

/* Maximum amount of time a Shadow function call may block. */
#define AzureTwinDemoTIMEOUT                    pdMS_TO_TICKS( 30000UL )

#define Device_Telemetry_JSON 				\
	"{"										\
		"\"aX\": %.2f", 					\
		"\"aY\": %.2f", 					\
		"\"aZ\": %.2f", 					\
		"\"light_intensity\": %.2f", 		\
		"\"rssi\": %d", 					\
		"\"device_id\": 19494031513", 		\
		"\"modem_fw\": \"UE5.2.0.1\"", 		\
		"\"imei\": 354658090355378", 		\
		"\"iccid\": 89148000005471125146"	\
	"}"

#define Device_Twin_SET_JSON 								\
	"{"														\
		"\"manufacturer\": \"Avnet\"",						\
		"\"model\": \"Monarch LTE-M Dev Kit\"",				\
		"\"swVersion\": \"v1.0\"",							\
		"\"osName\": \"FreeRTOS\"",							\
		"\"processorArchitecture\": \"LPC55S69\"",			\
		"\"processorManufacturer\": \"NXP Semiconductor\"",	\
		"\"totalStorage\": 640",							\
		"\"totalMemory\": 960",								\
	"}"

const char strTrue[] = "TRUE";
const char strFalse[] = "FALSE";
const char strReported[] = "\"reported\"";
const char strRgb_red[] = "\"rgb_red\":";
const char strRgb_green[] = "\"rgb_green\":";
const char strRgb_blue[] = "\"rgb_blue\":";

typedef enum AZURE_TWIN_TASK_ST
{
	AZURE_TW_IDLE,
	AZURE_TW_SUB_C2DM,	/* Subscription to cloud-to-device messages topic */
	AZURE_TW_SUB_DTWR,	/* Subscription to device twin's response topic */
	AZURE_TW_SUB_DM,	/* Subscription to direct method topic */
	AZURE_TW_SUB_DT,	/* Subscription to device telemetry topic */
	AZURE_TW_SUB_DTWPC,	/* Subscription to device twin's property changes topic */
	AZURE_TW_SET_STATE,
	AZURE_TW_WAIT_SET_RESP,
	AZURE_TELEMETRY_SET_STATE,
	AZURE_TELEMETRY_WAIT_SET_STATE,
	AZURE_TW_GET_STATE,
	AZURE_TW_WAIT_GET_RESP,
	AZURE_STATES_BNDRY
}Azure_TW_Task;

static MQTTAgentConnectParams_t xConnectParams;
const MQTTAgentConnectParams_t * pConnectParams = &xConnectParams;
MQTTAgentHandle_t xMQTTHandle;

static Azure_TW_Task enAzure_TW_Task;
static MQTTAgentSubscribeParams_t xSubscribeParams;
static MQTTAgentUnsubscribeParams_t xUnsubscribeParams;
static MQTTAgentPublishParams_t xPublishParameters;
EventGroupHandle_t xCreatedEventGroup;

char SAS_token[256];

void deviceTwinGetCallback(char * propertyName, char * payload, size_t payload_len)
{
	AZURE_PRINTF(("Received a SettingsUpdated event"));
	jsobject_t object;
	jsobject_initialize(&object, payload, payload_len);

	if (strcmp(propertyName, "rgb_red") == 0)
	{
		jsobject_t rgb_red;
		jsobject_get_object_by_name(&object, "rgb_red", &rgb_red);
		int v = jsobject_get_number_by_name(&rgb_red, "value");
		AZURE_PRINTF(("== Received a 'RED LED' update! New Value => %d", v));
		jsobject_free(&rgb_red);
	}
	else if (strcmp(propertyName, "rgb_green") == 0)
	{
		jsobject_t rgb_green;
		jsobject_get_object_by_name(&object, "rgb_green", &rgb_green);
		int v = jsobject_get_number_by_name(&rgb_green, "value");
		AZURE_PRINTF(("== Received a 'GREEN LED' update! New Value => %d", v));
		jsobject_free(&rgb_green);
	}
	else if (strcmp(propertyName, "rgb_blue") == 0)
	{
		jsobject_t rgb_blue;
		jsobject_get_object_by_name(&object, "rgb_blue", &rgb_blue);
		int v = jsobject_get_number_by_name(&rgb_blue, "value");
		AZURE_PRINTF(("== Received a 'BLUE LED' update! New Value => %d", v));
		jsobject_free(&rgb_blue);
	}
	else
	{
		// payload may not have a null ending
		AZURE_PRINTF(("Unknown Settings. Payload => %.*s", payload_len, payload));
	}
	jsobject_free(&object);
}

MQTTBool_t Azure_IoT_CallBack(void * pvPublishCallbackContext,
        							const MQTTPublishData_t * const pxPublishData )
{
	char * msg = (char *)pxPublishData->pvData;
	uint64_t msg_length = pxPublishData->ulDataLength;
	char * topic = (char *)pxPublishData->pucTopic;
	uint64_t topic_length = pxPublishData->usTopicLength;

	if (topic_length == 0)
	{
		AZURE_PRINTF( ( "ERROR: Azure_IoT_CallBack without a topic.\r\n" ) );
		return eMQTTFalse;
	}

	if (topic_check("$iothub/twin/PATCH/properties/desired/",
			        strlen("$iothub/twin/PATCH/properties/desired/"),
					topic,
					topic_length
					)
		)
	{
		/* Device Twin Get received */
		jsobject_t desired;
		jsobject_initialize(&desired, msg, msg_length);

		for (unsigned i = 0, count = jsobject_get_count(&desired); i < count;
		   i += 2) {
		char *itemName = jsobject_get_name_at(&desired, i);
		if (itemName != NULL && itemName[0] != '$') {
			deviceTwinGetCallback(itemName, msg, msg_length);
		}
		if (itemName) AZURE_IOTC_FREE(itemName);
		}
		jsobject_free(&desired);
	}


	xEventGroupSetBits(xCreatedEventGroup, 0x01);
	MQTT_AGENT_ReturnBuffer(( MQTTAgentHandle_t) 2, pxPublishData->xBuffer);

	return eMQTTTrue;
}

void prvmcsft_Azure_TwinTask( void * pvParameters )
{
	MQTTAgentReturnCode_t xMQTTReturn;
	char cPayload[100];
	char cTopic[50];
	uint8_t Req_Id =1;
	double aX, aY, aZ, light_intensity;
	aX = 0.2;
	aY = 50.7;
	aZ = 100.5;
	light_intensity = 79.1;


    ( void ) pvParameters;

    /* Initialize common libraries required by demo. */
	if (IotSdk_Init() != true)
	{
		configPRINTF(("Failed to initialize the common library."));
		vTaskDelete(NULL);
	}

    if( MQTT_AGENT_Create( &xMQTTHandle ) != eMQTTAgentSuccess )
    {
        configPRINTF(("Failed to initialize the MQTT Handle, stopping demo.\r\n"));
        vTaskDelete(NULL);
    }

    MQTT_AGENT_Init();

    memset(SAS_token, 0, sizeof(SAS_token));
    generate_sas_token(SAS_token, sizeof(SAS_token));

    memset( &xConnectParams, 0x00, sizeof( xConnectParams ) );
    xConnectParams.pcURL = clientcredentialAZURE_MQTT_BROKER_ENDPOINT;
    xConnectParams.usPort = clientcredentialAZURE_MQTT_BROKER_PORT;

    xConnectParams.xFlags = mqttagentREQUIRE_TLS;
    xConnectParams.pcCertificate = (char *)AZURE_SERVER_ROOT_CERTIFICATE_PEM;
    xConnectParams.ulCertificateSize = sizeof(AZURE_SERVER_ROOT_CERTIFICATE_PEM);
    xConnectParams.pxCallback = NULL;
    xConnectParams.pvUserData = NULL;

    xConnectParams.pucClientId = (const uint8_t *)(clientcredentialAZURE_IOT_THING_NAME);
    xConnectParams.usClientIdLength = (uint16_t)strlen(clientcredentialAZURE_IOT_THING_NAME);
#if SSS_USE_FTR_FILE
    xConnectParams.cUserName = clientcredentialAZURE_IOT_MQTT_USERNAME;
    xConnectParams.uUsernamelength = ( uint16_t ) strlen(clientcredentialAZURE_IOT_MQTT_USERNAME);
#ifdef SAS_KEY
    xConnectParams.p_password = SAS_token ;
    xConnectParams.passwordlength = ( uint16_t ) strlen(SAS_token);
#else
    xConnectParams.p_password = NULL;
    xConnectParams.passwordlength = 0;
#endif
#endif

    enAzure_TW_Task = AZURE_TW_IDLE;

    xCreatedEventGroup = xEventGroupCreate();

    while( 1 )
    {
    	switch( enAzure_TW_Task )
    	{
    		case AZURE_TW_IDLE:

    		    xMQTTReturn = MQTT_AGENT_Connect( xMQTTHandle,
    		    								  pConnectParams,
    											  AzureTwinDemoTIMEOUT);

    		    if( eMQTTAgentSuccess == xMQTTReturn )
    		    {
    		    	AZURE_PRINTF( ("Connected to Azure IoT Hub Successfully\r\n") );
    		    	enAzure_TW_Task = AZURE_TW_SUB_C2DM;
    		    }
    		    else
    		    {
    		    	AZURE_PRINTF( ("Connection refused!! \r\n") );
    		    	vTaskDelete(NULL);
    		    }
    		    break;

    		case AZURE_TW_SUB_C2DM:

    			xSubscribeParams.pucTopic = (const uint8_t *)AZURE_IOT_C2D_TOPIC_FOR_SUB;
				xSubscribeParams.pvPublishCallbackContext = NULL;
				xSubscribeParams.pxPublishCallback = Azure_IoT_CallBack;
				xSubscribeParams.usTopicLength = sizeof(AZURE_IOT_C2D_TOPIC_FOR_SUB) - 1;
				xSubscribeParams.xQoS = eMQTTQoS0;

				xUnsubscribeParams.pucTopic = (const uint8_t *)AZURE_IOT_C2D_TOPIC_FOR_SUB;
				xUnsubscribeParams.usTopicLength = sizeof(AZURE_IOT_C2D_TOPIC_FOR_SUB) - 1;

				if( MQTT_AGENT_Subscribe(xMQTTHandle, &xSubscribeParams, pdMS_TO_TICKS(10000)) == eMQTTAgentSuccess)
				{
					AZURE_PRINTF( ("Successfully Subscribe to Operations Topic\r\n") );
					enAzure_TW_Task = AZURE_TW_SUB_DTWR;
				}
				else
				{
					AZURE_PRINTF( ("Unsuccessfully Subscribe to Operations Topic\r\n"));
					AZURE_PRINTF( ("Disconnect\r\n"));
					MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);
					enAzure_TW_Task = AZURE_TW_IDLE;
				}
				break;

    		case AZURE_TW_SUB_DTWR:

    		    xSubscribeParams.pucTopic = (const uint8_t *)AZURE_IOT_TWIN_RESPONSE_TOPIC_FOR_SUB;
    		    xSubscribeParams.pvPublishCallbackContext = NULL;
    		    xSubscribeParams.pxPublishCallback = Azure_IoT_CallBack;
    		    xSubscribeParams.usTopicLength = sizeof(AZURE_IOT_TWIN_RESPONSE_TOPIC_FOR_SUB) - 1;
    		    xSubscribeParams.xQoS = eMQTTQoS0;

    		    xUnsubscribeParams.pucTopic = (const uint8_t *)AZURE_IOT_TWIN_RESPONSE_TOPIC_FOR_SUB;
    		    xUnsubscribeParams.usTopicLength = sizeof(AZURE_IOT_TWIN_RESPONSE_TOPIC_FOR_SUB) - 1;

    		    if( MQTT_AGENT_Subscribe(xMQTTHandle, &xSubscribeParams, pdMS_TO_TICKS(10000)) == eMQTTAgentSuccess)
    		    {
    		    	AZURE_PRINTF( ("Successfully Subscribe to Operations Topic\r\n") );
    		    	enAzure_TW_Task = AZURE_TW_SUB_DM;
    		    }
                else
                {
                	AZURE_PRINTF( ("Unsuccessfully Subscribe to Operations Topic\r\n"));
                	AZURE_PRINTF( ("Disconnect\r\n"));
                	MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);
                	enAzure_TW_Task = AZURE_TW_IDLE;
                }

    			break;

    		case AZURE_TW_SUB_DM:

				xSubscribeParams.pucTopic = (const uint8_t *)AZURE_IOT_METHOD_TOPIC_FOR_SUB;
				xSubscribeParams.pvPublishCallbackContext = NULL;
				xSubscribeParams.pxPublishCallback = Azure_IoT_CallBack;
				xSubscribeParams.usTopicLength = sizeof(AZURE_IOT_METHOD_TOPIC_FOR_SUB) - 1;
				xSubscribeParams.xQoS = eMQTTQoS0;

				xUnsubscribeParams.pucTopic = (const uint8_t *)AZURE_IOT_METHOD_TOPIC_FOR_SUB;
				xUnsubscribeParams.usTopicLength = sizeof(AZURE_IOT_METHOD_TOPIC_FOR_SUB) - 1;

				if( MQTT_AGENT_Subscribe(xMQTTHandle, &xSubscribeParams, pdMS_TO_TICKS(10000)) == eMQTTAgentSuccess)
				{
					AZURE_PRINTF( ("Successfully Subscribe to Operations Topic\r\n") );
					enAzure_TW_Task = AZURE_TW_SUB_DT;
				}
				else
				{
					AZURE_PRINTF( ("Unsuccessfully Subscribe to Operations Topic\r\n"));
					AZURE_PRINTF( ("Disconnect\r\n"));
					MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);
					enAzure_TW_Task = AZURE_TW_IDLE;
				}

				break;

    		case AZURE_TW_SUB_DT:

				xSubscribeParams.pucTopic = (const uint8_t *)AZURE_IOT_TELEMETRY_TOPIC_FOR_SUB;
				xSubscribeParams.pvPublishCallbackContext = NULL;
				xSubscribeParams.pxPublishCallback = Azure_IoT_CallBack;
				xSubscribeParams.usTopicLength = sizeof(AZURE_IOT_TELEMETRY_TOPIC_FOR_SUB) - 1;
				xSubscribeParams.xQoS = eMQTTQoS0;

				xUnsubscribeParams.pucTopic = (const uint8_t *)AZURE_IOT_TELEMETRY_TOPIC_FOR_SUB;
				xUnsubscribeParams.usTopicLength = sizeof(AZURE_IOT_TELEMETRY_TOPIC_FOR_SUB) - 1;

				if( MQTT_AGENT_Subscribe(xMQTTHandle, &xSubscribeParams, pdMS_TO_TICKS(10000)) == eMQTTAgentSuccess)
				{
					AZURE_PRINTF( ("Successfully Subscribe to Operations Topic\r\n") );
					enAzure_TW_Task = AZURE_TW_SUB_DTWPC;
				}
				else
				{
					AZURE_PRINTF( ("Unsuccessfully Subscribe to Operations Topic\r\n"));
					AZURE_PRINTF( ("Disconnect\r\n"));
					MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);
					enAzure_TW_Task = AZURE_TW_IDLE;
				}

				break;

			case AZURE_TW_SUB_DTWPC:

				xSubscribeParams.pucTopic = (const uint8_t *)AZURE_IOT_TWIN_PATCH_TOPIC_FOR_SUB;
				xSubscribeParams.pvPublishCallbackContext = NULL;
				xSubscribeParams.pxPublishCallback = Azure_IoT_CallBack;
				xSubscribeParams.usTopicLength = sizeof(AZURE_IOT_TWIN_PATCH_TOPIC_FOR_SUB) - 1;
				xSubscribeParams.xQoS = eMQTTQoS0;

				xUnsubscribeParams.pucTopic = (const uint8_t *)AZURE_IOT_TWIN_PATCH_TOPIC_FOR_SUB;
				xUnsubscribeParams.usTopicLength = sizeof(AZURE_IOT_TWIN_PATCH_TOPIC_FOR_SUB) - 1;

				if( MQTT_AGENT_Subscribe(xMQTTHandle, &xSubscribeParams, pdMS_TO_TICKS(10000)) == eMQTTAgentSuccess)
				{
					AZURE_PRINTF( ("Successfully Subscribe to Operations Topic\r\n") );
					enAzure_TW_Task = AZURE_TW_SET_STATE;
				}
				else
				{
					AZURE_PRINTF( ("Unsuccessfully Subscribe to Operations Topic\r\n"));
					AZURE_PRINTF( ("Disconnect\r\n"));
					MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);
					enAzure_TW_Task = AZURE_TW_IDLE;
				}

				break;

    		case AZURE_TW_SET_STATE:
    			vTaskDelay(pdMS_TO_TICKS(2));
                memset(&(xPublishParameters), 0x00, sizeof(xPublishParameters));
                sprintf(cTopic, AZURE_IOT_MQTT_TWIN_SET_TOPIC, Req_Id );
                Req_Id++;
                xPublishParameters.pucTopic = (const uint8_t *)cTopic;
                xPublishParameters.pvData = Device_Twin_SET_JSON;
                xPublishParameters.usTopicLength = (uint16_t)strlen((const char *)cTopic);
                xPublishParameters.ulDataLength = strlen(cPayload);
                xPublishParameters.xQoS = eMQTTQoS0;

                if( MQTT_AGENT_Publish(xMQTTHandle, &xPublishParameters, pdMS_TO_TICKS(30000)) == eMQTTAgentSuccess )
                {
                	AZURE_PRINTF( ("Successfully Publish to SET Topic\r\n"));
                    enAzure_TW_Task = AZURE_TW_WAIT_SET_RESP;
                }
                else
                {
                	AZURE_PRINTF( ("Unsuccessfully Publish to SET Topic\r\n"));
                	AZURE_PRINTF( ("Disconnect\r\n"));
                	MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);
                	enAzure_TW_Task = AZURE_TW_IDLE;
                }

    			break;

    		case AZURE_TW_WAIT_SET_RESP:

    			if( xEventGroupWaitBits(xCreatedEventGroup, 0x01, pdTRUE, pdFALSE, pdMS_TO_TICKS(30000)) == 0 )
    			{
    				enAzure_TW_Task = AZURE_TELEMETRY_SET_STATE;
    			}
    			else
    			{
    				enAzure_TW_Task = AZURE_TELEMETRY_SET_STATE;
    				AZURE_PRINTF ( ("No response received for AZURE_TW_SET_STATE state\n") );
    				vTaskDelay(pdMS_TO_TICKS(2000));
    			}

    			break;

    		case AZURE_TELEMETRY_SET_STATE:
				vTaskDelay(pdMS_TO_TICKS(2));
				memset(&(xPublishParameters), 0x00, sizeof(xPublishParameters));
				sprintf(cPayload, Device_Telemetry_JSON, aX , aY, aZ, light_intensity, gsm.m.rssi );
//				sprintf(cTopic, AZURE_IOT_TELEMETRY_TOPIC_FOR_PUB, Req_Id );
				Req_Id++;
				xPublishParameters.pucTopic = (const uint8_t *)AZURE_IOT_TELEMETRY_TOPIC_FOR_PUB;
				xPublishParameters.pvData = cPayload;
				xPublishParameters.usTopicLength = (uint16_t)sizeof(AZURE_IOT_TELEMETRY_TOPIC_FOR_PUB) - 1;
				xPublishParameters.ulDataLength = strlen(cPayload);
				xPublishParameters.xQoS = eMQTTQoS0;

				if( MQTT_AGENT_Publish(xMQTTHandle, &xPublishParameters, pdMS_TO_TICKS(30000)) == eMQTTAgentSuccess )
				{
					AZURE_PRINTF( ("Successfully Publish to TELEMETRY Topic\r\n"));
					enAzure_TW_Task = AZURE_TELEMETRY_WAIT_SET_STATE;
				}
				else
				{
					AZURE_PRINTF( ("Unsuccessfully Publish to SET Topic\r\n"));
					AZURE_PRINTF( ("Disconnect\r\n"));
					MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);
					enAzure_TW_Task = AZURE_TW_IDLE;
				}

				break;

			case AZURE_TELEMETRY_WAIT_SET_STATE:

				if( xEventGroupWaitBits(xCreatedEventGroup, 0x01, pdTRUE, pdFALSE, pdMS_TO_TICKS(30000)) == 0 )
				{
					enAzure_TW_Task = AZURE_TW_GET_STATE;
				}
				else
				{
					enAzure_TW_Task = AZURE_TW_GET_STATE;
    				AZURE_PRINTF ( ("No response received for AZURE_TELEMETRY_SET_STATE state\n") );
					vTaskDelay(pdMS_TO_TICKS(2000));
				}

				break;

    		case AZURE_TW_GET_STATE:
    			memset(&(xPublishParameters), 0x00, sizeof(xPublishParameters));
    			sprintf(cPayload, "{ }" );
                sprintf(cTopic, AZURE_IOT_MQTT_TWIN_GET_TOPIC, Req_Id );
                Req_Id++;
                xPublishParameters.pucTopic = (const uint8_t *)cTopic;
                xPublishParameters.pvData = cPayload;
                xPublishParameters.usTopicLength = (uint16_t)sizeof(xPublishParameters.pucTopic) - 1;
                xPublishParameters.ulDataLength = 3;
                xPublishParameters.xQoS = eMQTTQoS0;
                if( MQTT_AGENT_Publish(xMQTTHandle, &xPublishParameters, pdMS_TO_TICKS(30000)) == eMQTTAgentSuccess )
                {
                	AZURE_PRINTF( ("Successfully Publish to GET Topic\r\n"));
                    enAzure_TW_Task = AZURE_TW_WAIT_GET_RESP;
                }
                else
                {
                	AZURE_PRINTF( ("Unsuccessfully Publish to GET Topic\r\n"));
                	AZURE_PRINTF( ("Disconnect\r\n"));
                	MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);
                	enAzure_TW_Task = AZURE_TW_IDLE;
                }

    			break;

    		case AZURE_TW_WAIT_GET_RESP:

    			if( xEventGroupWaitBits(xCreatedEventGroup, 0x01, pdTRUE, pdFALSE, pdMS_TO_TICKS(30000)) == 0 )
    			{
    				enAzure_TW_Task = AZURE_TW_SET_STATE;
    			}
    			else
    			{
    				enAzure_TW_Task = AZURE_TW_SET_STATE;
    				AZURE_PRINTF ( ("No response received for AZURE_TW_GET_STATE state\n") );
    				vTaskDelay(pdMS_TO_TICKS(2000));
    			}
    			break;

    		default:
    	    	AZURE_PRINTF( ( "Invalid Application State!! \r\n" ) );
    	    	AZURE_PRINTF( ( "Closing Azure Demo\r\n" ) );
    	    	vTaskDelete(NULL);
    			break;

    	}
    }

}

void vStartAzureLedDemoTask( void )
{
    ( void ) xTaskCreate( prvmcsft_Azure_TwinTask,
                          "Microsoft Azure Twin Task",
						  AzureTwin_DemoUPDATE_TASK_STACK_SIZE,
                          NULL,
                          tskIDLE_PRIORITY,
                          NULL );
}

#endif /* MSFT_AZURE_C_ */
