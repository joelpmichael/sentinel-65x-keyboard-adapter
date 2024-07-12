/*
 * s65x_controller_mouse.c
 *
 *  Created on: 7 Jul 2024
 *      Author: joel
 */

#include "s65x_controller_mouse.h"

#include "custom_host.h"
#include "custom_mouse.h"
#include "ps2_host.h"
#include "ps2_host_mouse.h"
#include "usb_host.h"
#include "usb_host_mouse.h"

#include "FreeRTOS.h"
#include "task.h"

#ifdef HAS_CONTROLLER_MOUSE
#ifndef HAS_USB_MOUSE
#ifndef HAS_PS2_MOUSE
#ifndef HAS_CUSTOM_MOUSE
#error At least one of -DHAS_USB_MOUSE, -DHAS_PS2_MOUSE, or -DHAS_CUSTOM_MOUSE must be enabled
#endif
#endif
#endif
#endif

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
