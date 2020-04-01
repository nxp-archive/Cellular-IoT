/*
 * nbiot_common.h
 *
 *  Created on: 22 oct. 2019
 *      Author: nxf37702
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
