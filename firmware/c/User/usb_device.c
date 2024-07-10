/*
 * usb_device.c
 *
 *  Created on: 7 Jul 2024
 *      Author: joel
 */

#include "usb_device.h"

// flag for usb device init completion
static bool usb_device_init_complete = false;

bool usb_device_init(void) {
    // ensure init only runs once, turn it into a no-op if called again
    if (usb_device_init_complete == true)
        return true;

    usb_device_init_complete = true;
    return true;
}
