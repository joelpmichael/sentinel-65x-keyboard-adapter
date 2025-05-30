/*
 * FreeRTOS Kernel V10.4.6
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#ifndef MPU_WRAPPERS_H
#define MPU_WRAPPERS_H

/* This file redefines API functions to be called through a wrapper macro, but
 * only for ports that are using the MPU. */
#if (portUSING_MPU_WRAPPERS == 1)

/* MPU_WRAPPERS_INCLUDED_FROM_API_FILE will be defined when this file is
 * included from queue.c or task.c to prevent it from having an effect within
 * those files. */
#ifndef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/*
 * Map standard (non MPU) API functions to equivalents that start
 * "MPU_".  This will cause the application code to call the MPU_
 * version, which wraps the non-MPU version with privilege promoting
 * then demoting code, so the kernel code always runs will full
 * privileges.
 */

/* Map standard tasks.h API functions to the MPU equivalents. */
#define xTaskCreate MPU_xTaskCreate
#define xTaskCreateStatic MPU_xTaskCreateStatic
#define vTaskDelete MPU_vTaskDelete
#define vTaskDelay MPU_vTaskDelay
#define xTaskDelayUntil MPU_xTaskDelayUntil
#define xTaskAbortDelay MPU_xTaskAbortDelay
#define uxTaskPriorityGet MPU_uxTaskPriorityGet
#define eTaskGetState MPU_eTaskGetState
#define vTaskGetInfo MPU_vTaskGetInfo
#define vTaskPrioritySet MPU_vTaskPrioritySet
#define vTaskSuspend MPU_vTaskSuspend
#define vTaskResume MPU_vTaskResume
#define vTaskSuspendAll MPU_vTaskSuspendAll
#define xTaskResumeAll MPU_xTaskResumeAll
#define xTaskGetTickCount MPU_xTaskGetTickCount
#define uxTaskGetNumberOfTasks MPU_uxTaskGetNumberOfTasks
#define pcTaskGetName MPU_pcTaskGetName
#define xTaskGetHandle MPU_xTaskGetHandle
#define uxTaskGetStackHighWaterMark MPU_uxTaskGetStackHighWaterMark
#define uxTaskGetStackHighWaterMark2 MPU_uxTaskGetStackHighWaterMark2
#define vTaskSetApplicationTaskTag MPU_vTaskSetApplicationTaskTag
#define xTaskGetApplicationTaskTag MPU_xTaskGetApplicationTaskTag
#define vTaskSetThreadLocalStoragePointer MPU_vTaskSetThreadLocalStoragePointer
#define pvTaskGetThreadLocalStoragePointer MPU_pvTaskGetThreadLocalStoragePointer
#define xTaskCallApplicationTaskHook MPU_xTaskCallApplicationTaskHook
#define xTaskGetIdleTaskHandle MPU_xTaskGetIdleTaskHandle
#define uxTaskGetSystemState MPU_uxTaskGetSystemState
#define vTaskList MPU_vTaskList
#define vTaskGetRunTimeStats MPU_vTaskGetRunTimeStats
#define ulTaskGetIdleRunTimeCounter MPU_ulTaskGetIdleRunTimeCounter
#define ulTaskGetIdleRunTimePercent MPU_ulTaskGetIdleRunTimePercent
#define xTaskGenericNotify MPU_xTaskGenericNotify
#define xTaskGenericNotifyWait MPU_xTaskGenericNotifyWait
#define ulTaskGenericNotifyTake MPU_ulTaskGenericNotifyTake
#define xTaskGenericNotifyStateClear MPU_xTaskGenericNotifyStateClear
#define ulTaskGenericNotifyValueClear MPU_ulTaskGenericNotifyValueClear
#define xTaskCatchUpTicks MPU_xTaskCatchUpTicks

#define xTaskGetCurrentTaskHandle MPU_xTaskGetCurrentTaskHandle
#define vTaskSetTimeOutState MPU_vTaskSetTimeOutState
#define xTaskCheckForTimeOut MPU_xTaskCheckForTimeOut
#define xTaskGetSchedulerState MPU_xTaskGetSchedulerState

/* Map standard queue.h API functions to the MPU equivalents. */
#define xQueueGenericSend MPU_xQueueGenericSend
#define xQueueReceive MPU_xQueueReceive
#define xQueuePeek MPU_xQueuePeek
#define xQueueSemaphoreTake MPU_xQueueSemaphoreTake
#define uxQueueMessagesWaiting MPU_uxQueueMessagesWaiting
#define uxQueueSpacesAvailable MPU_uxQueueSpacesAvailable
#define vQueueDelete MPU_vQueueDelete
#define xQueueCreateMutex MPU_xQueueCreateMutex
#define xQueueCreateMutexStatic MPU_xQueueCreateMutexStatic
#define xQueueCreateCountingSemaphore MPU_xQueueCreateCountingSemaphore
#define xQueueCreateCountingSemaphoreStatic MPU_xQueueCreateCountingSemaphoreStatic
#define xQueueGetMutexHolder MPU_xQueueGetMutexHolder
#define xQueueTakeMutexRecursive MPU_xQueueTakeMutexRecursive
#define xQueueGiveMutexRecursive MPU_xQueueGiveMutexRecursive
#define xQueueGenericCreate MPU_xQueueGenericCreate
#define xQueueGenericCreateStatic MPU_xQueueGenericCreateStatic
#define xQueueCreateSet MPU_xQueueCreateSet
#define xQueueAddToSet MPU_xQueueAddToSet
#define xQueueRemoveFromSet MPU_xQueueRemoveFromSet
#define xQueueSelectFromSet MPU_xQueueSelectFromSet
#define xQueueGenericReset MPU_xQueueGenericReset

#if (configQUEUE_REGISTRY_SIZE > 0)
#define vQueueAddToRegistry MPU_vQueueAddToRegistry
#define vQueueUnregisterQueue MPU_vQueueUnregisterQueue
#define pcQueueGetName MPU_pcQueueGetName
#endif

/* Map standard timer.h API functions to the MPU equivalents. */
#define xTimerCreate MPU_xTimerCreate
#define xTimerCreateStatic MPU_xTimerCreateStatic
#define pvTimerGetTimerID MPU_pvTimerGetTimerID
#define vTimerSetTimerID MPU_vTimerSetTimerID
#define xTimerIsTimerActive MPU_xTimerIsTimerActive
#define xTimerGetTimerDaemonTaskHandle MPU_xTimerGetTimerDaemonTaskHandle
#define xTimerPendFunctionCall MPU_xTimerPendFunctionCall
#define pcTimerGetName MPU_pcTimerGetName
#define vTimerSetReloadMode MPU_vTimerSetReloadMode
#define uxTimerGetReloadMode MPU_uxTimerGetReloadMode
#define xTimerGetPeriod MPU_xTimerGetPeriod
#define xTimerGetExpiryTime MPU_xTimerGetExpiryTime
#define xTimerGenericCommand MPU_xTimerGenericCommand

/* Map standard event_group.h API functions to the MPU equivalents. */
#define xEventGroupCreate MPU_xEventGroupCreate
#define xEventGroupCreateStatic MPU_xEventGroupCreateStatic
#define xEventGroupWaitBits MPU_xEventGroupWaitBits
#define xEventGroupClearBits MPU_xEventGroupClearBits
#define xEventGroupSetBits MPU_xEventGroupSetBits
#define xEventGroupSync MPU_xEventGroupSync
#define vEventGroupDelete MPU_vEventGroupDelete

/* Map standard message/stream_buffer.h API functions to the MPU
 * equivalents. */
#define xStreamBufferSend MPU_xStreamBufferSend
#define xStreamBufferReceive MPU_xStreamBufferReceive
#define xStreamBufferNextMessageLengthBytes MPU_xStreamBufferNextMessageLengthBytes
#define vStreamBufferDelete MPU_vStreamBufferDelete
#define xStreamBufferIsFull MPU_xStreamBufferIsFull
#define xStreamBufferIsEmpty MPU_xStreamBufferIsEmpty
#define xStreamBufferReset MPU_xStreamBufferReset
#define xStreamBufferSpacesAvailable MPU_xStreamBufferSpacesAvailable
#define xStreamBufferBytesAvailable MPU_xStreamBufferBytesAvailable
#define xStreamBufferSetTriggerLevel MPU_xStreamBufferSetTriggerLevel
#define xStreamBufferGenericCreate MPU_xStreamBufferGenericCreate
#define xStreamBufferGenericCreateStatic MPU_xStreamBufferGenericCreateStatic


/* Remove the privileged function macro, but keep the PRIVILEGED_DATA
 * macro so applications can place data in privileged access sections
 * (useful when using statically allocated objects). */
#define PRIVILEGED_FUNCTION
#define PRIVILEGED_DATA __attribute__ ((section ("privileged_data")))
#define FREERTOS_SYSTEM_CALL

#else /* MPU_WRAPPERS_INCLUDED_FROM_API_FILE */

/* Ensure API functions go in the privileged execution section. */
#define PRIVILEGED_FUNCTION __attribute__ ((section ("privileged_functions")))
#define PRIVILEGED_DATA __attribute__ ((section ("privileged_data")))
#define FREERTOS_SYSTEM_CALL __attribute__ ((section ("freertos_system_calls")))

/**
 * @brief Calls the port specific code to raise the privilege.
 *
 * Sets xRunningPrivileged to pdFALSE if privilege was raised, else sets
 * it to pdTRUE.
 */
#define xPortRaisePrivilege(xRunningPrivileged)                            \
    {                                                                      \
        /* Check whether the processor is already privileged. */           \
        xRunningPrivileged = portIS_PRIVILEGED();                          \
                                                                           \
        /* If the processor is not already privileged, raise privilege. */ \
        if (xRunningPrivileged == pdFALSE) {                               \
            portRAISE_PRIVILEGE();                                         \
        }                                                                  \
    }

/**
 * @brief If xRunningPrivileged is not pdTRUE, calls the port specific
 * code to reset the privilege, otherwise does nothing.
 */
#define vPortResetPrivilege(xRunningPrivileged) \
    {                                           \
        if (xRunningPrivileged == pdFALSE) {    \
            portRESET_PRIVILEGE();              \
        }                                       \
    }

#endif /* MPU_WRAPPERS_INCLUDED_FROM_API_FILE */

#else  /* portUSING_MPU_WRAPPERS */

#define PRIVILEGED_FUNCTION
#define PRIVILEGED_DATA
#define FREERTOS_SYSTEM_CALL

#endif /* portUSING_MPU_WRAPPERS */


#endif /* MPU_WRAPPERS_H */
