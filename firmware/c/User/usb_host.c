/*
 * usb_host.c
 *
 *  Created on: 7 Jul 2024
 *      Author: joel
 */

#include "usb_host.h"

// flag for controller port init completion
static bool usb_host_init_complete = false;

bool usb_host_init(void) {
    // ensure init only runs once, turn it into a no-op if called again
    if (usb_host_init_complete == true)
        return true;

    usb_host_init_complete = true;
    return true;
}
