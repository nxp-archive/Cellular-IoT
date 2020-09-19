/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SHIELDS_CELLIOT_COMMON_H_
#define SHIELDS_CELLIOT_COMMON_H_

extern void *p_Global_Context;

/* porting macros only,
 * redirected to env, board fn */
//#include "CellIoT_port.h"

/* enviroment related code */
#include "CellIoT_env.h"

/* board related code  */
#include "CellIoT_shield.h"

/* General structures for Sequans modules */
#include "sqns_structs.h"

#endif /* SHIELDS_CELLIOT_COMMON_H_ */
