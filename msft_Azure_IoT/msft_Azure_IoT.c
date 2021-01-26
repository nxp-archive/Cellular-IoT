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
#include "timers.h"
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

/* Board specific accelerometer driver include */
#if defined(BOARD_ACCEL_FXOS)
#include "fsl_fxos.h"
#elif defined(BOARD_ACCEL_MMA)
#include "fsl_mma.h"
#endif

#if defined(BOARD_ACCEL_FXOS) || defined(BOARD_ACCEL_MMA)
/* Type definition of structure for data from the accelerometer */
typedef struct
{
    int16_t A_x;
    int16_t A_y;
    int16_t A_z;
} vector_t;
#endif

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
		"\"iccid\": \"%s\", "					\
		"\"imei\": \"%s\", " 					\
		"\"modem_fw\": \"%s\", " 				\
		"\"device_id\": \"%s\"" 				\
	"}"

uint32_t mcc = 208;
uint32_t mnc = 01;
uint32_t lac = 0;
uint32_t cid = 4;
char iccid[] = {"89148000005471125146"};
char imei[] = {"354658090355378"};
char modem_fw[] = {"UE5.2.0.1"};
char device_id[] = {"19494031513"};


#define Device_Sensor_Telemetry_JSON 		\
	"{"										\
		"\"aX\": %d, "	 					\
		"\"aY\": %d, "	 					\
		"\"aZ\": %d, "	 					\
		"\"light_sensor\": %.2f, " 			\
		"\"rssi\": %d, "	 				\
		"\"current\": %.2f, "				\
		"\"button\": %d"					\
	"}"

vector_t accel_vector;
double light_sensor = 79.1;
//uint16_t rssi = -40;
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

char tx_interval_state[50];
#define NB_OF_DIGITS_FOR_UINT16 5
#define SEC_TO_MILLISEC_MULTIPLIER	1000
#define MIN_TO_SEC_MULTIPLIER		60
#define HOUR_TO_MIN_MULTIPLIER		60
int16_t tx_interval_hours = 0;
int16_t tx_interval_mins = 1;
int16_t tx_interval_secs = 0;

double lat = 49.187104;
double lon = -0.308766;
double alt = 0;

#define Device_Led_Property_JSON 	\
	"{"								\
		"\"rgb_red\": %s, "			\
		"\"rgb_green\": %s, "		\
		"\"rgb_blue\": %s"			\
	"}"

char red_led_state[6];
char green_led_state[6];
char blue_led_state[6];


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
	AZURE_SM_PUB_SET_LED_PROPERTIES,
	AZURE_SM_WAIT_SET_LED_PROPERTIES,
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
TimerHandle_t xTelemetryPublishTimer;
EventBits_t uxBits;
bool bIsStartUpPhase = true;
#define EVENT_BIT_MASK						( 1 << 0 )
#define LED_UPDATE_BIT_MASK					( 1 << 1 )
#define TELEMETRY_PUB_BIT_MASK				( 1 << 2 )
#define REPORTING_PERIOD_UPDATE_BIT_MASK	( 1 << 3 )

#if defined(BOARD_ACCEL_FXOS) || defined(BOARD_ACCEL_MMA)
/* Actual state of accelerometer */
uint16_t accState       = 0;
uint16_t parsedAccState = 0;
#endif

/* Accelerometer driver specific defines */
#if defined(BOARD_ACCEL_FXOS)
#define ACCELL_READ_SENSOR_DATA(handle, data) FXOS_ReadSensorData(handle, data)
#elif defined(BOARD_ACCEL_MMA)
#define ACCELL_READ_SENSOR_DATA(handle, data) MMA_ReadSensorData(handle, data)
#endif

/* Accelerometer and magnetometer */
#if defined(BOARD_ACCEL_FXOS)
extern fxos_handle_t accelHandle;
#elif defined(BOARD_ACCEL_MMA)
extern mma_handle_t accelHandle;
#endif

#if defined(BOARD_ACCEL_FXOS) || defined(BOARD_ACCEL_MMA)
extern uint8_t g_accelDataScale;
extern uint8_t g_accelResolution;
#endif

extern void turnOnLed(uint8_t id);
extern void turnOffLed(uint8_t id);

#define RED_LED_ID		0U
#define GREEN_LED_ID	1U
#define BLUE_LED_ID		2U

char* sas_token;
char* assigned_hub;
char* username;
char* password;
char* operation_id;



#if defined(BOARD_ACCEL_FXOS) || defined(BOARD_ACCEL_MMA)
/*!
 * @brief Read accelerometer sensor value
 */
void read_mag_accel(vector_t *results, bool *status, uint8_t accelResolution)
{
#if defined(BOARD_ACCEL_FXOS)
    fxos_data_t sensorData = {0};
#elif defined(BOARD_ACCEL_MMA)
    mma_data_t sensorData = {0};
#endif
    if (kStatus_Success != ACCELL_READ_SENSOR_DATA(&accelHandle, &sensorData))
    {
        /* Failed to read magnetometer and accelerometer data! */
        *status = false;
        return;
    }

    uint8_t divider = (1 << (16 - accelResolution));

    /* Get the accelerometer data from the sensor */
    results->A_x =
        (int16_t)((uint16_t)((uint16_t)sensorData.accelXMSB << 8) | (uint16_t)sensorData.accelXLSB) / divider;
    results->A_y =
        (int16_t)((uint16_t)((uint16_t)sensorData.accelYMSB << 8) | (uint16_t)sensorData.accelYLSB) / divider;
    results->A_z =
        (int16_t)((uint16_t)((uint16_t)sensorData.accelZMSB << 8) | (uint16_t)sensorData.accelZLSB) / divider;

    *status = true;
}

/* Build JSON document with reported state of the "accel" */
int readAccelData(vector_t* vec)
{
    /* Read data from accelerometer */
    bool read_ok = false;
    read_mag_accel(vec, &read_ok, g_accelResolution);
    if (read_ok == false)
    {
        return -1;
    }

    /* Convert raw data from accelerometer to acceleration range multiplied by 1000 (for range -2/+2 the values will be
     * in range -2000/+2000) */
    vec->A_x = (int16_t)((int32_t)vec->A_x * g_accelDataScale * 1000 / (1 << (g_accelResolution - 1)));
    vec->A_y = (int16_t)((int32_t)vec->A_y * g_accelDataScale * 1000 / (1 << (g_accelResolution - 1)));
    vec->A_z = (int16_t)((int32_t)vec->A_z * g_accelDataScale * 1000 / (1 << (g_accelResolution - 1)));

    return 0;
}
#endif

int16_t check_and_get_time_interval(char * time_in_characters)
{
	int16_t time = 0;

	for (uint8_t i=0; i<strlen(time_in_characters); i++)
	{
		if( ( '0' <= time_in_characters[i] ) && ( time_in_characters[i] <= '9' ) )
		{
			time *= 10;
			time += (time_in_characters[i] - 0x30);
		}
		else
		{
			/* Invalid character, stop processing here --> -1 = error */
			time = -1;
			break;
		}
	}

	return time;

}

int process_and_update_tx_telemetry_time(const char * interval)
{
	/*
	 * Step 1: Search for P
	 * Step 2: Search for T and discard everything in between
	 * Step 3: Search for H/h and M/m and S/s and keep the value seen before
	 * Step 4: Store the value found in the correct variable
	 */
	typedef enum
	{
		FIND_P,
		FIND_T,
		PROCESS_VALUES,
		STOP_PROCESSING
	} searchSteps;

	searchSteps state = FIND_P;

	int i,j = 0;
	char temp[5];

	memset(temp, 0, sizeof(temp));

	for(i=0; i<strlen(interval); i++)
	{
		switch(state)
		{
			case FIND_P:
				if (interval[i] == 0x50 ||
					interval[i] == 0x70)	state = FIND_T;
				break;

			case FIND_T:
				if (interval[i] == 0x54 ||
					interval[i] == 0x74 )	state = PROCESS_VALUES;
				break;

			case PROCESS_VALUES:
				if (interval[i] == 0x48 ||
					interval[i] == 0x68 )
				{
					tx_interval_hours = check_and_get_time_interval(temp);
					if (tx_interval_hours == -1)
					{
						state = STOP_PROCESSING;
						break;
					}
					memset(temp, 0, sizeof(temp));
					j = 0;
				}
				else if (interval[i] == 0x4D ||
						 interval[i] == 0x6D )
				{
					tx_interval_mins = check_and_get_time_interval(temp);
					if (tx_interval_mins == -1)
					{
						state = STOP_PROCESSING;
						break;
					}
					memset(temp, 0, sizeof(temp));
					j = 0;
				}
				else if (interval[i] == 0x53 ||
						 interval[i] == 0x73 )
				{
					tx_interval_secs = check_and_get_time_interval(temp);
					if (tx_interval_secs == -1)
					{
						state = STOP_PROCESSING;
						break;
					}
					memset(temp, 0, sizeof(temp));
					j = 0;
				}
				else
				{
					temp[j++] = (char)interval[i];
					/* If number processing get more than 5 digits,
					 * the format is too large to be stored in uint16_t type so we stop processing
					 */
					if(j == NB_OF_DIGITS_FOR_UINT16)	state = STOP_PROCESSING;
				}
				break;

			case STOP_PROCESSING:
				break;

			default:
				break;
		}
		if (state == STOP_PROCESSING)	break;
	}

	if (state == PROCESS_VALUES &&
		tx_interval_hours != -1 &&
		tx_interval_mins != -1 &&
		tx_interval_secs != -1)
	{
		xTimerChangePeriod(xTelemetryPublishTimer,
						   pdMS_TO_TICKS(tx_interval_secs  * SEC_TO_MILLISEC_MULTIPLIER +
										 tx_interval_mins  * MIN_TO_SEC_MULTIPLIER * SEC_TO_MILLISEC_MULTIPLIER +
										 tx_interval_hours * HOUR_TO_MIN_MULTIPLIER * MIN_TO_SEC_MULTIPLIER * SEC_TO_MILLISEC_MULTIPLIER),
						   100);
		return 0;	/* Success */
	}
	else
	{
		return 1;	/* Error */
	}
}


void vTimerCallback( TimerHandle_t xTimer )
{
   /* Optionally do something if the pxTimer parameter is NULL. */
   configASSERT( xTimer );

   xEventGroupSetBits(xCreatedEventGroup, TELEMETRY_PUB_BIT_MASK);

}

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
		    memset(red_led_state, 0, sizeof(red_led_state));
			memcpy(red_led_state, v, strlen(v));
			if(strcmp(red_led_state, "true") == 0)
			{
				turnOnLed(RED_LED_ID);
			}
			else if(strcmp(red_led_state, "false") == 0)
			{
				turnOffLed(RED_LED_ID);
			}
			else
			{
				/* Do nothing */
			}
			AZURE_IOTC_FREE(v);
			xEventGroupSetBits(xCreatedEventGroup, LED_UPDATE_BIT_MASK);
		}
	}
	else if (strcmp(propertyName, "rgb_green") == 0)
	{
		char* v = jsobject_get_data_by_name(&object, "rgb_green");
		if(v)
		{
			AZURE_PRINTF(("==> Received a 'GREEN LED' update! New Value => %s\n", v));
		    memset(green_led_state, 0, sizeof(green_led_state));
			memcpy(green_led_state, v, strlen(v));
			if(strcmp(green_led_state, "true") == 0)
			{
				turnOnLed(GREEN_LED_ID);
			}
			else if(strcmp(green_led_state, "false") == 0)
			{
				turnOffLed(GREEN_LED_ID);
			}
			else
			{
				/* Do nothing */
			}
			AZURE_IOTC_FREE(v);
			xEventGroupSetBits(xCreatedEventGroup, LED_UPDATE_BIT_MASK);
		}
	}
	else if (strcmp(propertyName, "rgb_blue") == 0)
	{
		char* v = jsobject_get_data_by_name(&object, "rgb_blue");
		if(v)
		{
			AZURE_PRINTF(("==> Received a 'BLUE LED' update! New Value => %s\n", v));
		    memset(blue_led_state, 0, sizeof(blue_led_state));
			memcpy(blue_led_state, v, strlen(v));
			if(strcmp(blue_led_state, "true") == 0)
			{
				turnOnLed(BLUE_LED_ID);
			}
			else if(strcmp(blue_led_state, "false") == 0)
			{
				turnOffLed(BLUE_LED_ID);
			}
			else
			{
				/* Do nothing */
			}
			AZURE_IOTC_FREE(v);
			xEventGroupSetBits(xCreatedEventGroup, LED_UPDATE_BIT_MASK);
		}
	}
	else if (strcmp(propertyName, "tx_interval") == 0)
	{
		char* v = jsobject_get_data_by_name(&object, "tx_interval");
		if(v)
		{
			AZURE_PRINTF(("==> Received a 'TX Interval' update! New Value => %s\n", v));
		    memset(tx_interval_state, 0, sizeof(tx_interval_state));
			memcpy(tx_interval_state, v, strlen(v));
			process_and_update_tx_telemetry_time(v);
			AZURE_IOTC_FREE(v);
			xEventGroupSetBits(xCreatedEventGroup, REPORTING_PERIOD_UPDATE_BIT_MASK);
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

    memset(red_led_state, 0, sizeof(red_led_state));
    memset(green_led_state, 0, sizeof(red_led_state));
    memset(blue_led_state, 0, sizeof(red_led_state));

    memcpy(red_led_state, "false", strlen("false"));
    memcpy(green_led_state, "false", strlen("false"));
    memcpy(blue_led_state, "false", strlen("false"));

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
    xTelemetryPublishTimer = xTimerCreate( "Telemetry Publish Timer",
											pdMS_TO_TICKS(60000),
											pdFALSE,
											( void * ) 0,
											vTimerCallback );

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
    			vTaskDelay(pdMS_TO_TICKS(3000));

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
				MQTT_AGENT_Disconnect(xMQTTHandle, pdMS_TO_TICKS( 10000UL ));

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
					if( true == bIsStartUpPhase)	eAzure_SM_Task = AZURE_SM_PUB_GET_TW_PROPERTIES;
					else	eAzure_SM_Task = AZURE_SM_IDLE;
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
    				AZURE_PRINTF ( ("Got response from AZURE_SM_PUB_GET_TW_PROPERTIES state\n") );
    			}
    			else
    			{
    				AZURE_PRINTF ( ("No response received for AZURE_SM_PUB_GET_TW_PROPERTIES state\n") );
    				vTaskDelay(pdMS_TO_TICKS(2000));
    			}

				if( true == bIsStartUpPhase)	eAzure_SM_Task = AZURE_SM_PUB_SET_TW_PROPERTIES;
				else	eAzure_SM_Task = AZURE_SM_IDLE;

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
    				AZURE_PRINTF ( ("Got response from AZURE_SM_PUB_SET_TW_PROPERTIES state\n") );
    			}
    			else
    			{
    				AZURE_PRINTF ( ("No response received for AZURE_SM_PUB_SET_TW_PROPERTIES state\n") );
    				vTaskDelay(pdMS_TO_TICKS(2000));
    			}

				if( true == bIsStartUpPhase)	eAzure_SM_Task = AZURE_SM_PUB_SET_CONTROL_PROPERTIES;
				else	eAzure_SM_Task = AZURE_SM_IDLE;

    			break;

    		case AZURE_SM_PUB_SET_CONTROL_PROPERTIES:
    			vTaskDelay(pdMS_TO_TICKS(1000));

                memset(&(xPublishParameters), 0x00, sizeof(xPublishParameters));
                memset(cTopic, 0, sizeof(cTopic));
                memset(cPayload, 0, sizeof(cPayload));

                sprintf(cTopic, AZURE_IOT_MQTT_TWIN_SET_TOPIC, Req_Id++ );
                sprintf(cPayload, Device_Control_Property_JSON, tx_interval_hours , tx_interval_mins, tx_interval_secs);

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
    				AZURE_PRINTF ( ("Got response from AZURE_SM_PUB_SET_CONTROL_PROPERTIES state\n") );
    			}
    			else
    			{
    				AZURE_PRINTF ( ("No response received for AZURE_SM_PUB_SET_CONTROL_PROPERTIES state\n") );
    				vTaskDelay(pdMS_TO_TICKS(2000));
    			}

				if( true == bIsStartUpPhase)	eAzure_SM_Task = AZURE_SM_PUB_SET_LED_PROPERTIES;
				else	eAzure_SM_Task = AZURE_SM_IDLE;

    			break;

    		case AZURE_SM_PUB_SET_LED_PROPERTIES:
    			vTaskDelay(pdMS_TO_TICKS(1000));

                memset(&(xPublishParameters), 0x00, sizeof(xPublishParameters));
                memset(cTopic, 0, sizeof(cTopic));
                memset(cPayload, 0, sizeof(cPayload));

                sprintf(cTopic, AZURE_IOT_MQTT_TWIN_SET_TOPIC, Req_Id++ );
                sprintf(cPayload, Device_Led_Property_JSON, red_led_state, green_led_state, blue_led_state);

                xPublishParameters.pucTopic = (const uint8_t *)cTopic;
                xPublishParameters.pvData = cPayload;
                xPublishParameters.usTopicLength = (uint16_t)strlen((const char *)cTopic);
                xPublishParameters.ulDataLength = strlen((const char *)cPayload);
                xPublishParameters.xQoS = eMQTTQoS0;

                if( MQTT_AGENT_Publish(xMQTTHandle, &xPublishParameters, AzureTwinDemoTIMEOUT) == eMQTTAgentSuccess )
                {
                	AZURE_PRINTF( ("Successfully Publish to Device Twin Properties Topic\r\n"));
                    eAzure_SM_Task = AZURE_SM_WAIT_SET_LED_PROPERTIES;
                }
                else
                {
                	AZURE_PRINTF( ("Unsuccessfully Publish to Device Twin Properties Topic\r\n"));
                	AZURE_PRINTF( ("Disconnect\r\n"));
                	MQTT_AGENT_Disconnect(xMQTTHandle, AzureTwinDemoTIMEOUT);
                	eAzure_SM_Task = AZURE_SM_STATES_BNDRY;
                }

				break;

    		case AZURE_SM_WAIT_SET_LED_PROPERTIES:

    			if( xEventGroupWaitBits(xCreatedEventGroup, EVENT_BIT_MASK, pdTRUE, pdFALSE, AzureTwinDemoTIMEOUT) == EVENT_BIT_MASK )
    			{
    				AZURE_PRINTF ( ("Got response from AZURE_SM_PUB_SET_LED_PROPERTIES state\n") );
    			}
    			else
    			{
    				eAzure_SM_Task = AZURE_SM_PUB_SENSOR_TELEMETRY;
    				AZURE_PRINTF ( ("No response received for AZURE_SM_PUB_SET_LED_PROPERTIES state\n") );
    				vTaskDelay(pdMS_TO_TICKS(2000));
    			}

				if( true == bIsStartUpPhase)
				{
					eAzure_SM_Task = AZURE_SM_PUB_SENSOR_TELEMETRY;
					bIsStartUpPhase = false;
				}
				else	eAzure_SM_Task = AZURE_SM_IDLE;

    			break;

    		case AZURE_SM_PUB_SENSOR_TELEMETRY:
				vTaskDelay(pdMS_TO_TICKS(1000));

				if(readAccelData(&accel_vector))
				{
					accel_vector.A_x = 0;
					accel_vector.A_y = 0;
					accel_vector.A_z = 0;
				}

				memset(&(xPublishParameters), 0x00, sizeof(xPublishParameters));
				memset(cTopic, 0, sizeof(cTopic));
				memset(cPayload, 0, sizeof(cPayload));

                sprintf(cTopic, AZURE_IOT_TELEMETRY_TOPIC_FOR_PUB, clientcredentialAZURE_IOT_DEVICE_ID);
				sprintf(cPayload, Device_Sensor_Telemetry_JSON, accel_vector.A_x , accel_vector.A_y, accel_vector.A_z, light_sensor, gsm.m.rssi, current, button);

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
				sprintf(cPayload, Device_Cellular_Telemetry_JSON, mcc , mnc, lac, cid, iccid, imei, modem_fw, device_id);

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
    			if( xTimerIsTimerActive( xTelemetryPublishTimer ) == pdFALSE )
    			{
    				xTimerStart( xTelemetryPublishTimer, 0 );
    			}

    			uxBits = xEventGroupWaitBits(xCreatedEventGroup,
    										 LED_UPDATE_BIT_MASK | TELEMETRY_PUB_BIT_MASK | REPORTING_PERIOD_UPDATE_BIT_MASK,
											 pdTRUE,
											 pdFALSE,
											 pdMS_TO_TICKS( 120000UL ));

    			if( ( uxBits & LED_UPDATE_BIT_MASK ) != 0 )
				{
					eAzure_SM_Task = AZURE_SM_PUB_SET_LED_PROPERTIES;
				}
				else if( ( uxBits & REPORTING_PERIOD_UPDATE_BIT_MASK ) != 0 )
				{
					eAzure_SM_Task = AZURE_SM_PUB_SET_CONTROL_PROPERTIES;
				}
				else if( ( uxBits & TELEMETRY_PUB_BIT_MASK ) != 0 )
				{
					eAzure_SM_Task = eNext_Azure_State;
				}
				else
				{
					/* Do nothing */
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
