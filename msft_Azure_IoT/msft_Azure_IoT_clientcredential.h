/*
 * msft_Azure_IoT_clientcredential.h
 *
 *  Created on: 8 avr. 2020
 *      Author: nxf37702
 */

#ifndef MSFT_AZURE_IOT_CLIENTCREDENTIAL_H_
#define MSFT_AZURE_IOT_CLIENTCREDENTIAL_H_

/*
 * Include for device certificate and private key
 */
#include "msft_Azure_IoT_clientcredential_keys.h"

/*
 * MQTT Broker endpoint.
 */
#define AZURE_IOT_HUB_BROKER_ENDPOINT 	"Cellular-IoT-Hub.azure-devices.net"


/* Use of a "define" and not a "static const" here to be able to
 * use pre-compile concatenation on the string. */
#define AZURE_DEVICE_NAME               "myNXPCellularIoTDevice"

#define AZURE_IOT_MQTT_USERNAME AZURE_IOT_HUB_BROKER_ENDPOINT "/" AZURE_DEVICE_NAME "/?api-version=2018-06-30"

/*
 * Port number the MQTT broker is using.
 */
#define AZURE_IOT_MQTT_PORT             8883

/*
 * Various MQTT topics for the Device Twins mechanism (Shadow)
 */
#define AZURE_IOT_MQTT_TWIN_SET_TOPIC  "$iothub/twin/PATCH/properties/reported/?$rid={%d}"
#define AZURE_IOT_MQTT_TWIN_GET_TOPIC  "$iothub/twin/GET/?$rid={%d}"
#define AZURE_OPERATION_RESPONSE_TOPIC "$iothub/twin/res/#"

#endif /* MSFT_AZURE_IOT_CLIENTCREDENTIAL_H_ */
