/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef MSFT_AZURE_IOT_CLIENTCREDENTIAL_H_
#define MSFT_AZURE_IOT_CLIENTCREDENTIAL_H_
/*
 * AZURE_IOT_BROKER = 1 --> Azure IoT Hub
 * AZURE_IOT_BROKER = 2 --> Azure IoT Central
 */
#define AZURE_IOT_BROKER 2

/*
 * @brief MQTT Broker endpoint.
 *
 * @todo Set this to the fully-qualified DNS name of your MQTT broker.
 */
#define clientcredentialAZURE_MQTT_BROKER_ENDPOINT 	     ""


#if (AZURE_IOT_BROKER == 2)
/*
 * @brief Host name.
 *
 * @todo Set this to the Scope ID of your IoT Thing.
 */
#define clientcredentialAZURE_IOT_SCOPE_ID               ""

#endif
/*
 * @brief Host name.
 *
 * @todo Set this to the unique name of your IoT Thing.
 */
#define clientcredentialAZURE_IOT_THING_NAME             ""

/*
 * @brief UniStream MQTT username.
 *
 * @todo The UniStream MQTT username for Azure IoT Hub will be {iothubhostname}/{device_id}/?api-version=2018-06-30,
 * where {iothubhostname} is the full CName of the IoT hub and {device_id} is the ClientID.
 *
 * @todo The MQTT username forAzure IoT Central will be {scope_id}/registrations/{device_id}/api-version=2019-03-31,
 * where {scope_id} is the Scope ID of the Azure IoT Central Dashboard and {device_id} is the ClientID.
 *
 */


#if (AZURE_IOT_BROKER == 1)

#define clientcredentialAZURE_IOT_MQTT_USERNAME clientcredentialAZURE_MQTT_BROKER_ENDPOINT "/" clientcredentialAZURE_IOT_DEVICE_ID "/?api-version=2018-06-30"

#elif (AZURE_IOT_BROKER == 2)

#define clientcredentialAZURE_IOT_MQTT_USERNAME clientcredentialAZURE_IOT_SCOPE_ID "/registrations/" clientcredentialAZURE_IOT_DEVICE_ID "/api-version=2019-03-31"

#endif
/*
 * @brief Port number the MQTT broker is using.
 */
#define clientcredentialAZURE_MQTT_BROKER_PORT             8883


#endif /* MSFT_AZURE_IOT_CLIENTCREDENTIAL_H_ */
