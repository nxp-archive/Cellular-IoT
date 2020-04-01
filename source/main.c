/*
 * Amazon FreeRTOS V1.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

///////////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////////
/* SDK Included Files */
#include "board.h"
#include "fsl_debug_console.h"
#include "ksdk_mbedtls.h"

#include "pin_mux.h"

/* Amazon FreeRTOS Demo Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "aws_logging_task.h"
#include "aws_system_init.h"
#include "aws_dev_mode_key_provisioning.h"

/* Board specific accelerometer driver include */
#if defined(BOARD_ACCEL_FXOS)
#include "fsl_fxos.h"
#elif defined(BOARD_ACCEL_MMA)
#include "fsl_mma.h"
#endif

#include "aws_clientcredential.h"
#include "aws_wifi.h"
#include "aws_nbiot.h"
#include "clock_config.h"
#include "gm01q_api.h"
#include "gsm/apps/gsm_init.h"
#include "gsm/gsm_includes.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define INIT_SUCCESS 0
#define INIT_FAIL 1

#define LOGGING_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define LOGGING_TASK_STACK_SIZE (200)
#define LOGGING_QUEUE_LENGTH (16)

/* Accelerometer driver specific defines */
#if defined(BOARD_ACCEL_FXOS)
#define XYZ_DATA_CFG XYZ_DATA_CFG_REG
#define ACCEL_INIT(handle, config) FXOS_Init(handle, config)
#define ACCEL_READ_REG(handle, reg, val) FXOS_ReadReg(handle, reg, val, 1)
#define ACCELL_READ_SENSOR_DATA(handle, data) FXOS_ReadSensorData(handle, data)
#define ACCEL_GET_RESOLUTION() FXOS_GetResolutionBits()
#elif defined(BOARD_ACCEL_MMA)
#define XYZ_DATA_CFG kMMA8652_XYZ_DATA_CFG
#define ACCEL_INIT(handle, config) MMA_Init(handle, config)
#define ACCEL_READ_REG(handle, reg, val) MMA_ReadReg(handle, reg, val)
#define ACCELL_READ_SENSOR_DATA(handle, data) MMA_ReadSensorData(handle, data)
#define ACCEL_GET_RESOLUTION() MMA_GetResolutionBits()
#endif

/* Accelerometer and magnetometer */
#if defined(BOARD_ACCEL_FXOS)
fxos_handle_t accelHandle           = {0};
static const uint8_t accelAddress[] = {0x1CU, 0x1EU, 0x1DU, 0x1FU};
fxos_config_t config                = {0};
#elif defined(BOARD_ACCEL_MMA)
mma_handle_t accelHandle            = {0};
static const uint8_t accelAddress[] = {0x1CU, 0x1DU, 0x1EU, 0x1FU};
mma_config_t config                 = {0};
#endif

/* Accelerometer data scale */
uint8_t g_accelDataScale = 0;
/* Resolution of accelerometer (14 bit or 12 bit) */
uint8_t g_accelResolution = 0;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
extern void vStartLedDemoTask(void);
extern int initNetwork(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
const WIFINetworkParams_t pxNetworkParams = {
    .pcSSID           = clientcredentialWIFI_SSID,
    .ucSSIDLength     = sizeof(clientcredentialWIFI_SSID) - 1,
    .pcPassword       = clientcredentialWIFI_PASSWORD,
    .ucPasswordLength = sizeof(clientcredentialWIFI_PASSWORD) - 1,
    .xSecurity        = clientcredentialWIFI_SECURITY,
};
/* Count of LED */
uint8_t ledCount = 3;
/* Array of LED names */
const char *ledName[] = {"LED RED", "LED GREEN", "LED BLUE"};
/* Array of LED colors in JSON */
char ledColors[] = "[\"red\", \"green\", \"blue\"]";

/*******************************************************************************
 * Code
 ******************************************************************************/
int initNetwork(void)
{
#if (CONNECTIVITY_SUPPORT==1)
    WIFIReturnCode_t result;

    configPRINTF(("Starting WiFi...\r\n"));

    result = WIFI_On();
    if (result != eWiFiSuccess)
    {
        configPRINTF(("Could not enable WiFi, reason %d.\r\n", result));
        return INIT_FAIL;
    }

    configPRINTF(("WiFi module initialized.\r\n"));

    result = WIFI_ConnectAP(&pxNetworkParams);
    if (result != eWiFiSuccess)
    {
        configPRINTF(("Could not connect to WiFi, reason %d.\r\n", result));
        return INIT_FAIL;
    }

    configPRINTF(("WiFi connected to AP %s.\r\n", pxNetworkParams.pcSSID));

    uint8_t tmp_ip[4] = {0};
    result            = WIFI_GetIP(tmp_ip);

    if (result != eWiFiSuccess)
    {
        configPRINTF(("Could not get IP address, reason %d.\r\n", result));
        return INIT_FAIL;
    }

    configPRINTF(("IP Address acquired %d.%d.%d.%d\r\n", tmp_ip[0], tmp_ip[1], tmp_ip[2], tmp_ip[3]));

    return INIT_SUCCESS;
#elif (CONNECTIVITY_SUPPORT==2)
    uint8_t result;
    /* Previous UART initialization before the
     * integration of the AT Parser Library
     */
//	configPRINTF(("Starting NB-IoT...\r\n"));
//
//	result = (uint8_t)NBIOT_On();
//	if (result != eNbIoTSuccess)
//	{
//		configPRINTF(("Could not enable NB-IoT, reason %d.\r\n", result));
//		return INIT_FAIL;
//	}
//
//	configPRINTF(("NB-IoT module initialized.\r\n"));

    /*
     * Initialization of the AT Parser Library
     * It consists of:
     * - Creation of AT Command produce task (gsm_thread_produce) and its associated queue
     * - Creation of AT Command process task (gsm_thread_process) and its associated queue
     * - Configuration of the low-level layers (Pins config, USART, DMA)
     * - Creation of the USART Receiving task (usart_ll_thread)
     * - Reseting the NB-IoT module
     */
	result = (uint8_t)AT_Parser_Init();
	if (result != gsmOK)
	{
		configPRINTF(("Could not initialize the AT Parser library, reason %d.\r\n", result));
		return INIT_FAIL;
	}

	configPRINTF(("AT Parser library initialized.\r\n"));

	configPRINTF(("Wait to be connected to the network...\r\n"));

	while(
			( gsm_network_get_reg_status() != GSM_NETWORK_REG_STATUS_CONNECTED )
			&&
			( gsm_network_get_reg_status() != GSM_NETWORK_REG_STATUS_CONNECTED_ROAMING)
	)
	{
		vTaskDelay(pdMS_TO_TICKS(100));
	}
	/* Do not enable this section, the certificates and private keys are already stored in the NB-IoT module */
	/* The issue is that we get a +CME ERROR: 4 when sending one of these commands most of the time */
#if 0
	configPRINTF(("Erase CA certificate / client certificate / private key in the NB-IoT module\r\n"));
	gsmr_t xResult;
	xResult = gm01q_api_WriteCertKeyInNVM(NULL, SQNS_MQTT_CERTIFICATE, 0, 0, NULL, NULL, 1);
	PRINTF("xResult:%d\n",xResult);
	xResult = gm01q_api_WriteCertKeyInNVM(NULL, SQNS_MQTT_CERTIFICATE, 1, 0, NULL, NULL, 1 );
	PRINTF("xResult:%d\n",xResult);
	xResult = gm01q_api_WriteCertKeyInNVM(NULL, SQNS_MQTT_PRIVATEKEY,  2, 0, NULL, NULL, 1 );
	PRINTF("xResult:%d\n",xResult);

	configPRINTF(("Send the CA certificate / client certificate / private key to the NB-IoT module\r\n"));
	xResult = gm01q_api_WriteCertKeyInNVM(keyCERTIFICATE_AUTHORITY_PEM, SQNS_MQTT_CERTIFICATE, 0, sizeof( keyCERTIFICATE_AUTHORITY_PEM ) - 1, NULL, NULL, 1);
	PRINTF("xResult:%d\n",xResult);
	xResult = gm01q_api_WriteCertKeyInNVM(keyCLIENT_CERTIFICATE_PEM, SQNS_MQTT_CERTIFICATE, 1, sizeof( keyCLIENT_CERTIFICATE_PEM ) - 1, NULL, NULL, 1 );
	PRINTF("xResult:%d\n",xResult);
	xResult = gm01q_api_WriteCertKeyInNVM(keyCLIENT_PRIVATE_KEY_PEM, SQNS_MQTT_PRIVATEKEY, 2, sizeof( keyCLIENT_PRIVATE_KEY_PEM ) - 1, NULL, NULL, 1 );
	PRINTF("xResult:%d\n",xResult);
#endif
	gm01q_api_setLogInModule();
	gm01q_api_readConfTestMode();

/*
	result = WIFI_ConnectAP(&pxNetworkParams);
	if (result != eWiFiSuccess)
	{
		configPRINTF(("Could not connect to WiFi, reason %d.\r\n", result));
		return INIT_FAIL;
	}

	configPRINTF(("WiFi connected to AP %s.\r\n", pxNetworkParams.pcSSID));

	uint8_t tmp_ip[4] = {0};
	result            = WIFI_GetIP(tmp_ip);

	if (result != eWiFiSuccess)
	{
		configPRINTF(("Could not get IP address, reason %d.\r\n", result));
		return INIT_FAIL;
	}

	configPRINTF(("IP Address acquired %d.%d.%d.%d\r\n", tmp_ip[0], tmp_ip[1], tmp_ip[2], tmp_ip[3]));
*/
	return INIT_SUCCESS;
#endif
}
void turnOnLed(uint8_t id)
{
    if (id == 0)
    {
        LED_RED_ON();
    }
    if (id == 1)
	{
		LED_GREEN_ON();
	}
    if (id == 2)
	{
		LED_BLUE_ON();
	}
}

void turnOffLed(uint8_t id)
{
    if (id == 0)
    {
        LED_RED_OFF();
    }
    if (id == 1)
	{
		LED_GREEN_OFF();
	}
	if (id == 2)
	{
		LED_BLUE_OFF();
	}
}

/*!
 * @brief Initialize pins for control LEDs
 */
void BOARD_InitLEDs()
{
    LED_RED_INIT(LOGIC_LED_OFF);
    LED_GREEN_INIT(LOGIC_LED_OFF);
    LED_BLUE_INIT(LOGIC_LED_OFF);
}

void print_string(const char *string)
{
    PRINTF(string);
}

#if defined(BOARD_ACCEL_FXOS) || defined(BOARD_ACCEL_MMA)
/*!
 * @brief Initialize accelerometer sensor
 */
status_t init_mag_accel(uint8_t *accelDataScale, uint8_t *accelResolution)
{
    uint8_t arrayAddrSize = 0;
    uint8_t sensorRange   = 0;
    uint16_t i            = 0;
    status_t result       = kStatus_Fail;

    /* Configure the I2C function */
    config.I2C_SendFunc    = BOARD_Accel_I2C_Send;
    config.I2C_ReceiveFunc = BOARD_Accel_I2C_Receive;

    /* Initialize sensor devices */
    arrayAddrSize = sizeof(accelAddress) / sizeof(accelAddress[0]);
    for (i = 0; i < arrayAddrSize; i++)
    {
        config.slaveAddress = accelAddress[i];
        /* Initialize accelerometer sensor */
        result = ACCEL_INIT(&accelHandle, &config);
        if (result == kStatus_Success)
        {
            break;
        }
    }

    if (result != kStatus_Success)
    {
        PRINTF("\r\nSensor device initialize failed!\r\n");
        PRINTF("\r\nPlease check the sensor chip\r\n");
        return result;
    }

    *accelResolution = ACCEL_GET_RESOLUTION();

    /* Get sensor range */
    if (kStatus_Success != ACCEL_READ_REG(&accelHandle, XYZ_DATA_CFG, &sensorRange))
    {
        return kStatus_Fail;
    }
    if (sensorRange == 0x00)
    {
        *accelDataScale = 2U;
    }
    else if (sensorRange == 0x01)
    {
        *accelDataScale = 4U;
    }
    else if (sensorRange == 0x10)
    {
        *accelDataScale = 8U;
    }

    return kStatus_Success;
}
#endif

void vApplicationDaemonTaskStartupHook(void)
{
    /* A simple example to demonstrate key and certificate provisioning in
     * microcontroller flash using PKCS#11 interface. This should be replaced
     * by production ready key provisioning mechanism. */

	/* Alex's Comment
	 * Don't forget to change the certificate to use
	 * the one specified by Sequans to be properly
	 * connected to the AWS Cloud using NB-IoT
	 * Refer to the following email exchange to understand
	 *
	 * Hello Alexandre,
	 *
	 * About the following action:
	 * �	Am�lie to get more information on where the certificate that enabled connecting to AWS cloud comes from
	 *
	 * Actually, we found the right certificate to connect to AWS by doing a quick google search. We found the following article about AWS moving it root authority:
	 * https://aws.amazon.com/blogs/security/how-to-prepare-for-aws-move-to-its-own-certificate-authority/
	 *
	 * In the article they provide a link to download the new root authority:
	 * �	Note: as far as we understand, Amazon doesn�t own this root and doesn�t have a test URL for it. The certificate can be downloaded from here. (LINK INFO: https://certs.secureserver.net/repository/sf-class2-root.crt)
	 * �	Note also that the Common Name on the cert from the link they provide matches Starfield Class 2 Certification Authority. This is a good indicator that you should have the right cert (ultimately it comes down to which private key was used to create the cert and sign the chain) � you need to check that you have this Name in your .crt (and not something like Amazon Root CA 1 which would not match � that�s what you had in the past and blocked you).
	 *
	 */
	vDevModeKeyProvisioning();

    /*
     * Initialize
     *  - Buffer pools
     *  - MQTT: generation of the MQTT Task (prvMQTTTask)
     *  - Socket
     */
    if (SYSTEM_Init() == pdPASS)
    {
        if (initNetwork() != 0)
        {
            configPRINTF(("Network init failed, stopping demo.\r\n"));
            vTaskDelete(NULL);
        }
        else
        {
            vStartLedDemoTask();
        }
    }
}

int main(void)
{
    /* attach main clock divide to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    /* attach 12 MHz clock to FLEXCOMM4 (I2C master) */
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM4);

    /* reset FLEXCOMM for I2C */
    RESET_PeripheralReset(kFC4_RST_SHIFT_RSTn);

    BOARD_InitPins();
    BOARD_BootClockFROHF96M();
    BOARD_InitDebugConsole();

    BOARD_InitLEDs();
    CRYPTO_InitHardware();

#if defined(BOARD_ACCEL_FXOS) || defined(BOARD_ACCEL_MMA)
    /* Initialize I2C */
    BOARD_Accel_I2C_Init();

    /* Initialize magnetometer and accelerometer */
    if (kStatus_Success != init_mag_accel(&g_accelDataScale, &g_accelResolution))
    {
        /* Failed to initialize accelerometer */
        for (;;)
            ;
    }
#endif

    xLoggingTaskInitialize(LOGGING_TASK_STACK_SIZE, LOGGING_TASK_PRIORITY, LOGGING_QUEUE_LENGTH);

    vTaskStartScheduler();
    for (;;)
        ;
}

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
 * used by the Idle task. */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
    /* If the buffers to be provided to the Idle task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
 * application must provide an implementation of vApplicationGetTimerTaskMemory()
 * to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize)
{
    /* If the buffers to be provided to the Timer task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
     * task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
/**
 * @brief Warn user if pvPortMalloc fails.
 *
 * Called if a call to pvPortMalloc() fails because there is insufficient
 * free memory available in the FreeRTOS heap.  pvPortMalloc() is called
 * internally by FreeRTOS API functions that create tasks, queues, software
 * timers, and semaphores.  The size of the FreeRTOS heap is set by the
 * configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h.
 *
 */
void vApplicationMallocFailedHook()
{
    configPRINTF(("ERROR: Malloc failed to allocate memory\r\n"));
}

/**
 * @brief Loop forever if stack overflow is detected.
 *
 * If configCHECK_FOR_STACK_OVERFLOW is set to 1,
 * this hook provides a location for applications to
 * define a response to a stack overflow.
 *
 * Use this hook to help identify that a stack overflow
 * has occurred.
 *
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    portDISABLE_INTERRUPTS();

    /* Loop forever */
    for (;;)
        ;
}

void *pvPortCalloc(size_t xNum, size_t xSize)
{
    void *pvReturn;

    pvReturn = pvPortMalloc(xNum * xSize);
    if (pvReturn != NULL)
    {
        memset(pvReturn, 0x00, xNum * xSize);
    }

    return pvReturn;
}
