/*
 * Copyright 2016 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "nbiot_common.h"

/*TODO: check return value */
void nbiot_free(void *addr)
{
    vPortFree(addr);
}

/* FIXME !! */
//extern uint32_t g_totAlloc;
void *nbiot_malloc(int32_t size)
{
    void *addr;

    addr = (void *)pvPortMalloc(size);
    assert(addr);
    if (addr != NULL)
    {
        /*FIXME: !!!*/
      //  g_totAlloc += size;
    }

    return addr;
}

SQNS_STATUS nbiot_mutex_init(xSemaphoreHandle *pMutex)
{
    assert(pMutex);
    *pMutex = xSemaphoreCreateMutex();
    if (NULL == *pMutex)
    {
        return SQNS_ERROR;
    }
    else
    {
        return SQNS_SUCCESS;
    }
}

SQNS_STATUS nbiot_mutex_acquire(xSemaphoreHandle *pMutex)
{
    assert(pMutex);
    if (xSemaphoreGetMutexHolder(*pMutex) == xTaskGetCurrentTaskHandle())
    {
        return SQNS_ERROR;
    }
    if (xSemaphoreTake(*pMutex, portMAX_DELAY) == pdFALSE)
    {
        return SQNS_ERROR;
    }
    else
    {
        return SQNS_SUCCESS;
    }
}

SQNS_STATUS nbiot_mutex_release(xSemaphoreHandle *pMutex)
{
    assert(pMutex);
    if (xSemaphoreGetMutexHolder(*pMutex) != xTaskGetCurrentTaskHandle())
    {
        assert(0); // TODO: use assert cause no-one check return value
        return SQNS_ERROR;
    }
    if (xSemaphoreGive(*pMutex) == pdPASS)
    {
        return SQNS_SUCCESS;
    }
    else
    {
        assert(0); // TODO: use assert cause no-one check return value
        return SQNS_ERROR;
    }
}

bool nbiot_is_mutex_valid(xSemaphoreHandle *pMutex)
{
    // FIXME: check owner of mutex
    return true;
}

SQNS_STATUS nbiot_mutex_delete(xSemaphoreHandle *pMutex)
{
    assert(pMutex);
    assert(*pMutex);

    vSemaphoreDelete(*pMutex);
    return SQNS_SUCCESS;
}

SQNS_STATUS nbiot_event_delete(event_t *pEvent)
{
    assert(pEvent);
    vEventGroupDelete(pEvent->eventHandler);
    return SQNS_SUCCESS;
}

// zrusit autoclear flag !!!
SQNS_STATUS nbiot_event_init(event_t *pEvent, osa_event_clear_mode_t clearMode)
{
    assert(pEvent);

    pEvent->eventHandler = xEventGroupCreate();

    if (pEvent->eventHandler)
    {
        pEvent->clearMode = clearMode;
        return SQNS_SUCCESS;
    }
    else
    {
        return SQNS_ERROR;
    }
}

SQNS_STATUS nbiot_event_clear(event_t *pEvent, EventBits_t flagsToClear)
{
    assert(pEvent);

    if (__get_IPSR())
    {
        xEventGroupClearBitsFromISR(pEvent->eventHandler, flagsToClear);
    }
    else
    {
        xEventGroupClearBits(pEvent->eventHandler, flagsToClear);
    }

    return SQNS_SUCCESS;
}

SQNS_STATUS nbiot_event_set(event_t *pEvent, EventBits_t flagsToSet)
{
    assert(pEvent);
    portBASE_TYPE xResult, taskToWake = pdFALSE;

    if (__get_IPSR())
    {
        xResult = xEventGroupSetBitsFromISR(pEvent->eventHandler, flagsToSet, &taskToWake);
        if (xResult != pdFAIL)
        {
            portYIELD_FROM_ISR(taskToWake);
        }
    }
    else
    {
        xEventGroupSetBits(pEvent->eventHandler, flagsToSet);
    }
    return SQNS_SUCCESS;
}

SQNS_STATUS nbiot_event_wait(
    event_t *pEvent, EventBits_t flagsToWait, bool waitAll, uint32_t timeout, EventBits_t *setFlags)
{
    assert(pEvent);
    BaseType_t clearMode;
    uint32_t timeoutTicks;
    EventBits_t flagsSave;

    /* Convert timeout from millisecond to tick. */
    if (timeout == 0xFFFFFFFFU) // OSA_WAIT_FOREVER)
    {
        timeoutTicks = portMAX_DELAY;
    }
    else
    {
        timeoutTicks = timeout / portTICK_PERIOD_MS;
    }

    clearMode = (kEventAutoClear == pEvent->clearMode) ? pdTRUE : pdFALSE;

    flagsSave = xEventGroupWaitBits(pEvent->eventHandler, flagsToWait, clearMode, (BaseType_t)waitAll, timeoutTicks);

    *setFlags = flagsSave & flagsToWait;
    // TODO DIFF: timeout ??
    if (*setFlags)
    {
        return SQNS_SUCCESS;
    }
    else
    {
        return SQNS_ERROR; // TODO: unify with caller
    }
}

uint32_t nbiot_time_get_msec(void)
{
    portTickType ticks;

    if (__get_IPSR())
    {
        ticks = xTaskGetTickCountFromISR();
    }
    else
    {
        ticks = xTaskGetTickCount();
    }

    return TICKS_TO_MSEC(ticks);
}

#define OSA_MAX_ISR_CRITICAL_SECTION_DEPTH 8

/* TODO: replace with semaphore */
static uint32_t g_base_priority_array[OSA_MAX_ISR_CRITICAL_SECTION_DEPTH];
static int32_t g_base_priority_top = 0;

void OS_nbiot_EnterCritical(osa_critical_section_mode_t mode)
{
    if (kCriticalDisableInt == mode)
    {
        if (__get_IPSR())
        {
            assert(g_base_priority_top < OSA_MAX_ISR_CRITICAL_SECTION_DEPTH);

            g_base_priority_array[g_base_priority_top] = portSET_INTERRUPT_MASK_FROM_ISR();
            g_base_priority_top++;
        }
        else
        {
            portENTER_CRITICAL();
        }
    }
    else
    {
        vTaskSuspendAll();
    }
}

void OS_nbiot_ExitCritical(osa_critical_section_mode_t mode)
{
    if (kCriticalDisableInt == mode)
    {
        if (__get_IPSR())
        {
            g_base_priority_top--;
            assert(g_base_priority_top >= 0);

            portCLEAR_INTERRUPT_MASK_FROM_ISR(g_base_priority_array[g_base_priority_top]);
        }
        else
        {
            portEXIT_CRITICAL();
        }
    }
    else
    {
        xTaskResumeAll();
    }
}


