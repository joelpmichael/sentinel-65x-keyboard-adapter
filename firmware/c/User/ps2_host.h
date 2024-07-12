/*
 * ps2_controller.h
 *
 *  Created on: 10 Jul 2024
 *      Author: joel
 */

#ifndef USER_PS2_HOST_H_
#define USER_PS2_HOST_H_

#include "stdatomic.h"
#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"

typedef enum ps2_devices_e {
#ifdef HAS_PS2_MOUSE
    PS2_MOUSE,
#endif
#ifdef HAS_PS2_KEYBOARD
    PS2_KEYBOARD,
#endif
    NUM_PS2_DEVICES
} ps2_devices_t;

bool ps2_host_init(void);

#endif /* USER_PS2_HOST_H_ */
