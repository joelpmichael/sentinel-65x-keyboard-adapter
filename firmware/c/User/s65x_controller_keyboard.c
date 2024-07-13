/*
 * s65x_controller_keyboard.c
 *
 *  Created on: 7 Jul 2024
 *      Author: joel
 */

#include "s65x_controller_keyboard.h"

#include "custom_host.h"
#include "custom_keyboard.h"
#include "ps2_host.h"
#include "ps2_host_keyboard.h"
#include "usb_host.h"
#include "usb_host_keyboard.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#ifdef HAS_CONTROLLER_KEYBOARD
#ifndef HAS_USB_KEYBOARD
#ifndef HAS_PS2_KEYBOARD
#ifndef HAS_CUSTOM_KEYBOARD
#error At least one of -DHAS_USB_KEYBOARD, -DHAS_PS2_KEYBOARD, or -DHAS_CUSTOM_KEYBOARD must be enabled
#endif
#endif
#endif
#endif

#define KEYBOARD_QUEUE_LEN 16
QueueHandle_t s65x_keyboard_output_queue = NULL;

TaskHandle_t s65x_keyboard_task_handle = NULL;

// fetch next keyboard word to be transmitted on the SNES controller port
bool s65x_keyboard_get_next_word(uint16_t *next_word) {
    // if the queue hasn't been created, just return false
    // because there's definitely no data to retrieve yet
    if (s65x_keyboard_output_queue == NULL)
        return false;

    // receive into a temporary variable, without blocking
    // if there's no data in the queue, don't load the next word
    // the queue entry is sent as a s65x_keyboard_t, but received
    // as a uint16_t for transmission
    uint16_t temp;
    if (xQueueReceive(s65x_keyboard_output_queue, &temp, 0) == pdFALSE)
        return false;

    // ensure interrupts are disabled during load of *next_word
    taskENTER_CRITICAL();
    *next_word = temp;
    taskEXIT_CRITICAL();

    // wake up keyboard task if needed
    if (s65x_keyboard_task_handle != NULL)
        xTaskNotify(s65x_keyboard_task_handle, 0, eIncrement);

    // successfully loaded new value
    return true;
}

// task to retrieve the next word to be sent by the enabled controllers
void s65x_keyboard_task(void *pvParameters) {
    s65x_keyboard_task_handle = xTaskGetCurrentTaskHandle();

    // current loop keyboard state
    s65x_keyboard this_state = {.u16 = 0};

    // round-robin selection of the keyboard to query
    s65x_keyboard_types_t next_keyboard = 0;

    // current N-key rollover keyboard state (max 16 keys)
    s65x_keyboard keyboard_state[KEYBOARD_QUEUE_LEN] = {0};

    while (1) {
#ifdef HAS_USB_KEYBOARD
        if (next_keyboard == KEYBOARD_USB) {
        }
#endif
#ifdef HAS_PS2_KEYBOARD
        if (next_keyboard == KEYBOARD_PS2) {
        }
#endif
#ifdef HAS_CUSTOM_KEYBOARD
        if (next_keyboard == KEYBOARD_CUSTOM) {
        }
#endif
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static bool s65x_controller_keyboard_init_complete = false;
bool s65x_controller_keyboard_init(void) {
    if (s65x_controller_keyboard_init_complete == true)
        return;

    if (s65x_keyboard_output_queue == NULL)
        s65x_keyboard_output_queue = xQueueCreate(KEYBOARD_QUEUE_LEN, sizeof(s65x_keyboard_t));
    if (s65x_keyboard_output_queue == NULL)
        return false;

    if (s65x_keyboard_task_handle == NULL)
        if (xTaskCreate(s65x_keyboard_task,
                        "s65x_keyboard",
                        64,
                        NULL,
                        1,
                        NULL) != pdPASS)
            return false;

    s65x_controller_keyboard_init_complete = true;
    return true;
}
