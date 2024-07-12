/*
 * custom_host.c
 *
 *  Created on: 12 Jul 2024
 *      Author: joel
 */

#include "custom_host.h"

// flag for controller port init completion
static bool custom_host_init_complete = false;

bool custom_host_init(void) {
    // ensure init only runs once, turn it into a no-op if called again
    if (custom_host_init_complete == true)
        return true;

    custom_host_init_complete = true;
    return true;
}
