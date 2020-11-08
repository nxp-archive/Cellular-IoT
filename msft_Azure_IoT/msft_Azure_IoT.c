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

/* Maximum amount of time a function call may block. */
#define AzureTwinDemoTIMEOUT                    pdMS_TO_TICKS( 30000UL )

#define Device_Property_JSON 					\
	"{"											\
		"\"manufacturer\": \"%s\", "			\
		"\"model\": \"%s\", "					\
		"\"swVersion\": \"%s\", "				\
		"\"osName\": \"%s\", "					\
		"\"processorArchitecture\": \"%s\", "	\
		"\"processorManufacturer\": \"%s\", "	\
		"\"totalStorage\": %d, "				\
		"\"totalMemory\": %d"					\
	"}"

char manufacturer[] = {"Avnet"};
char model[] = {"Monarch LTE-M Dev Kit"};
char swVersion[] = {"v1.0"};
char osName[] = {"FreeRTOS"};
char processorArchitecture[] = {"LPC55S69"};
char processorManufacturer[] = {"NXP Semiconductor"};
uint32_t totalStorage = 640;
uint32_t totalMemory = 960;

#define Device_Cellular_Telemetry_JSON 			\
	"{"											\
		"\"mcc\": %d, "	 						\
		"\"mnc\": %d, "	 						\
		"\"lac\": %d, "	 						\
		"\"cid\": %d, " 						\
		"\"rssi\": %d, "	 					\
		"\"iccid\": \"%s\", "					\
		"\"imei\": \"%s\", " 					\
		"\"modem_fw\": \"%s\", " 				\
		"\"device_id\": \"%s\"" 				\
	"}"

uint32_t mcc = 208;
uint32_t mnc = 01;
uint32_t lac = 0;
uint32_t cid = 4;
//double rssi = -63.4;
char iccid[] = {"89148000005471125146"};
char imei[] = {"354658090355378"};
char modem_fw[] = {"UE5.2.0.1"};
char device_id[] = {"19494031513"};


#define Device_Sensor_Telemetry_JSON 		\
	"{"										\
		"\"aX\": %.2f, "	 				\
		"\"aY\": %.2f, "	 				\
		"\"aZ\": %.2f, "	 				\
		"\"light_sensor\": %.2f, " 			\
		"\"button\": %d, "					\
		"\"current\": %.2f"					\
	"}"

double aX = -3.2;
double aY = 50.7;
double aZ = -89.5;
double light_sensor = 79.1;
bool button = false;
double current =  100.05;

#define Device_Location_Telemetry_JSON 		\
	"{"										\
		"\"Location\":"						\
		"{"									\
			"\"value\":"					\
			"{"								\
				"\"lat\": %.6f, " 			\
				"\"lon\": %.6f, " 			\
				"\"alt\": %.6f" 			\
			"}"								\
		"}" 								\
	"}"

#define Device_Control_Property_JSON 		\
	"{"										\
		"\"tx_interval\": \"PT%dH%dM%dS\""	\
	"}"

double lat = 49.187104;
double lon = -0.308766;
double alt = 0;


typedef enum AZURE_TWIN_TASK_ST
{
	AZURE_SM_CONNECT_TO_DPS,
	AZURE_SM_SUB_DPSR,						/* Subscription to DPS Registration topic */
	AZURE_SM_PUB_DPSR,						/* Publish to DPS Registration topic */
	AZURE_SM_WAIT_PUB_DPSR_RESP,			/* Wait for DPS Registration response */
	AZURE_SM_PUB_GOS,						/* Publish to poll for registration operation status topic */
	AZURE_SM_WAIT_PUB_GOS_RESP,				/* Wait for registration operation status response */
	AZURE_SM_GEN_IOTC_CREDENTIALS,
	AZURE_SM_CONNECT_TO_ASSIGNED_HUB,
	AZURE_SM_SUB_C2DM,						/* Subscription to cloud-to-device messages topic */
	AZURE_SM_SUB_DTWR,						/* Subscription to device twin's response topic */
	AZURE_SM_SUB_DM,						/* Subscription to direct method topic */
	AZURE_SM_SUB_DT,						/* Subscription to device telemetry topic */
	AZURE_SM_SUB_DTWPC,						/* Subscription to device twin's property changes topic */
	AZURE_SM_PUB_GET_TW_PROPERTIES,
	AZURE_SM_WAIT_GET_TW_PROPERTIES_RESP,
	AZURE_SM_PUB_SET_TW_PROPERTIES,
	AZURE_SM_WAIT_SET_TW_PROPERTIES_RESP,
	AZURE_SM_PUB_SET_CONTROL_PROPERTIES,
	AZURE_SM_WAIT_SET_CONTROL_PROPERTIES,
	AZURE_SM_PUB_SENSOR_TELEMETRY,
	AZURE_SM_PUB_LOC_TELEMETRY,
	AZURE_SM_PUB_CELLULAR_TELEMETRY,
	AZURE_SM_IDLE,
	AZURE_SM_STATES_BNDRY
}Azure_SM_Task;

static MQTTAgentConnectParams_t xConnectParams;
const MQTTAgentConnectParams_t * pConnectParams = &xConnectParams;
MQTTAgentHandle_t xMQTTHandle;

static Azure_SM_Task eAzure_SM_Task;
static Azure_SM_Task eNext_Azure_State;
static MQTTAgentSubscribeParams_t xSubscribeParams;
//static MQTTAgentUnsubscribeParams_t xUnsubscribeParams;
static MQTTAgentPublishParams_t xPublishParameters;
EventGroupHandle_t xCreatedEventGroup;
#define EVENT_BIT_MASK	0x01

char* sas_token;
char* assigned_hub;
char* username;
char* password;
char* operation_id;


void deviceRegistrationCallback(char * propertyName, char * payload, size_t payload_len)
{
	jsobject_t object;
	jsobject_initialize(&object, payload, payload_len);

	if (strcmp(propertyName, "operationId") == 0)
	{
		if (!operation_id)
		{
			char* v = jsobject_get_data_by_name(&object, "operationId");
			if (v)
			{
//				operation_id = (char *)AZURE_IOTC_MALLOC(strlen(v) - 2);
//				memcpy(operation_id, (char *)v + 1, strlen(v) - 2);
//				operation_id[strlen(v) - 2] = 0;
//				AZURE_IOTC_FREE(v);
				operation_id = v;
				AZURE_PRINTF(("==> Received an operationId! Value => %s\n", operation_id));
			}
		}
	}
	if (strcmp(propertyName, "assignedHub") == 0)
	{
		if (!assigned_hub)
		{
			char* v = jsobject_get_data_by_name(&object, "assignedHub");
			if (v)
			{
				assigned_hub = (char *)AZURE_IOTC_MALLOC(strlen(v) - 2);
				memcpy(assigned_hub, (char *)v + 1, strlen(v) - 2);
				assigned_hub[strlen(v) - 2] = 0;
				AZURE_IOTC_FREE(v);
				AZURE_PRINTF(("==> Received an assignedHub! Value => %s\n", assigned_hub));
			}
		}
	}
	else if (strcmp(propertyName, "status") == 0)
	{
		char* v = jsobject_get_data_by_name(&object, "status");
		if (v)
		{
			AZURE_PRINTF(("==> status value => %s\n", v));
			AZURE_IOTC_FREE(v);
		}
	}
	else if (strcmp(propertyName, "errorCode") == 0)
	{
		int v = jsobject_get_number_by_name(&object, "errorCode");
		AZURE_PRINTF(("==> Received an error Code! Value => %d\n", v));
	}
	else if (strcmp(propertyName, "message") == 0)
	{
		char* v = jsobject_get_data_by_name(&object, "message");
		if (v)
		{
			AZURE_PRINTF(("==> message value => %s", v));
			AZURE_IOTC_FREE(v);
		}
	}
	else
	{
		char* v = jsobject_get_data_by_name(&object, propertyName);
		if (v)
		{
			AZURE_PRINTF(("==> %s: %s\n", propertyName, v));
			AZURE_IOTC_FREE(v);
		}
	}
	jsobject_free(&object);
}

void deviceTwinGetCallback(char * propertyName, char * payload, size_t payload_len)
{
	jsobject_t object;
	jsobject_initialize(&object, payload, payload_len);

	if (strcmp(propertyName, "rgb_red") == 0)
	{
		char* v = jsobject_get_data_by_name(&object, "rgb_red");
		if(v)
		{
			AZURE_PRINTF(("==> Received a 'RED LED' update! New Value => %s\n", v));
		}
	}
	else if (strcmp(propertyName, "rgb_green") == 0)
	{
		char* v = jsobject_get_data_by_name(&object, "rgb_green");
		if(v)
		{
			AZURE_PRINTF(("==> Received a 'GREEN LED' update! New Value => %s\n", v));
		}
	}
	else if (strcmp(propertyName, "rgb_blue") == 0)
	{
		char* v = jsobject_get_data_by_name(&object, "rgb_blue");
		if(v)
		{
			AZURE_PRINTF(("==> Received a 'BLUE LED' update! New Value => %s\n", v));
		}
	}
	else
	{
		char* v = jsobject_get_data_by_name(&object, propertyName);
		if (v)
		{
			AZURE_PRINTF(("==> %s: %s\n", propertyName, v));
			AZURE_IOTC_FREE(v);
		}
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

	AZURE_PRINTF( ( "Azure_IoT_CallBack received topic: %s\r\n", topic ) );

	if (topic_length == 0)
	{
		AZURE_PRINTF( ( "ERROR: Azure_IoT_CallBack without a topic.\r\n" ) );
		return eMQTTFalse;
	}

	if (topic_check("$dps/registrations/res/",
					strlen("$dps/registrations/res/"),
					topic,
					topic_length
					)
		)
	{
		/* Registration event received */
		AZURE_PRINTF(("Received a Registration event\n"));
		jsobject_t received;
		jsobject_initialize(&received, msg, msg_length);

		for (unsigned i = 0, count = jsobject_get_count(&received); i < count; i +=2)
		{
			char *itemName = jsobject_get_string_at(&received, i);
			if (itemName != NULL && itemName[0] != '$')
			{
				deviceRegistrationCallback(itemName, msg, msg_length);
			}
			if (itemName) AZURE_IOTC_FREE(itemName);
		}
		jsobject_free(&received);
	}

	else if (topic_check("$iothub/twin/PATCH/properties/desired/",
						 strlen("$iothub/twin/PATCH/properties/desired/"),
						 topic,
						 topic_length
						)
		     )
	{
		/* Device Twin Get received */
		AZURE_PRINTF(("Received a SettingsUpdated event\n"));
		jsobject_t desired;
		jsobject_initialize(&desired, msg, msg_length);

		for (unsigned i = 0, count = jsobject_get_count(&desired); i < count; i +=2 )
		{
			char *itemName = jsobject_get_string_at(&desired, i);
			if (itemName != NULL && itemName[0] != '$')
			{
				deviceTwinGetCallback(itemName, msg, msg_length);
			}
			if (itemName) AZURE_IOTC_FREE(itemName);
		}
		jsobject_free(&desired);
	}


	xEventGroupSetBits(xCreatedEventGroup, EVENT_BIT_MASK);
	MQTT_AGENT_ReturnBuffer(( MQTTAgentHandle_t) 2, pxPublishData->xBuffer);

	return eMQTTTrue;
}

void prvmcsft_Azure_TwinTask( void * pvParameters )
{
	MQTTAgentReturnCode_t xMQTTReturn;
	char cPayload[256];
	char cTopic[256];
	uint8_t Req_Id =1;

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

#ifdef SAS_KEY
    generateSasToken(&sas_token,
    				 clientcredentialAZURE_IOT_SCOPE_ID, strlen(clientcredentialAZURE_IOT_SCOPE_ID),
					 clientcredentialAZURE_IOT_DEVICE_ID, strlen(clientcredentialAZURE_IOT_DEVICE_ID),
					 keyDEVICE_SAS_PRIMARY_KEY, strlen(keyDEVICE_SAS_PRIMARY_KEY));
#endif

    memset( &xConnectParams, 0x00, sizeof( xConnectParams ) );
    xConnectParams.pcURL = clientcredentialAZURE_MQTT_BROKER_ENDPOINT;
    xConnectParams.usPort = clientcredentialAZURE_MQTT_BROKER_PORT;

    xConnectParams.xFlags = mqttagentREQUIRE_TLS;
    xConnectParams.pcCertificate = (char *)AZURE_SERVER_ROOT_CERTIFICATE_PEM;
    xConnectParams.ulCertificateSize = sizeof(AZURE_SERVER_ROOT_CERTIFICATE_PEM);
    xConnectParams.pxCallback = NULL;
    xConnectParams.pvUserData = NULL;

    xConnectParams.pucClientId = (const uint8_t *)(clientcredentialAZURE_IOT_DEVICE_ID);
    xConnectParams.usClientIdLength = (uint16_t)strlen(clientcredentialAZURE_IOT_DEVICE_ID);
#if SSS_USE_FTR_FILE
    xConnectParams.cUserName = clientcredentialAZURE_IOT_MQTT_USERNAME;
    xConnectParams.uUsernamelength = ( uint16_t ) strlen(clientcredentialAZURE_IOT_MQTT_USERNAME);
#ifdef SAS_KEY
    xConnectParams.p_password = sas_token ;
    xConnectParams.passwordlength = ( uint16_t ) strlen(sas_token);
#else
    xConnectParams.p_password = NULL;
    xConnectParams.passwordlength = 0;
#endif
#endif

    eAzure_SM_Task = AZURE_SM_CONNECT_TO_DPS;

    xCreatedEventGroup = xEventGroupCreate();

    while( 1 )
    {
    	switch( eAzure_SM_Task )
    	{
    		case AZURE_SM_CONNECT_TO_DPS:

    		    xMQTTReturn = MQTT_AGENT_Connect( xMQTTHandle,
    		    								  pConnectParams,
    											  AzureTwinDemoTIMEOUT);

    		    if( eMQTTAgentSuccess == xMQTTReturn )
    		    {
    		    	AZURE_PRINTF( ("Connected to DPS Hub Successfully\r\n") );
    		    	eAzure_SM_Task = AZURE_SM_SUB_DPSR;
    		    }
    		    else
    		    {
    		    	AZURE_PRINTF( ("Connection refused!! \r\n") );
    		    	vTaskDelete(NULL);
    		    }
    		    break;

    		case AZURE_SM_SUB_DPSR:
    			xSubscribeParams.pucTopic = (const uint8_t *)AZURE_IOT_DPS_REGISTRATION_TOPIC_FOR_SUB;
				xSubscribeParams.pvPublishCallbackContext = NULL;
				xSubscribeParams.pxPublishCallback = Azure_IoT_CallBack;
				xSubscribeParams.usTopicLength = strlen(AZURE_IOT_DPS_REGISTRATION_TOPIC_FOR_SUB);
				xSubscribeParams.xQoS = eMQTTQoS0;

				if( MQTT_AGENT_Subscribe(xMQTTHandle, &xSubscribeParams, AzureTwinDemoTIMEOUT) == eMQTTAgentSuccess)
				{
					AZURE_PRINTF( ("Successfully Subscribe to DPS Registration Topic\r\n") );
					eAzure_SM_Task = AZURE_SM_PUB_DPSR;
				}
				else
				{
					AZURE_PRINTF( ("Unsuccessfully Subscribe to DPS Registration Topic\r\n"));
					AZURE_PRINTF( ("Disconnect\r\n"));

					MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);

					eAzure_SM_Task = AZURE_SM_STATES_BNDRY;
				}
				break;

    		case AZURE_SM_PUB_DPSR:
    			vTaskDelay(pdMS_TO_TICKS(1000));

				memset(&(xPublishParameters), 0x00, sizeof(xPublishParameters));
				memset(cTopic, 0, sizeof(cTopic));
				memset(cPayload, 0, sizeof(cPayload));

				sprintf(cTopic, AZURE_IOT_DPS_REGISTRATION_TOPIC_FOR_PUB, Req_Id++ );
				sprintf(cPayload, "{\"registrationId\":\"%s\"}", clientcredentialAZURE_IOT_DEVICE_ID);

				xPublishParameters.pucTopic = (const uint8_t *)cTopic;
				xPublishParameters.pvData = cPayload;
				xPublishParameters.usTopicLength = (uint16_t)strlen((const char *)cTopic);
				xPublishParameters.ulDataLength = strlen(cPayload);
				xPublishParameters.xQoS = eMQTTQoS0;

				if( MQTT_AGENT_Publish(xMQTTHandle, &xPublishParameters, AzureTwinDemoTIMEOUT) == eMQTTAgentSuccess )
				{
					AZURE_PRINTF( ("Successfully Publish to DPS Registration Topic\r\n"));
					eAzure_SM_Task = AZURE_SM_WAIT_PUB_DPSR_RESP;
				}
				else
				{
					AZURE_PRINTF( ("Unsuccessfully Publish to DPS Registration Topic\r\n"));
					AZURE_PRINTF( ("Disconnect\r\n"));
					MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);
					eAzure_SM_Task = AZURE_SM_STATES_BNDRY;
				}

				break;

			case AZURE_SM_WAIT_PUB_DPSR_RESP:

				if( xEventGroupWaitBits(xCreatedEventGroup, EVENT_BIT_MASK, pdTRUE, pdFALSE, AzureTwinDemoTIMEOUT) == EVENT_BIT_MASK )
				{
					eAzure_SM_Task = AZURE_SM_PUB_GOS;
				}
				else
				{
					eAzure_SM_Task = AZURE_SM_STATES_BNDRY;
    				AZURE_PRINTF ( ("No response received for AZURE_SM_PUB_DPSR state\n") );
					vTaskDelay(pdMS_TO_TICKS(2000));
				}

				break;

    		case AZURE_SM_PUB_GOS:
    			/* Place a longer delay here to leave the time
    			 * to the DPS Registration broken to change the state
    			 * of the request from the previous Publish
    			 */
    			vTaskDelay(pdMS_TO_TICKS(1000));

				memset(cTopic, 0, sizeof(cTopic));
				memset(cPayload, 0, sizeof(cPayload));

				/* Removing " characters at start & end of the operation_id string */
				char* tmp = operation_id;
				tmp++;
				memcpy(cPayload, tmp, strlen(tmp));
				cPayload[strlen(tmp)] = 0;
				cPayload[strlen(tmp) - 1] = 0;
				sprintf(cTopic, AZURE_IOT_DPS_GET_REGISTRATION_OPERATION_STATUS_TOPIC_FOR_PUB, Req_Id++, cPayload );

				memset(&(xPublishParameters), 0x00, sizeof(xPublishParameters));
				xPublishParameters.pucTopic = (const uint8_t *)cTopic;
				xPublishParameters.pvData = NULL;
				xPublishParameters.usTopicLength = (uint16_t)strlen((const char *)cTopic);
				xPublishParameters.ulDataLength = 0U;
				xPublishParameters.xQoS = eMQTTQoS0;

				if( MQTT_AGENT_Publish(xMQTTHandle, &xPublishParameters, AzureTwinDemoTIMEOUT) == eMQTTAgentSuccess )
				{
					AZURE_PRINTF( ("Successfully Publish to Get Operation Status Topic\r\n"));
					eAzure_SM_Task = AZURE_SM_WAIT_PUB_GOS_RESP;
				}
				else
				{
					AZURE_PRINTF( ("Unsuccessfully Publish to Get Operation Status Topic\r\n"));
					AZURE_PRINTF( ("Disconnect\r\n"));
					MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);
					eAzure_SM_Task = AZURE_SM_STATES_BNDRY;
				}

				break;

			case AZURE_SM_WAIT_PUB_GOS_RESP:

				if( xEventGroupWaitBits(xCreatedEventGroup, EVENT_BIT_MASK, pdTRUE, pdFALSE, AzureTwinDemoTIMEOUT) == EVENT_BIT_MASK )
				{
					eAzure_SM_Task = AZURE_SM_GEN_IOTC_CREDENTIALS;
				}
				else
				{
					eAzure_SM_Task = AZURE_SM_STATES_BNDRY;
    				AZURE_PRINTF ( ("No response received for AZURE_SM_PUB_GOS state\n") );
					vTaskDelay(pdMS_TO_TICKS(2000));
				}

				break;

			case AZURE_SM_GEN_IOTC_CREDENTIALS:
#ifdef SAS_KEY
				getUsernameAndPassword(&username, &password,
									   clientcredentialAZURE_IOT_DEVICE_ID, strlen(clientcredentialAZURE_IOT_DEVICE_ID),
									   assigned_hub, strlen(assigned_hub),
									   keyDEVICE_SAS_PRIMARY_KEY, strlen(keyDEVICE_SAS_PRIMARY_KEY));
#endif

				if( !(strlen(username) == 0U && strlen(password) == 0U) )
				{
					eAzure_SM_Task = AZURE_SM_CONNECT_TO_ASSIGNED_HUB;
				}
				else
				{
					eAzure_SM_Task = AZURE_SM_STATES_BNDRY;
    				AZURE_PRINTF ( ("Not able to generate username and password for AZURE_SM_GEN_IOTC_CREDENTIALS state\n") );
					vTaskDelay(pdMS_TO_TICKS(2000));
				}

				break;

			case AZURE_SM_CONNECT_TO_ASSIGNED_HUB:

				/* Disconnect to the generic DPS hub */
				MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);

				if( MQTT_AGENT_Delete( xMQTTHandle ) != eMQTTAgentSuccess )
				{
					configPRINTF(("Failed to delete the MQTT Handle, stopping demo.\r\n"));
					vTaskDelete(NULL);
				}

				if( MQTT_AGENT_Create( &xMQTTHandle ) != eMQTTAgentSuccess )
				{
					configPRINTF(("Failed to initialize the MQTT Handle, stopping demo.\r\n"));
					vTaskDelete(NULL);
				}

			    memset( &xConnectParams, 0x00, sizeof( xConnectParams ) );
			    xConnectParams.pcURL = assigned_hub;
			    xConnectParams.usPort = clientcredentialAZURE_MQTT_BROKER_PORT;

			    xConnectParams.xFlags = mqttagentREQUIRE_TLS;
			    xConnectParams.pcCertificate = (char *)AZURE_SERVER_ROOT_CERTIFICATE_PEM;
			    xConnectParams.ulCertificateSize = sizeof(AZURE_SERVER_ROOT_CERTIFICATE_PEM);
			    xConnectParams.pxCallback = NULL;
			    xConnectParams.pvUserData = NULL;

			    xConnectParams.pucClientId = (const uint8_t *)(clientcredentialAZURE_IOT_DEVICE_ID);
			    xConnectParams.usClientIdLength = (uint16_t)strlen(clientcredentialAZURE_IOT_DEVICE_ID);
			#if SSS_USE_FTR_FILE
			    xConnectParams.cUserName = username;
			    xConnectParams.uUsernamelength = ( uint16_t ) strlen(username);
			#ifdef SAS_KEY
			    xConnectParams.p_password = password;
			    xConnectParams.passwordlength = ( uint16_t ) strlen(password);
			#else
			    xConnectParams.p_password = NULL;
			    xConnectParams.passwordlength = 0;
			#endif
			#endif

				xMQTTReturn = MQTT_AGENT_Connect( xMQTTHandle,
												  pConnectParams,
												  AzureTwinDemoTIMEOUT);

				if( eMQTTAgentSuccess == xMQTTReturn )
				{
					AZURE_PRINTF( ("Connected to Azure IoT Central Successfully\r\n") );
					eAzure_SM_Task = AZURE_SM_SUB_C2DM;
				}
				else
				{
					AZURE_PRINTF( ("Connection refused!! \r\n") );
					eAzure_SM_Task = AZURE_SM_STATES_BNDRY;
				}
				break;

    		case AZURE_SM_SUB_C2DM:
    			vTaskDelay(pdMS_TO_TICKS(1000));

    			memset(cTopic, 0, sizeof(cTopic));

				sprintf(cTopic, AZURE_IOT_C2D_TOPIC_FOR_SUB, clientcredentialAZURE_IOT_DEVICE_ID);

    			xSubscribeParams.pucTopic = (const uint8_t *)cTopic;
				xSubscribeParams.pvPublishCallbackContext = NULL;
				xSubscribeParams.pxPublishCallback = Azure_IoT_CallBack;
				xSubscribeParams.usTopicLength = strlen(cTopic);
				xSubscribeParams.xQoS = eMQTTQoS0;

				if( MQTT_AGENT_Subscribe(xMQTTHandle, &xSubscribeParams, AzureTwinDemoTIMEOUT) == eMQTTAgentSuccess)
				{
					AZURE_PRINTF( ("Successfully Subscribe to Cloud-to-Device Topic\r\n") );
					eAzure_SM_Task = AZURE_SM_SUB_DTWR;
				}
				else
				{
					AZURE_PRINTF( ("Unsuccessfully Subscribe to Cloud-to-Device Topic\r\n"));
					AZURE_PRINTF( ("Disconnect\r\n"));

					MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);

					eAzure_SM_Task = AZURE_SM_STATES_BNDRY;
				}
				break;

    		case AZURE_SM_SUB_DTWR:
    			vTaskDelay(pdMS_TO_TICKS(1000));

    		    xSubscribeParams.pucTopic = (const uint8_t *)AZURE_IOT_TWIN_RESPONSE_TOPIC_FOR_SUB;
    		    xSubscribeParams.pvPublishCallbackContext = NULL;
    		    xSubscribeParams.pxPublishCallback = Azure_IoT_CallBack;
    		    xSubscribeParams.usTopicLength = strlen(AZURE_IOT_TWIN_RESPONSE_TOPIC_FOR_SUB);
    		    xSubscribeParams.xQoS = eMQTTQoS0;

    		    if( MQTT_AGENT_Subscribe(xMQTTHandle, &xSubscribeParams, AzureTwinDemoTIMEOUT) == eMQTTAgentSuccess)
    		    {
    		    	AZURE_PRINTF( ("Successfully Subscribe to Device Twin Response Topic\r\n") );
    		    	eAzure_SM_Task = AZURE_SM_SUB_DM;
    		    }
                else
                {
                	AZURE_PRINTF( ("Unsuccessfully Subscribe to Device Twin Response Topic\r\n"));
                	AZURE_PRINTF( ("Disconnect\r\n"));

                	MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);

                	eAzure_SM_Task = AZURE_SM_STATES_BNDRY;
                }

    			break;

    		case AZURE_SM_SUB_DM:
    			vTaskDelay(pdMS_TO_TICKS(2));

				xSubscribeParams.pucTopic = (const uint8_t *)AZURE_IOT_METHOD_TOPIC_FOR_SUB;
				xSubscribeParams.pvPublishCallbackContext = NULL;
				xSubscribeParams.pxPublishCallback = Azure_IoT_CallBack;
				xSubscribeParams.usTopicLength = strlen(AZURE_IOT_METHOD_TOPIC_FOR_SUB);
				xSubscribeParams.xQoS = eMQTTQoS0;

				if( MQTT_AGENT_Subscribe(xMQTTHandle, &xSubscribeParams, AzureTwinDemoTIMEOUT) == eMQTTAgentSuccess)
				{
					AZURE_PRINTF( ("Successfully Subscribe to Device Method Topic\r\n") );
					eAzure_SM_Task = AZURE_SM_SUB_DT;
				}
				else
				{
					AZURE_PRINTF( ("Unsuccessfully Subscribe to Device Method Topic\r\n"));
					AZURE_PRINTF( ("Disconnect\r\n"));

					MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);

					eAzure_SM_Task = AZURE_SM_STATES_BNDRY;
				}

				break;

    		case AZURE_SM_SUB_DT:
    			vTaskDelay(pdMS_TO_TICKS(1000));

    			memset(cTopic, 0, sizeof(cTopic));

				sprintf(cTopic, AZURE_IOT_TELEMETRY_TOPIC_FOR_SUB, clientcredentialAZURE_IOT_DEVICE_ID);

    			xSubscribeParams.pucTopic = (const uint8_t *)cTopic;
				xSubscribeParams.pvPublishCallbackContext = NULL;
				xSubscribeParams.pxPublishCallback = Azure_IoT_CallBack;
				xSubscribeParams.usTopicLength = strlen(cTopic);
				xSubscribeParams.xQoS = eMQTTQoS0;

				if( MQTT_AGENT_Subscribe(xMQTTHandle, &xSubscribeParams, AzureTwinDemoTIMEOUT) == eMQTTAgentSuccess)
				{
					AZURE_PRINTF( ("Successfully Subscribe to Device Telemetry Topic\r\n") );
					eAzure_SM_Task = AZURE_SM_SUB_DTWPC;
				}
				else
				{
					AZURE_PRINTF( ("Unsuccessfully Subscribe to Device Telemetry Topic\r\n"));
					AZURE_PRINTF( ("Disconnect\r\n"));

					MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);
					eAzure_SM_Task = AZURE_SM_STATES_BNDRY;
				}

				break;

			case AZURE_SM_SUB_DTWPC:
				vTaskDelay(pdMS_TO_TICKS(1000));

				xSubscribeParams.pucTopic = (const uint8_t *)AZURE_IOT_TWIN_PATCH_TOPIC_FOR_SUB;
				xSubscribeParams.pvPublishCallbackContext = NULL;
				xSubscribeParams.pxPublishCallback = Azure_IoT_CallBack;
				xSubscribeParams.usTopicLength = strlen(AZURE_IOT_TWIN_PATCH_TOPIC_FOR_SUB);
				xSubscribeParams.xQoS = eMQTTQoS0;

				if( MQTT_AGENT_Subscribe(xMQTTHandle, &xSubscribeParams, AzureTwinDemoTIMEOUT) == eMQTTAgentSuccess)
				{
					AZURE_PRINTF( ("Successfully Subscribe to Device Twin Patch Topic\r\n") );
					eAzure_SM_Task = AZURE_SM_PUB_GET_TW_PROPERTIES;
				}
				else
				{
					AZURE_PRINTF( ("Unsuccessfully Subscribe to Device Twin Patch Topic\r\n"));
					AZURE_PRINTF( ("Disconnect\r\n"));

					MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);

					eAzure_SM_Task = AZURE_SM_STATES_BNDRY;
				}

				break;

    		case AZURE_SM_PUB_GET_TW_PROPERTIES:
    			vTaskDelay(pdMS_TO_TICKS(1000));

    			memset(&(xPublishParameters), 0x00, sizeof(xPublishParameters));
                memset(cTopic, 0, sizeof(cTopic));

                sprintf(cTopic, AZURE_IOT_MQTT_TWIN_GET_TOPIC, Req_Id++ );

                xPublishParameters.pucTopic = (const uint8_t *)cTopic;
                xPublishParameters.pvData = NULL;
                xPublishParameters.usTopicLength = (uint16_t)strlen(cTopic);
                xPublishParameters.ulDataLength = 0U;
                xPublishParameters.xQoS = eMQTTQoS0;
                if( MQTT_AGENT_Publish(xMQTTHandle, &xPublishParameters, AzureTwinDemoTIMEOUT) == eMQTTAgentSuccess )
                {
                	AZURE_PRINTF( ("Successfully Publish to GET Device Twin Properties Topic\r\n"));
                    eAzure_SM_Task = AZURE_SM_WAIT_GET_TW_PROPERTIES_RESP;
                }
                else
                {
                	AZURE_PRINTF( ("Unsuccessfully Publish to GET Device Twin Properties Topic\r\n"));
                	AZURE_PRINTF( ("Disconnect\r\n"));
                	MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);
                	eAzure_SM_Task = AZURE_SM_STATES_BNDRY;
                }

    			break;

    		case AZURE_SM_WAIT_GET_TW_PROPERTIES_RESP:

    			if( xEventGroupWaitBits(xCreatedEventGroup, EVENT_BIT_MASK, pdTRUE, pdFALSE, AzureTwinDemoTIMEOUT) == EVENT_BIT_MASK )
    			{
    				eAzure_SM_Task = AZURE_SM_PUB_SET_TW_PROPERTIES;
    			}
    			else
    			{
    				eAzure_SM_Task = AZURE_SM_PUB_SET_TW_PROPERTIES;
    				AZURE_PRINTF ( ("No response received for AZURE_SM_PUB_GET_TW_PROPERTIES state\n") );
    				vTaskDelay(pdMS_TO_TICKS(2000));
    			}
    			break;

    		case AZURE_SM_PUB_SET_TW_PROPERTIES:
    			vTaskDelay(pdMS_TO_TICKS(1000));

                memset(&(xPublishParameters), 0x00, sizeof(xPublishParameters));
                memset(cTopic, 0, sizeof(cTopic));
                memset(cPayload, 0, sizeof(cPayload));

                sprintf(cTopic, AZURE_IOT_MQTT_TWIN_SET_TOPIC, Req_Id++ );
                sprintf(cPayload, Device_Property_JSON, manufacturer , model, swVersion, osName, processorArchitecture, processorManufacturer, totalStorage, totalMemory);

                xPublishParameters.pucTopic = (const uint8_t *)cTopic;
                xPublishParameters.pvData = cPayload;
                xPublishParameters.usTopicLength = (uint16_t)strlen((const char *)cTopic);
                xPublishParameters.ulDataLength = strlen((const char *)cPayload);
                xPublishParameters.xQoS = eMQTTQoS0;

                if( MQTT_AGENT_Publish(xMQTTHandle, &xPublishParameters, AzureTwinDemoTIMEOUT) == eMQTTAgentSuccess )
                {
                	AZURE_PRINTF( ("Successfully Publish to Device Twin Properties Topic\r\n"));
                    eAzure_SM_Task = AZURE_SM_WAIT_SET_TW_PROPERTIES_RESP;
                }
                else
                {
                	AZURE_PRINTF( ("Unsuccessfully Publish to Device Twin Properties Topic\r\n"));
                	AZURE_PRINTF( ("Disconnect\r\n"));
                	MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);
                	eAzure_SM_Task = AZURE_SM_STATES_BNDRY;
                }

    			break;

    		case AZURE_SM_WAIT_SET_TW_PROPERTIES_RESP:

    			if( xEventGroupWaitBits(xCreatedEventGroup, EVENT_BIT_MASK, pdTRUE, pdFALSE, AzureTwinDemoTIMEOUT) == EVENT_BIT_MASK )
    			{
    				eAzure_SM_Task = AZURE_SM_PUB_SET_CONTROL_PROPERTIES;
    			}
    			else
    			{
    				eAzure_SM_Task = AZURE_SM_PUB_SET_CONTROL_PROPERTIES;
    				AZURE_PRINTF ( ("No response received for AZURE_SM_PUB_SET_TW_PROPERTIES state\n") );
    				vTaskDelay(pdMS_TO_TICKS(2000));
    			}

    			break;

    		case AZURE_SM_PUB_SET_CONTROL_PROPERTIES:
    			vTaskDelay(pdMS_TO_TICKS(1000));

                memset(&(xPublishParameters), 0x00, sizeof(xPublishParameters));
                memset(cTopic, 0, sizeof(cTopic));
                memset(cPayload, 0, sizeof(cPayload));

                sprintf(cTopic, AZURE_IOT_MQTT_TWIN_SET_TOPIC, Req_Id++ );
                sprintf(cPayload, Device_Control_Property_JSON, 0U , 1U, 0U);

                xPublishParameters.pucTopic = (const uint8_t *)cTopic;
                xPublishParameters.pvData = cPayload;
                xPublishParameters.usTopicLength = (uint16_t)strlen((const char *)cTopic);
                xPublishParameters.ulDataLength = strlen((const char *)cPayload);
                xPublishParameters.xQoS = eMQTTQoS0;

                if( MQTT_AGENT_Publish(xMQTTHandle, &xPublishParameters, AzureTwinDemoTIMEOUT) == eMQTTAgentSuccess )
                {
                	AZURE_PRINTF( ("Successfully Publish to Device Twin Properties Topic\r\n"));
                    eAzure_SM_Task = AZURE_SM_WAIT_SET_CONTROL_PROPERTIES;
                }
                else
                {
                	AZURE_PRINTF( ("Unsuccessfully Publish to Device Twin Properties Topic\r\n"));
                	AZURE_PRINTF( ("Disconnect\r\n"));
                	MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);
                	eAzure_SM_Task = AZURE_SM_STATES_BNDRY;
                }

				break;

    		case AZURE_SM_WAIT_SET_CONTROL_PROPERTIES:

    			if( xEventGroupWaitBits(xCreatedEventGroup, EVENT_BIT_MASK, pdTRUE, pdFALSE, AzureTwinDemoTIMEOUT) == EVENT_BIT_MASK )
    			{
    				eAzure_SM_Task = AZURE_SM_PUB_SENSOR_TELEMETRY;
    			}
    			else
    			{
    				eAzure_SM_Task = AZURE_SM_PUB_SENSOR_TELEMETRY;
    				AZURE_PRINTF ( ("No response received for AZURE_SM_PUB_SET_TW_PROPERTIES state\n") );
    				vTaskDelay(pdMS_TO_TICKS(2000));
    			}

    			break;

    		case AZURE_SM_PUB_SENSOR_TELEMETRY:
				vTaskDelay(pdMS_TO_TICKS(1000));

				memset(&(xPublishParameters), 0x00, sizeof(xPublishParameters));
				memset(cTopic, 0, sizeof(cTopic));
				memset(cPayload, 0, sizeof(cPayload));

                sprintf(cTopic, AZURE_IOT_TELEMETRY_TOPIC_FOR_PUB, clientcredentialAZURE_IOT_DEVICE_ID);
				sprintf(cPayload, Device_Sensor_Telemetry_JSON, aX , aY, aZ, light_sensor, button, current);

				xPublishParameters.pucTopic = (const uint8_t *)cTopic;
				xPublishParameters.pvData = cPayload;
				xPublishParameters.usTopicLength = (uint16_t)strlen(cTopic);
				xPublishParameters.ulDataLength = strlen(cPayload);
				xPublishParameters.xQoS = eMQTTQoS0;

				if( MQTT_AGENT_Publish(xMQTTHandle, &xPublishParameters, AzureTwinDemoTIMEOUT) == eMQTTAgentSuccess )
				{
					AZURE_PRINTF( ("Successfully Publish to SENSOR_TELEMETRY Topic\r\n"));
					eNext_Azure_State = AZURE_SM_PUB_LOC_TELEMETRY;
					eAzure_SM_Task = AZURE_SM_IDLE;
				}
				else
				{
					AZURE_PRINTF( ("Unsuccessfully Publish to SENSOR_TELEMETRY Topic\r\n"));
					AZURE_PRINTF( ("Disconnect\r\n"));
					MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);
					eAzure_SM_Task = AZURE_SM_STATES_BNDRY;
				}

				break;

    		case AZURE_SM_PUB_LOC_TELEMETRY:
				vTaskDelay(pdMS_TO_TICKS(1000));

				memset(&(xPublishParameters), 0x00, sizeof(xPublishParameters));
				memset(cTopic, 0, sizeof(cTopic));
				memset(cPayload, 0, sizeof(cPayload));

                sprintf(cTopic, AZURE_IOT_TELEMETRY_TOPIC_FOR_PUB, clientcredentialAZURE_IOT_DEVICE_ID);
				sprintf(cPayload, Device_Location_Telemetry_JSON, lat, lon, alt);

				xPublishParameters.pucTopic = (const uint8_t *)cTopic;
				xPublishParameters.pvData = cPayload;
				xPublishParameters.usTopicLength = (uint16_t)strlen(cTopic);
				xPublishParameters.ulDataLength = strlen(cPayload);
				xPublishParameters.xQoS = eMQTTQoS0;

				if( MQTT_AGENT_Publish(xMQTTHandle, &xPublishParameters, AzureTwinDemoTIMEOUT) == eMQTTAgentSuccess )
				{
					AZURE_PRINTF( ("Successfully Publish to LOC_TELEMETRY Topic\r\n"));
					eNext_Azure_State = AZURE_SM_PUB_CELLULAR_TELEMETRY;
					eAzure_SM_Task = AZURE_SM_IDLE;
				}
				else
				{
					AZURE_PRINTF( ("Unsuccessfully Publish to LOC_TELEMETRY Topic\r\n"));
					AZURE_PRINTF( ("Disconnect\r\n"));
					MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);
					eAzure_SM_Task = AZURE_SM_STATES_BNDRY;
				}

				break;

    		case AZURE_SM_PUB_CELLULAR_TELEMETRY:
				vTaskDelay(pdMS_TO_TICKS(1000));

				memset(&(xPublishParameters), 0x00, sizeof(xPublishParameters));
				memset(cTopic, 0, sizeof(cTopic));
				memset(cPayload, 0, sizeof(cPayload));

                sprintf(cTopic, AZURE_IOT_TELEMETRY_TOPIC_FOR_PUB, clientcredentialAZURE_IOT_DEVICE_ID);
				sprintf(cPayload, Device_Cellular_Telemetry_JSON, mcc , mnc, lac, cid, gsm.m.rssi, iccid, imei, modem_fw, device_id);

				xPublishParameters.pucTopic = (const uint8_t *)cTopic;
				xPublishParameters.pvData = cPayload;
				xPublishParameters.usTopicLength = (uint16_t)strlen(cTopic);
				xPublishParameters.ulDataLength = strlen(cPayload);
				xPublishParameters.xQoS = eMQTTQoS0;

				if( MQTT_AGENT_Publish(xMQTTHandle, &xPublishParameters, AzureTwinDemoTIMEOUT) == eMQTTAgentSuccess )
				{
					AZURE_PRINTF( ("Successfully Publish to CELLULAR_TELEMETRY Topic\r\n"));
					eNext_Azure_State = AZURE_SM_PUB_SENSOR_TELEMETRY;
					eAzure_SM_Task = AZURE_SM_IDLE;
				}
				else
				{
					AZURE_PRINTF( ("Unsuccessfully Publish to CELLULAR_TELEMETRY Topic\r\n"));
					AZURE_PRINTF( ("Disconnect\r\n"));
					MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);
					eAzure_SM_Task = AZURE_SM_STATES_BNDRY;
				}

				break;

    		case AZURE_SM_IDLE:
    			vTaskDelay(pdMS_TO_TICKS(60000));
    			eAzure_SM_Task = eNext_Azure_State;
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
