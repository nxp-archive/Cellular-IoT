/*
 * gsm_init.c
 *
 *  Created on: 7 janv. 2020
 *      Author: nxf37702
 */

#include "gsm/apps/gsm_init.h"
#include "gsm/gsm.h"
#include "sim_manager.h"
#include "network_apn_settings.h"
#include "network_utils.h"

/**
 * \brief           Initialization thread
 * \param[in]       arg: Thread argument
 */
gsmr_t AT_Parser_Init(void)
{
	gsmr_t result;
	configPRINTF(("Starting GSM application!\r\n"));

    /* Initialize GSM with default callback function */
    if (gsm_init(gsm_callback_func, 1) != gsmOK) {
    	configPRINTF(("Cannot initialize GSM-AT Library\r\n"));
    }

//    /* Configure device by unlocking SIM card */
//    if (configure_sim_card()) {
//    	configPRINTF(("SIM card configured. Adding delay to stabilize SIM card.\r\n"));
//        gsm_delay(10000);
//    } else {
//    	configPRINTF(("Cannot configure SIM card! Is it inserted, pin valid and not under PUK? Closing down...\r\n"));
//        while (1) { gsm_delay(1000); }
//    }
//
//    while (1) {
//        /* Connect to network for TCP/IP */
//        if (gsm_network_attach(NETWORK_APN, NETWORK_APN_USER, NETWORK_APN_PASS, NULL, NULL, 1) == gsmOK) {
//            printf("Attached to network!\r\n");
//
//            /* Start netconn thread */
//            gsm_sys_thread_create(NULL, "mqtt_thread", (gsm_sys_thread_fn)mqtt_client_api_thread, NULL, GSM_SYS_THREAD_SS, GSM_SYS_THREAD_PRIO);
//            break;
//        } else {
//            printf("Cannot attach to network!\r\n");
//            gsm_delay(1000);
//        }
//    }
//
//    while (1) {
//        gsm_delay(1000);
//    }
//
//    osThreadTerminate(NULL);                    /* Terminate current thread */
    result = gsmOK;
    return result;
}

/**
 * \brief           Event callback function for GSM stack
 * \param[in]       evt: Event information with data
 * \return          \ref gsmOK on success, member of \ref gsmr_t otherwise
 */
gsmr_t gsm_callback_func(gsm_evt_t* evt)
{
    switch (gsm_evt_get_type(evt)) {
        case GSM_EVT_INIT_FINISH: configPRINTF(("Library initialized!\r\n")); break;
        /* Process and print registration change */
        case GSM_EVT_NETWORK_REG_CHANGED: network_utils_process_reg_change(evt); break;
        /* Process current network operator */
        case GSM_EVT_NETWORK_OPERATOR_CURRENT: network_utils_process_curr_operator(evt); break;
        /* Process signal strength */
        case GSM_EVT_SIGNAL_STRENGTH: network_utils_process_rssi(evt); break;

        /* Other user events here... */

        default: break;
    }
    return gsmOK;
}
