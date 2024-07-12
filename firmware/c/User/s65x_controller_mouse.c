/*
 * s65x_controller_mouse.c
 *
 *  Created on: 7 Jul 2024
 *      Author: joel
 */

#include "s65x_controller_mouse.h"

#include "ps2_controller.h"
#include "ps2_controller_mouse.h"

#include "usb_host.h"
#include "usb_host_mouse.h"

#include "FreeRTOS.h"
#include "task.h"

bool s65x_mouse_get_next_word(uint16_t *next_word) {
    return false;
}

// task to retrieve the next word to be sent by the controller
void s65x_mouse_task(void *pvParameters) {
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

bool s65x_controller_mouse_init(void) {
    return true;
}
