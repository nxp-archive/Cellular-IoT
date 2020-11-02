/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef MSFT_AZURE_IOT_CLIENTCREDENTIAL_H_
#define MSFT_AZURE_IOT_CLIENTCREDENTIAL_H_

/*
 * @brief MQTT Broker endpoint.
 *
 * @todo Set this to the fully-qualified DNS name of your MQTT broker.
 */
#define clientcredentialAZURE_MQTT_BROKER_ENDPOINT 	"global.azure-devices-provisioning.net"

/*
 * @brief Host name.
 *
 * @todo Set this to the Scope ID of your IoT Thing.
 */
#define clientcredentialAZURE_IOT_SCOPE_ID               "0ne001AEE7B"

/*
 * @brief Host name.
 *
 * @todo Set this to the unique name of your IoT Thing.
 */
#define clientcredentialAZURE_IOT_THING_NAME               "29w66iriqt5"

/*
 * @brief UniStream MQTT username.
 *
 * @todo The UniStream MQTT username will be {iothubhostname}/{device_id}/?api-version=2018-06-30, 
 * where {iothubhostname} is the full CName of the IoT hub and {device_id} is the ClientID.
 */
//#define clientcredentialAZURE_IOT_MQTT_USERNAME clientcredentialAZURE_MQTT_BROKER_ENDPOINT "/" clientcredentialAZURE_IOT_THING_NAME "/?api-version=2018-06-30"
#define clientcredentialAZURE_IOT_MQTT_USERNAME clientcredentialAZURE_IOT_SCOPE_ID "/registrations/" clientcredentialAZURE_IOT_THING_NAME "/api-version=2019-03-31"

/*
 * @brief Port number the MQTT broker is using.
 */
#define clientcredentialAZURE_MQTT_BROKER_PORT             8883

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

#endif /* MSFT_AZURE_IOT_CLIENTCREDENTIAL_H_ */
