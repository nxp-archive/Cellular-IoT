/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "CellIoT_tools.h"
#include "CellIoT_lib.h"
#include "CellIoT_types.h"


int32_t SOCKETS_SetCfgExt(void)
{
	return CellIoT_lib_setSocketCfgExt(gsm.m.conn_val_id + 1, 1, 1, 0, 1, 1, NULL, NULL, 1);
}

int32_t SOCKETS_SetCfg(void)
{
#ifdef USE_TRUPHONE
	return CellIoT_lib_setSocketCfg(gsm.m.conn_val_id + 1, 1, 1450, 0, 600, 50, 1);
#else
	/* PDP=3 is used for Verizon network! */
	return CellIoT_lib_setSocketCfg(gsm.m.conn_val_id + 1, 3, 1450, 0, 600, 50, 1);
#endif
}

int32_t SOCKETS_SetSockSecurity(void)
{
    return CellIoT_lib_setSocketSecurity(gsm.m.conn_val_id + 1, gsm.m.conn_val_id + 1, 0, NULL, NULL, 1);
}

int32_t SOCKETS_SetTLSSecurityCfg(void)
{
	return CellIoT_lib_setTLSSecurityProfileCfg(gsm.m.conn_val_id + 1, 2, "0x3C", 0, 0, 0, 0, NULL, NULL, NULL, 1);
}
