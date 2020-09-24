/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CELLIOT_TOOLS_H_
#define CELLIOT_TOOLS_H_

#include <stdint.h>

int32_t SOCKETS_SetCfgExt(void);
int32_t SOCKETS_SetCfg(void);
int32_t SOCKETS_SetSockSecurity(void);
int32_t SOCKETS_SetTLSSecurityCfg(void);

#endif /* CELLIOT_TOOLS_H_ */
