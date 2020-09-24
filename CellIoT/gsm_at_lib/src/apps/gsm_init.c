/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "gsm_init.h"
#include "gsm.h"
#include "sim_manager.h"
#include "network_apn_settings.h"
#include "network_utils.h"

/**
 * \brief           Initialization thread
 * \param[in]       arg: Thread argument
 */
gsmr_t AT_Parser_Init(void)
{
	gsmr_t result = gsmERR;
	configPRINTF(("Starting GSM application!\r\n"));

    /* Initialize GSM with default callback function */
	result = gsm_init(gsm_callback_func, 1);
    if (result != gsmOK) {
    	configPRINTF(("Cannot initialize GSM-AT Library\r\n"));
    }

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
