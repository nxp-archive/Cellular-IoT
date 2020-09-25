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
#define clientcredentialAZURE_MQTT_BROKER_ENDPOINT 	""

/*
 * @brief Host name.
 *
 * @todo Set this to the unique name of your IoT Thing.
 */
#define clientcredentialAZURE_IOT_THING_NAME               ""

/*
 * @brief UniStream MQTT username.
 *
 * @todo The UniStream MQTT username will be {iothubhostname}/{device_id}/?api-version=2018-06-30, 
 * where {iothubhostname} is the full CName of the IoT hub and {device_id} is the ClientID.
 */
#define clientcredentialAZURE_IOT_MQTT_USERNAME clientcredentialAZURE_MQTT_BROKER_ENDPOINT "/" clientcredentialAZURE_IOT_THING_NAME "/?api-version=2018-06-30"

/*
 * @brief Port number the MQTT broker is using.
 */
#define clientcredentialAZURE_MQTT_BROKER_PORT             8883

/*
 * @brief Various MQTT topics for the Device Twins mechanism
 */
#define AZURE_IOT_MQTT_TWIN_SET_TOPIC  "$iothub/twin/PATCH/properties/reported/?$rid={%d}"
#define AZURE_IOT_MQTT_TWIN_GET_TOPIC  "$iothub/twin/GET/?$rid={%d}"
#define AZURE_OPERATION_RESPONSE_TOPIC "$iothub/twin/res/#"

#endif /* MSFT_AZURE_IOT_CLIENTCREDENTIAL_H_ */
