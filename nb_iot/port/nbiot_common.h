/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SHIELDS_NBIOT_COMMON_H_
#define SHIELDS_NBIOT_COMMON_H_

extern void *p_Global_Context;

/* porting macros only,
 * redirected to env, board fn */
//#include "nbiot_port.h"

/* enviroment related code */
#include "nbiot_env.h"

/* board related code  */
#include "nbiot_shield.h"

/* General structures for Sequans modules */
#include "sqns_structs.h"

#endif /* SHIELDS_NBIOT_COMMON_H_ */
