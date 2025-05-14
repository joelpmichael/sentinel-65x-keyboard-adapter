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
#include "stdlib.h"

#ifndef HAS_CONTROLLER_PAD
#ifndef HAS_CONTROLLER_MOUSE
#ifndef HAS_CONTROLLER_KEYBOARD
#error At least one of -DHAS_CONTROLLER_PAD, -DHAS_CONTROLLER_MOUSE, or -DHAS_CONTROLLER_KEYBOARD must be enabled
#endif
#endif
#endif

#define S65X_SIGNATURE_ERROR 0b1111

#define S65X_ERROR_SOURCE_GENERIC 0b1111

#define S65X_ERROR_GENERIC_TRANSIENT_INPUT 0x01
#define S65X_ERROR_GENERIC_TRANSIENT_OUTPUT 0x02
#define S65X_ERROR_GENERIC_TRANSIENT_PROCESS 0x03
#define S65X_ERROR_GENERIC_FATAL_RUNTIME 0xFD
#define S65X_ERROR_GENERIC_FATAL_POST 0xFE
#define S65X_ERROR_GENERIC_UNDERFLOW 0xFF

typedef struct __attribute((packed)) s65x_error_s {
    uint8_t signature : 4;
    uint8_t source_signature : 4;
    uint8_t error_code : 8;
} s65x_error_t;

typedef union s65x_error_u {
    s65x_error_t error;
    uint16_t u16;
} s65x_error;

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

bool s65x_controller_init(void) __attribute__((section(".slowfunc")));
void s65x_controller_post_fail(void) __attribute__((section(".slowfunc")));
void s65x_controller_run_fail(void) __attribute__((section(".slowfunc")));

#endif /* USER_S65X_CONTROLLER_H_ */
