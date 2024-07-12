/*
 * s65x_controller.h
 *
 *  Created on: 7 Jul 2024
 *      Author: joel
 */

#ifndef USER_S65X_CONTROLLER_H_
#define USER_S65X_CONTROLLER_H_

#include "stdatomic.h"
#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"

#ifndef HAS_CONTROLLER_PAD
#ifndef HAS_CONTROLLER_MOUSE
#ifndef HAS_CONTROLLER_KEYBOARD
#error At least one of -DHAS_CONTROLLER_PAD, -DHAS_CONTROLLER_MOUSE, or -DHAS_CONTROLLER_KEYBOARD must be enabled
#endif
#endif
#endif

typedef enum s65x_controller_devices_e {
#ifdef HAS_CONTROLLER_PAD
    CONTROLLER_PAD,
#endif
#ifdef HAS_CONTROLLER_MOUSE
    CONTROLLER_MOUSE,
#endif
#ifdef HAS_CONTROLLER_KEYBOARD
    CONTROLLER_KEYBOARD,
#endif
    NUM_CONTROLLER_DEVICES
} s65x_controller_devices_t;

bool s65x_controller_init(void);
void s65x_controller_post_fail(void) __attribute__((noreturn));
void s65x_controller_run_fail(void) __attribute__((noreturn));

#endif /* USER_S65X_CONTROLLER_H_ */
