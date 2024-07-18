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

// current N-key rollover keyboard state (mode 1, 2, 3)
static s65x_keyboard keyboard_keypress_state[128] = {0};

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
TimerHandle_t keyboard_keypress_timers[128] = {NULL};
void s65x_keyboard_set_autorepeat(bool repeat) {
    portENTER_CRITICAL();
    keyboard_autorepeat = repeat;
    portEXIT_CRITICAL();
}
// auto-repeat timer callback function
void s65x_keyboard_autorepeat_cb(TimerHandle_t xTimer) {
    // after first timer repeat period, set the timer timeout to autorepeat rate
    if (xTimerGetPeriod(xTimer) != KEYBOARD_AUTOREPEAT_RATE) {
        if (xTimerChangePeriod(xTimer, KEYBOARD_AUTOREPEAT_RATE, 1) != pdPASS)
            s65x_controller_run_fail();
    }
    keyboard_scancode_t scancode = (keyboard_scancode_t)pvTimerGetTimerID(xTimer);

    portENTER_CRITICAL();
    keyboard_keypress_state[scancode].keyboard.key_up = true;
    portEXIT_CRITICAL();
    if (xQueueSendToBack(s65x_keyboard_output_queue, &keyboard_keypress_state[scancode], portMAX_DELAY) != pdTRUE)
        s65x_controller_run_fail();
    portENTER_CRITICAL();
    keyboard_keypress_state[scancode].keyboard.key_up = false;
    portEXIT_CRITICAL();
    if (xQueueSendToBack(s65x_keyboard_output_queue, &keyboard_keypress_state[scancode], portMAX_DELAY) != pdTRUE)
        s65x_controller_run_fail();
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

    s65x_keyboard mode0_keypress = {.u16 = 0};
    mode0_keypress.keyboard.signature = S65X_SIGNATURE_KEYBOARD;

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

        // check if this is an error, send directly to queue if it is
        if (this_state.error.signature == S65X_SIGNATURE_ERROR) {
            if (xQueueSendToBack(s65x_keyboard_output_queue, &this_state, portMAX_DELAY) != pdTRUE) {
                s65x_controller_run_fail();
            }
            continue;
        }

        // set outer flag only if unset
        if (wakeup_has_data == false)
            wakeup_has_data = keyboard_has_data;

        // check that the received state is actually a keyboard (this should never happen!)
        if (this_state.keyboard.signature != S65X_SIGNATURE_KEYBOARD)
            s65x_controller_run_fail();

        // modify scan codes according to re-mapping
        portENTER_CRITICAL();
        this_state.keyboard.scan_code = keyboard_remap[this_state.keyboard.scan_code];
        portEXIT_CRITICAL();

        // process modifier keys, and arm auto-repeats on repeating keys
        // doing auto-repeats here allows us to use a single switch/case statement

        switch (this_state.keyboard.scan_code) {

        // modifier keys
        case KEYBOARD_SCANCODE_LALT: {
            portENTER_CRITICAL();
            modifier_alt = !this_state.keyboard.key_up;
            portEXIT_CRITICAL();
        }
        case KEYBOARD_SCANCODE_RALT_ALTGR: {
            // special handling for right-alt / AltGr
            portENTER_CRITICAL();
            if (altgr_mode == true) {
                modifier_altgr = !this_state.keyboard.key_up;
            } else {
                modifier_alt = !this_state.keyboard.key_up;
            }
            portEXIT_CRITICAL();
            break;
        }
        case KEYBOARD_SCANCODE_LSHIFT:
        case KEYBOARD_SCANCODE_RSHIFT: {
            portENTER_CRITICAL();
            modifier_shift = !this_state.keyboard.key_up;
            portEXIT_CRITICAL();
            break;
        }
        case KEYBOARD_SCANCODE_LCTRL:
        case KEYBOARD_SCANCODE_RCTRL: {
            portENTER_CRITICAL();
            modifier_ctrl = !this_state.keyboard.key_up;
            portEXIT_CRITICAL();
            break;
        }
        case KEYBOARD_SCANCODE_LMETA:
        case KEYBOARD_SCANCODE_RMETA: {
            portENTER_CRITICAL();
            modifier_meta = !this_state.keyboard.key_up;
            portEXIT_CRITICAL();
            break;
        }

        // lock keys
        case KEYBOARD_SCANCODE_CAPSLOCK:
        case KEYBOARD_SCANCODE_NUMLOCK:
        case KEYBOARD_SCANCODE_SCRLOCK: {
            portENTER_CRITICAL();
            if (this_state.keyboard.scan_code == KEYBOARD_SCANCODE_CAPSLOCK)
                caps_lock = !this_state.keyboard.key_up;
            else if (this_state.keyboard.scan_code == KEYBOARD_SCANCODE_NUMLOCK)
                num_lock = !this_state.keyboard.key_up;
            else if (this_state.keyboard.scan_code == KEYBOARD_SCANCODE_SCRLOCK)
                num_lock = !this_state.keyboard.key_up;
            portEXIT_CRITICAL();
            break;
        }

        // non-repeating keys
        case KEYBOARD_SCANCODE_PRTSCN:
        case KEYBOARD_SCANCODE_PAUSE:
        case KEYBOARD_SCANCODE_VOLMUTE:
        case KEYBOARD_SCANCODE_APPS:
        case KEYBOARD_SCANCODE_SLEEP:
        case KEYBOARD_SCANCODE_POWER: {
            break;
        }

        // everything else can repeat
        default: {
            if (keyboard_autorepeat == true) {
                if (this_state.keyboard.key_up == true && keyboard_keypress_timers[this_state.keyboard.scan_code] != NULL) {
                    // stop and delete timer
                    if (xTimerStop(keyboard_keypress_timers[this_state.keyboard.scan_code], 1) != pdPASS)
                        s65x_controller_run_fail();
                    if (xTimerDelete(keyboard_keypress_timers[this_state.keyboard.scan_code], 1) != pdPASS)
                        s65x_controller_run_fail();
                    portENTER_CRITICAL();
                    keyboard_keypress_timers[this_state.keyboard.scan_code] = NULL;
                    portEXIT_CRITICAL();
                }
                if (this_state.keyboard.key_up == false && keyboard_keypress_timers[this_state.keyboard.scan_code] == NULL) {
                    // create and start timer
                    portENTER_CRITICAL();
                    keyboard_keypress_timers[this_state.keyboard.scan_code] = xTimerCreate("KB_REPEAT_T", pdMS_TO_TICKS(KEYBOARD_AUTOREPEAT_DELAY), pdTRUE, (void *)this_state.keyboard.scan_code, s65x_keyboard_autorepeat_cb);
                    portEXIT_CRITICAL();
                    if (keyboard_keypress_timers[this_state.keyboard.scan_code] == NULL)
                        s65x_controller_run_fail();
                    if (xTimerStart(keyboard_keypress_timers[this_state.keyboard.scan_code], 1) != pdPASS)
                        s65x_controller_run_fail();
                }
            }
            break;
        }
        }

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
