/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef GSM_AT_LIB_SRC_APPS_GSM_INIT_H_
#define GSM_AT_LIB_SRC_APPS_GSM_INIT_H_

#include "gsm/gsm_typedefs.h"

gsmr_t AT_Parser_Init(void);
gsmr_t gsm_callback_func(gsm_evt_t* evt);

#endif /* GSM_AT_LIB_SRC_APPS_GSM_INIT_H_ */
