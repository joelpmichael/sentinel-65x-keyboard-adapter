/*
 * s65x_controller_pad.c
 *
 *  Created on: 7 Jul 2024
 *      Author: joel
 */

#include "s65x_controller_pad.h"

#include "custom_host.h"
#include "custom_pad.h"

#include "FreeRTOS.h"
#include "task.h"

#ifdef HAS_CONTROLLER_PAD
#ifndef HAS_USB_PAD
#ifndef HAS_CUSTOM_PAD
#error At least one of -DHAS_USB_PAD, or -DHAS_CUSTOM_PAD must be enabled
#endif
#endif
#endif

bool s65x_pad_get_next_word(uint16_t *next_word) {
    return false;
}

// task to retrieve the next word to be sent by the controller
void s65x_pad_task(void *pvParameters) {
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

bool s65x_controller_pad_init(void) {
    return true;
}
