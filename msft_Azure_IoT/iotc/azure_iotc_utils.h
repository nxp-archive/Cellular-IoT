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
#define AZURE_IOT_MQTT_TWIN_SET_TOPIC         "$iothub/twin/PATCH/properties/reported/?$rid={%d}"
#define AZURE_IOT_MQTT_TWIN_GET_TOPIC         "$iothub/twin/GET/?$rid={%d}"
#define AZURE_IOT_TWIN_RESPONSE_TOPIC_FOR_SUB "$iothub/twin/res/#"
#define AZURE_IOT_TWIN_PATCH_TOPIC_FOR_SUB    "$iothub/twin/PATCH/properties/desired/#"

#define AZURE_IOT_TELEMETRY_TOPIC_FOR_SUB     "devices/%s/messages/events/#"
#define AZURE_IOT_TELEMETRY_TOPIC_FOR_PUB     "devices/%s/messages/events/"
#define AZURE_IOT_C2D_TOPIC_FOR_SUB           "devices/%s/messages/devicebound/#"
#define AZURE_IOT_METHOD_TOPIC_FOR_SUB        "$iothub/methods/POST/#"
#define AZURE_IOT_METHOD_TOPIC_FOR_PUB        "$iothub/methods/res/%s/?$rid={%d}"


void generate_sas_token(char * buffer, size_t bufferSize);
bool topic_check(const char* topic, size_t len, char* str, size_t str_len);

#endif /* IOTC_AZURE_IOTC_UTILS_H_ */
