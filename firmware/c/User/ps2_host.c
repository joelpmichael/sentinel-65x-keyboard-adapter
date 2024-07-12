/*
 * ps2_controller.c
 *
 *  Created on: 10 Jul 2024
 *      Author: joel
 */

#include <ps2_host.h>

// flag for controller port init completion
static bool ps2_host_init_complete = false;

bool ps2_host_init(void) {
    // ensure init only runs once, turn it into a no-op if called again
    if (ps2_host_init_complete == true)
        return true;

    ps2_host_init_complete = true;
    return true;
}
