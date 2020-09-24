/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef GSM_HDR_SYSTEM_H
#define GSM_HDR_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <stdlib.h>

#include "gsm_config.h"

/**
 * \addtogroup      GSM_SYS
 * \{
 */

#if GSM_CFG_OS || __DOXYGEN__
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include "projdefs.h"

/**
 * \brief           GSM system mutex ID type - Mutex Handle structure
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
typedef SemaphoreHandle_t           gsm_sys_mutex_t;

/**
 * \brief           GSM system semaphore ID type - Semaphore Handle structure
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
typedef SemaphoreHandle_t			gsm_sys_sem_t;

/**
 * \brief           GSM system message queue ID type - Queue Handle structure
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
typedef QueueHandle_t        		gsm_sys_mbox_t;

/**
 * \brief           GSM system thread ID type - Task Handle structure
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
typedef TaskHandle_t          		gsm_sys_thread_t;

/**
 * \brief           GSM system thread priority type
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
typedef UBaseType_t          		gsm_sys_thread_prio_t;

/**
 * \brief           Value indicating message queue is not valid
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
#define GSM_SYS_MBOX_NULL           (QueueHandle_t)0

/**
 * \brief           Value indicating semaphore is not valid
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
#define GSM_SYS_SEM_NULL            (SemaphoreHandle_t)0

/**
 * \brief           Value indicating mutex is not valid
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
#define GSM_SYS_MUTEX_NULL          (SemaphoreHandle_t)0

/**
 * \brief           Value indicating timeout for OS timings - Value chosen by default, can be modified
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
#define GSM_SYS_TIMEOUT             (pdFALSE)

/**
 * \brief           GSM stack threads priority parameter - Value chosen by default, can be modified
 * \note            Usually normal priority is ok. If many threads are in the system and high traffic is introduced
 *                  This value might need to be set to higher value
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
#define GSM_SYS_THREAD_PRIO         (5U)

/**
 * \brief           Stack size of system threads
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
#define GSM_SYS_THREAD_SS           (1024U)
#endif /* GSM_CFG_OS || __DOXYGEN__ */

/**
 * \}
 */

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* GSM_HDR_SYSTEM_H */
