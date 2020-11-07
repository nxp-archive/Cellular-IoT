/*
 * Copyright 2019-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IOTC_AZURE_IOTC_UTILS_H_
#define IOTC_AZURE_IOTC_UTILS_H_

#include <stdbool.h>
#include "FreeRTOS.h"
#include "iotc_definitions.h"

/*
 * @brief Various MQTT topics for the Device Twins mechanism
 */
/* Device Twin topics */
#define AZURE_IOT_MQTT_TWIN_SET_TOPIC         "$iothub/twin/PATCH/properties/reported/?$rid=%d"
#define AZURE_IOT_MQTT_TWIN_GET_TOPIC         "$iothub/twin/GET/?$rid=%d"
#define AZURE_IOT_TWIN_RESPONSE_TOPIC_FOR_SUB "$iothub/twin/res/#"
#define AZURE_IOT_TWIN_PATCH_TOPIC_FOR_SUB    "$iothub/twin/PATCH/properties/desired/#"

/* Device Telemetry topics */
#define AZURE_IOT_C2D_TOPIC_FOR_SUB           "devices/%s/messages/devicebound/#"
#define AZURE_IOT_TELEMETRY_TOPIC_FOR_SUB     "devices/%s/messages/events/#"
#define AZURE_IOT_TELEMETRY_TOPIC_FOR_PUB     "devices/%s/messages/events/"

/* Device Method topics */
#define AZURE_IOT_METHOD_TOPIC_FOR_SUB        "$iothub/methods/#"
#define AZURE_IOT_METHOD_TOPIC_FOR_PUB        "$iothub/methods/res/%s/?$rid=%d"

/* DPS Registration topics */
#define AZURE_IOT_DPS_REGISTRATION_TOPIC_FOR_SUB "$dps/registrations/res/#"
#define AZURE_IOT_DPS_REGISTRATION_TOPIC_FOR_PUB "$dps/registrations/PUT/iotdps-register/?$rid=%d"
#define AZURE_IOT_DPS_GET_REGISTRATION_OPERATION_STATUS_TOPIC_FOR_PUB "$dps/registrations/GET/iotdps-get-operationstatus/?$rid=%d&operationId=%s"

#define BUFFER_SIZE_16     16
#define BUFFER_SIZE_32     32
#define BUFFER_SIZE_64     64
#define BUFFER_SIZE_128   128
#define BUFFER_SIZE_256   256
#define BUFFER_SIZE_512   512
#define BUFFER_SIZE_1024 1024
#define BUFFER_SIZE_2048 2048
#define BUFFER_SIZE_4096 4096

void generateSasToken(char** ptr_buffer, char* scope_id, size_t scope_id_length, char* device_id, size_t device_id_length, char* key, size_t key_length);
void getUsernameAndPassword(char** ptr_username, char** ptr_password, char* device_id, size_t device_id_length, char* host_name, size_t host_name_length, char* key, size_t key_length);
bool topic_check(const char* topic, size_t len, char* str, size_t str_len);

#endif /* IOTC_AZURE_IOTC_UTILS_H_ */
