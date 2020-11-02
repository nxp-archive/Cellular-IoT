/*
 * Copyright 2019-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IOTC_AZURE_IOTC_UTILS_H_
#define IOTC_AZURE_IOTC_UTILS_H_

#include <stdbool.h>
#include "FreeRTOS.h"
#include "iotc_definitions.h"

void generate_sas_token(char * buffer, size_t bufferSize);
bool topic_check(const char* topic, size_t len, char* str, size_t str_len);

#endif /* IOTC_AZURE_IOTC_UTILS_H_ */
