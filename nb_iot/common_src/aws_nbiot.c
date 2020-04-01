/*
 * NXP Header file To be Defined
 */

/**
 * @file aws_nbiot.c
 * @brief NB-IoT Interface.
 */

#include <stdio.h>
#include "FreeRTOS.h"
#include "aws_nbiot.h"
#include "nbiot_common.h"
#include "gm01q_api.h"
#include "semphr.h"
//#include "nbiot_env_port.h"
#include "cust_usart_com.h"

/* This is here because the maximum DNS name length is defined in aws_secure_sockets.h.
 * NB-IoT must not have a dependency on aws_secure_sockets.h
 */
#define nbiotMAX_DNS_NAME_LENGTH 253

/* Only 1 NB-IoT module is present at the time */
//static uint8_t g_devid = 0;

/* Semaphore for connection */
static SemaphoreHandle_t g_connect_semaph;

/* Protect API */
static SemaphoreHandle_t g_nbiot_semaph;

/* NB-IoT status - turned on */
static uint8_t g_nbiot_is_on = 0;

/* NB-IoT status - turned on */
//static uint8_t g_connected = 0;

/**
 * @brief Initializes the NB-IoT module.
 *
 * This function must be called exactly once before any other
 * NB-IoT functions (including socket functions) can be used.
 *
 * @return eNbIoTSuccess if everything succeeds, eNbIoTFailure otherwise.
 */
NBIOTReturnCode_t NBIOT_On( uint32_t baudrate )
{
    bool result;

    /* Prevent re-initialization. NB-IoT is already on this is successful. */
    if (g_nbiot_is_on)
        return eNbIoTSuccess;

    /* Initialize NB-IoT shield */
    result = NBIOTSHIELD_Init(baudrate);
    if (kStatus_Success != result)
        return eNbIoTFailure;

    /* Power off the WLAN and wait 30ms */
    //NBIOTSHIELD_PowerUp(false);	/*TODO:Later, have a look at this */
    vTaskDelay(MSEC_TO_TICK(30));

//    if (true != Custom_Api_Initialize())
//		return eNbIoTFailure;

    if (SQNS_SUCCESS != NBIOT_USART_Ressources_Init())
        return eNbIoTFailure;

    /* Hard Reset the NB-IoT module */
    //gm01q_api_ResetModem();

    /* Create a on_nbiot semaphore, */
    g_nbiot_semaph = xSemaphoreCreateBinary();
    if (NULL == g_nbiot_semaph)
        return eNbIoTFailure;
    xSemaphoreGive(g_nbiot_semaph);

    /* Create a on_connect semaphore, */
    g_connect_semaph = xSemaphoreCreateBinary();
    if (NULL == g_connect_semaph)
        return eNbIoTFailure;

    /* Wait for NB-IoT */
    vTaskDelay(MSEC_TO_TICK(100));
    g_nbiot_is_on = 1;

    return eNbIoTSuccess;
}

/**
 * @brief Connects to Access Point.
 *
 * @param[in] pxNetworkParams Configuration to join AP.
 *
 * @return eWiFiSuccess if connection is successful, eWiFiFailure otherwise.
 */
//WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams )
//{
//    WLAN_AUTH_MODE auth_mode;
//    WLAN_CRYPT_TYPE crypt_type;
//    WIFIReturnCode_t status = eWiFiFailure;
//    const TickType_t connect_timeout = pdMS_TO_TICKS( 30000UL );
//    const TickType_t dhcp_timeout = pdMS_TO_TICKS( 20000UL );
//    (void)dhcp_timeout;
//    uint32_t tmp_ip4_addr = 0, tmp_ip4_mask = 0, tmp_ip4_gw = 0;
//    uint8_t ucDhcpSuccessful = 0;
//
//    /* Check initialization */
//    if (!g_wifi_is_on)
//        return eWiFiFailure;
//
//    /* Check params */
//    if (NULL == pxNetworkParams || NULL == pxNetworkParams->pcSSID || NULL == pxNetworkParams->pcPassword)
//        return eWiFiFailure;
//
//    /* Acquire semaphore */
//    if (xSemaphoreTake(g_wifi_semaph, portMAX_DELAY) == pdTRUE)
//    {
//        do {
//            /* Reset "connect" and "dhcp" semaphores */
//            g_expected_event = expected_event_default;
//            xQueueReset((void*)g_connect_semaph);
//            xQueueReset((void*)g_dhcp_semaph);
//            /* Disconnect Wi-Fi */
//            if (g_connected)
//            {
//                g_expected_event = expected_event_disconnect;
//                if (A_OK == qcom_disconnect(g_devid))
//                {
//                    /* Consider disconnected */
//                    g_connected = 0;
//                    /* Wait for callback, that is invoked from 'driver_task' context */
//                    if (pdTRUE != xSemaphoreTake(g_connect_semaph, connect_timeout))
//                    {
//                        break;
//                    }
//                    /* Workaround for ARP cache */
//                    if (0 != g_ip4_gw)
//                    {
//                        if (A_OK != qcom_ipconfig(g_devid, QCOM_IPCONFIG_STATIC, &g_ip4_gw, &g_ip4_mask, &g_ip4_gw))
//                        {
//                            break;
//                        }
//                        g_ip4_addr = g_ip4_mask = g_ip4_gw = 0;
//                    }
//                }
//                else
//                {
//                    break;
//                }
//            }
//            /* Set Wi-Fi to device mode */
//            if (A_OK != (A_STATUS)qcom_op_set_mode(g_devid, QCOM_WLAN_DEV_MODE_STATION))
//            {
//                break;
//            }
//
//            /* Set SSID, must be done before auth, cipher and passphrase */
//            strncpy(g_ssid.ssid, pxNetworkParams->pcSSID, sizeof(g_ssid.ssid) - 1);
//            if (A_OK != (A_STATUS)qcom_set_ssid(g_devid, &g_ssid))
//            {
//                break;
//            }
//
//            g_security = pxNetworkParams->xSecurity;
//            /* Convert 'WIFISecurity_t' to 'WLAN_AUTH_MODE', 'WLAN_CRYPT_TYPE' */
//            if (eWiFiSuccess != conv_security_to_qcom(pxNetworkParams->xSecurity, &auth_mode, &crypt_type))
//            {
//                break;
//            }
//
//            /* Set encyption mode */
//            if (A_OK != (A_STATUS)qcom_sec_set_encrypt_mode(g_devid, crypt_type))
//            {
//                break;
//            }
//
//            /* Set auth mode */
//            if (A_OK != qcom_sec_set_auth_mode(g_devid, auth_mode))
//            {
//                break;
//            }
//
//            /* Set passphrase */
//            strncpy(g_passphr.passphrase, pxNetworkParams->pcPassword, sizeof(g_passphr.passphrase) - 1);
//            if (A_OK != qcom_sec_set_passphrase(g_devid, &g_passphr))
//            {
//                break;
//            }
//
//            /* Set channel */
//            if (0 != pxNetworkParams->cChannel)
//            {
//                if (A_OK != qcom_set_channel(g_devid, pxNetworkParams->cChannel))
//                {
//                    break;
//                }
//            }
//
//            /* Set connect_callback */
//            if (A_OK != qcom_set_connect_callback(g_devid, (void *)aws_connect_cb))
//            {
//                break;
//            }
//
//            g_expected_event = expected_event_connect;
//            /* Commit settings to Wi-Fi module. Calling this function starts the connection process. */
//            if (A_OK != qcom_commit(g_devid))
//            {
//                break;
//            }
//
//            /* Wait for callback, that is invoked from 'driver_task' context. This callback sets g_connected to connected (1) or disconnected (0). */
//            if (pdTRUE != xSemaphoreTake(g_connect_semaph, connect_timeout))
//            {
//                break;
//            }
//
//            /* Register DHCP callback */
//            if (A_OK != qcom_dhcpc_register_cb(0, (void*)aws_dhcpc_callback))
//            {
//                break;
//            }
//
//            /* Try several attempts in worst case */
//            for (int i = 0; i < 10 && 0 == g_ip4_addr; i++)
//            {
//                /* Perform DHCP request */
//                if (A_OK != qcom_ipconfig(g_devid, QCOM_IPCONFIG_DHCP, &tmp_ip4_addr, &tmp_ip4_mask, &tmp_ip4_gw))
//                {
//                    /* Error case, terminate loops */
//                    break;
//                }
//
//                /* DHCP response is available immediately */
//                if (IPs_are_valid(tmp_ip4_addr, tmp_ip4_mask, tmp_ip4_gw))
//                {
//                    /* Valid IP of DHCP response */
//                    g_ip4_addr = tmp_ip4_addr;
//                    g_ip4_mask = tmp_ip4_mask;
//                    g_ip4_gw = tmp_ip4_gw;
//                    /* Expected case, terminate loops */
//                    break;
//                }
//
//#if defined(WIFISHIELD_IS_GT202) && (WIFISHIELD_IS == WIFISHIELD_IS_GT202)
//                /* If DHCP response is not available immediately, wait for DHCP callback */
//                if (pdTRUE == xSemaphoreTake(g_dhcp_semaph, dhcp_timeout))
//                {
//                    /* If semaphore is posted before deadline, expects valid IPs assigned in callback */
//                    /* Expected case, terminate loops */
//                    break;
//                }
//#endif
//
//#if defined(WIFISHIELD_IS_SILEX2401) && (WIFISHIELD_IS == WIFISHIELD_IS_SILEX2401)
//                /* If there is no callback response or callback does not catch the deadline, try IPCONFIG_QUERY */
//                if (A_OK != qcom_ipconfig(g_devid, QCOM_IPCONFIG_QUERY, &tmp_ip4_addr, &tmp_ip4_mask, &tmp_ip4_gw))
//                {
//                    /* Error case, terminate loops */
//                    break;
//                }
//
//                /* Check received IPCONFIG response */
//                if (IPs_are_valid(tmp_ip4_addr, tmp_ip4_mask, tmp_ip4_gw))
//                {
//                    /* Valid IP of DHCP response */
//                    g_ip4_addr = tmp_ip4_addr;
//                    g_ip4_mask = tmp_ip4_mask;
//                    g_ip4_gw = tmp_ip4_gw;
//                    /* Expected case, terminate loops */
//                    break;
//                }
//#endif
//            }
//
//            /* Still not a valid IP, report error */
//            if (!IPs_are_valid(g_ip4_addr, g_ip4_mask, g_ip4_gw))
//            {
//                break;
//            }
//            /* Otherwise after all is said and done the DHCP request is successful. */
//            else
//            {
//                ucDhcpSuccessful = 1;
//            }
//
//            /* Everything is OK. We connected to the AP and got an IP address with DHCP. */
//            status = ( g_connected && ucDhcpSuccessful ) ? eWiFiSuccess : eWiFiFailure;
//        } while (0);
//
//        /* Release semaphore */
//        xSemaphoreGive(g_wifi_semaph);
//    }
//    return status;
//}
