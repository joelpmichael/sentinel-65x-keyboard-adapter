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
QueueHandle_t s65x_keyboard_input_queue = NULL;

// fetch next keyboard word to be transmitted on the SNES controller port
bool s65x_keyboard_get_next_word(uint16_t *next_word) {
    uint16_t temp;
    if (xQueueReceive(s65x_keyboard_output_queue, &temp, 0) == pdFALSE)
        return false;
    taskENTER_CRITICAL();
    *next_word = temp;
    taskEXIT_CRITICAL();
    return true;
}

// task to retrieve the next word to be sent by the enabled controllers
void s65x_keyboard_task(void *pvParameters) {
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

bool s65x_controller_keyboard_init(void) {
    s65x_keyboard_input_queue = xQueueCreate(KEYBOARD_QUEUE_LEN, sizeof(uint16_t));
    if (s65x_keyboard_input_queue == NULL)
        return false;

    s65x_keyboard_output_queue = xQueueCreate(KEYBOARD_QUEUE_LEN, sizeof(uint16_t));
    if (s65x_keyboard_output_queue == NULL)
        return false;

    if (xTaskCreate(s65x_keyboard_task,
                    "s65x_keyboard",
                    64,
                    NULL,
                    1,
                    NULL) != pdPASS)
        return false;

    return true;
}
