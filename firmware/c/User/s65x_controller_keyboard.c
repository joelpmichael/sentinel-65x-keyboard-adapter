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
#include "timers.h"

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
static QueueHandle_t s65x_keyboard_input_queue = NULL;
static QueueHandle_t s65x_keyboard_output_queue = NULL;

static TaskHandle_t s65x_keyboard_task_handle = NULL;

// configuration variables and set functions

// keyboard mode
static s65x_keyboard_mode_t keyboard_mode = KEYBOARD_MODE_0;
void s65x_keyboard_set_mode(s65x_keyboard_mode_t mode) {
    portENTER_CRITICAL();
    keyboard_mode = mode;
    portEXIT_CRITICAL();
}

// enable auto-repeat for mode 0 and 1
static bool keyboard_autorepeat = false;
// timers for auto-repeat mode
TimerHandle_t keyboard_keypress_timers[128] = {0};
void s65x_keyboard_set_autorepeat(bool repeat) {
    portENTER_CRITICAL();
    keyboard_autorepeat = repeat;
    portEXIT_CRITICAL();
}
// auto-repeat timer callback function
void s65x_keyboard_autorepeat_cb(TimerHandle_t xTimer) {
}

// re-mapping table
static keyboard_scancode_t keyboard_remap[128];
void s65x_keyboard_remap(keyboard_scancode_t from_scancode, keyboard_scancode_t to_scancode) {
    portENTER_CRITICAL();
    keyboard_remap[from_scancode] = to_scancode;
    portEXIT_CRITICAL();
}

// map lock to modifier
static bool map_lock = false;
void s65x_keyboard_set_map_lock(bool map) {
    portENTER_CRITICAL();
    map_lock = map;
    portEXIT_CRITICAL();
}

// enable special AltGr key handling for ISO keyboards
static bool altgr_mode = false;
void s65x_keyboard_set_altgr_mode(bool enable) {
    portENTER_CRITICAL();
    altgr_mode = enable;
    portEXIT_CRITICAL();
}

// fetch next keyboard word to be transmitted on the SNES controller port
bool s65x_keyboard_get_next_word(uint16_t *next_word) {
    // if the queue hasn't been created, just return false
    // because there's definitely no data to retrieve yet
    if (s65x_keyboard_output_queue == NULL)
        return false;

    // receive into a temporary variable, without blocking
    // if there's no data in the queue, don't load the next word
    s65x_keyboard temp;
    if (xQueueReceive(s65x_keyboard_output_queue, &temp, 0) == pdFALSE) {
        // wake up keyboard task if needed
        // keyboard tasks have higher priority than the controller
        if (s65x_keyboard_task_handle != NULL)
            xTaskNotify(s65x_keyboard_task_handle, 0, eNoAction);

        // if there's no data here now, there's definitely no data...
        if (xQueueReceive(s65x_keyboard_output_queue, &temp, 0) == pdFALSE)
            return false;
    }

    // ensure interrupts are disabled during load of *next_word
    taskENTER_CRITICAL();
    *next_word = temp.u16;
    taskEXIT_CRITICAL();

    // successfully loaded new value
    return true;
}

// receive a keyboard raw scan code or error to the queue, and wake up
bool s65x_keyboard_receive_event(s65x_keyboard event) {
    BaseType_t result;

    // attempt to send to queue non-blocking
    if (s65x_keyboard_input_queue != NULL)
        result = xQueueSend(s65x_keyboard_input_queue, &event, 0);

    // wake up the task - if sending to queue non-blocking failed
    // then this will clear the queue as well
    if (s65x_keyboard_task_handle != NULL)
        xTaskNotify(s65x_keyboard_task_handle, 0, eNoAction);

    // if couldn't send to queue non-blocking, we have woken up the task
    // so queue is probably empty now, try again with blocking for 1s
    // if it succeeds, wake up the task again
    if (result != pdTRUE)
        if (s65x_keyboard_input_queue != NULL) {
            if (xQueueSend(s65x_keyboard_input_queue, &event, pdMS_TO_TICKS(1000)) != pdTRUE)
                return false;
            else if (s65x_keyboard_task_handle != NULL)
                xTaskNotify(s65x_keyboard_task_handle, 0, eNoAction);
        }
    return true;
}

// task to retrieve the next word to be sent by the enabled controllers
void s65x_keyboard_task(void *pvParameters) {
    s65x_keyboard_task_handle = xTaskGetCurrentTaskHandle();

    keyboard_scancode_t mode0_keypress = KEYBOARD_SCANCODE_NONE;

    // current N-key rollover keyboard state (mode 1, 2, 3)
    bool keyboard_keypress_state[128] = {0};

    // current modifier key status
    bool modifier_ctrl = false;
    bool modifier_shift = false;
    bool modifier_alt = false;
    bool modifier_meta = false;
    bool modifier_altgr = false;

    // current lock status
    bool caps_lock = false;
    bool num_lock = false;
    bool scroll_lock = false;

    // if no data in an entire loop through, then wait for notification
    // reset to false only just before sleeping, not at the top of the loop
    bool wakeup_has_data = false;

    // start up paused
    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
    while (1) {
        // current loop keyboard state
        s65x_keyboard this_state = {.u16 = 0};
        this_state.keyboard.signature = S65X_SIGNATURE_KEYBOARD;

        // current loop data fetch state
        bool keyboard_has_data = false;

        // fetch new data
        if (xQueueReceive(s65x_keyboard_input_queue, &this_state, 0) == pdTRUE)
            keyboard_has_data = true;

        // set outer flag only if unset
        if (wakeup_has_data == false)
            wakeup_has_data = keyboard_has_data;

        // check if this is an error, send directly to queue if it is
        if (this_state.error.signature == S65X_SIGNATURE_ERROR) {
            if (xQueueSendToBack(s65x_keyboard_output_queue, &this_state, portMAX_DELAY) != pdTRUE) {
                s65x_controller_run_fail();
            }
            continue;
        }

        // check that the received state is actually a keyboard (this should never happen!)
        if (this_state.keyboard.signature != S65X_SIGNATURE_KEYBOARD)
            s65x_controller_run_fail();

        // modify scan codes according to re-mapping
        portENTER_CRITICAL();
        this_state.keyboard.scan_code = keyboard_remap[this_state.keyboard.scan_code];
        portEXIT_CRITICAL();

        // process keyboard event according to current mode
        switch (keyboard_mode) {
        case KEYBOARD_MODE_0: {
            // Mode 0
            // single key-down scan code is repeated while the key is held down

            if (keyboard_has_data == true) {
                // process received scancode
                // There are
            } else {
                // no scan-code received
                if (wakeup_has_data == false) {
                    // this was a poll from the controller, re-send current state
                } else
                    // no scan-code received, but one was previously received, so just go back to sleep
                    wakeup_has_data = false;
            }
            break;
        }
        case KEYBOARD_MODE_1: {
            // Mode 1
            // Multiple key-down scan codes are repeated in the order of keys being pressed
            if (keyboard_has_data == true) {
                // process received scancode
            } else {
                // no scan-code received
                if (wakeup_has_data == false) {
                    // this was a poll from the controller, re-send current state
                } else
                    // no scan-code received, but one was previously received, so just go back to sleep
                    wakeup_has_data = false;
            }
            break;
        }
        case KEYBOARD_MODE_2: {
            // Mode 2
            // Key-down and key-up scan codes are sent in the order of keys being pressed
            if (keyboard_has_data == true) {
                // process received scancode
            } else {
                // no scan-code received
                if (wakeup_has_data == false) {
                    // this was a poll from the controller, re-send current state
                } else
                    // no scan-code received, but one was previously received, so just go back to sleep
                    wakeup_has_data = false;
            }
            break;
        }
        case KEYBOARD_MODE_3: {
            // Mode 3
            // Raw scan codes are sent only on change, modifiers are not set
            // Zero scan code is sent on no-change
            if (xQueueSendToBack(s65x_keyboard_output_queue, &this_state, portMAX_DELAY) != pdTRUE)
                s65x_controller_run_fail();

            // no new data received, go back to sleep
            if (wakeup_has_data == true && keyboard_has_data == false)
                wakeup_has_data = false;

            break;
        }
        }

        // sleepy time after chewing on all the data!
        if (wakeup_has_data == false)
            xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
    }
}

static bool s65x_controller_keyboard_init_complete = false;
bool s65x_controller_keyboard_init(void) {
    // only init once
    if (s65x_controller_keyboard_init_complete == true)
        return true;

    // init mapping table 1:1
    for (uint8_t i = 0; i < 128; i++)
        keyboard_remap[i] = i;

    // create input queue (from USB/PS2/etc keyboard)
    if (s65x_keyboard_input_queue == NULL)
        s65x_keyboard_input_queue = xQueueCreate(KEYBOARD_QUEUE_LEN, sizeof(s65x_keyboard));
    if (s65x_keyboard_input_queue == NULL)
        return false;

    // create output queue (to Sentinel 65X)
    if (s65x_keyboard_output_queue == NULL)
        s65x_keyboard_output_queue = xQueueCreate(KEYBOARD_QUEUE_LEN, sizeof(s65x_keyboard));
    if (s65x_keyboard_output_queue == NULL)
        return false;

    // create processing task
    if (s65x_keyboard_task_handle == NULL)
        if (xTaskCreate(s65x_keyboard_task,
                        "s65x_keyboard",
                        384,
                        NULL,
                        2,
                        NULL) != pdPASS)
            return false;

    s65x_controller_keyboard_init_complete = true;
    return true;
}
