//*****************************************************************************
// Audio Microsoft AZure IoT Implementation
//
// Author: nxf52380
//*****************************************************************************
//
// Copyright 2016-2020 NXP
// All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause
//*****************************************************************************

#ifndef MSFT_AZURE_IOT_H_
#define MSFT_AZURE_IOT_H_

#include "FreeRTOSConfig.h"
#include "fsl_debug_console.h"
/* Stack size for task that handles shadow delta and updates. */
#define AzureTwin_DemoUPDATE_TASK_STACK_SIZE     ( ( uint16_t ) configMINIMAL_STACK_SIZE * ( uint16_t ) 20 )

/* Create the shadow demo main task which will act as a client application to
 * request periodic change in state (color) of light bulb.  */

#define AZURE_PRINTF(x)		vLoggingPrintf x

void prvmcsft_Azure_TwinTask( void * pvParameters );
void vStartAzureLedDemoTask( void );

#endif /* MSFT_AZURE_IOT_H_ */
