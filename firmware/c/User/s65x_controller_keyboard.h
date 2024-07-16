/*
 * s65x_controller_keyboard.h
 *
 *  Created on: 7 Jul 2024
 *      Author: joel
 */

#ifndef USER_S65X_CONTROLLER_KEYBOARD_H_
#define USER_S65X_CONTROLLER_KEYBOARD_H_

#include "stdatomic.h"
#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"

#include "s65x_controller.h"

#define S65X_SIGNATURE_KEYBOARD 0b0010

#define S65X_ERROR_KEYBOARD_ROLLOVER 0x01
#define S65X_ERROR_KEYBOARD_BUFFER 0x02

typedef struct __attribute((packed)) s65x_keyboard_s {
    uint8_t signature : 4;
    bool mod_meta : 1;
    bool mod_alt : 1;
    bool mod_shift : 1;
    bool mod_ctrl : 1;
    uint8_t scan_code : 7;
    bool key_up : 1;
} s65x_keyboard_t;

typedef union s65x_keyboard_u {
    s65x_keyboard_t keyboard;
    s65x_error_t error;
    uint16_t u16;
} s65x_keyboard;

typedef enum s65x_keyboard_types_e {
#ifdef HAS_USB_KEYBOARD
    KEYBOARD_USB,
#endif
#ifdef HAS_PS2_KEYBOARD
    KEYBOARD_PS2,
#endif
#ifdef HAS_CUSTOM_KEYBOARD
    KEYBOARD_CUSTOM,
#endif
    NUM_KEYBOARD_TYPES,
} s65x_keyboard_types_t;

typedef enum s65x_keyboard_mode_e {
    KEYBOARD_MODE_0 = 0,
    KEYBOARD_MODE_1,
    KEYBOARD_MODE_2,
    KEYBOARD_MODE_3,
} s65x_keyboard_mode_t;

inline void s65s_keyboard_set_mode(s65x_keyboard_mode_t mode);
inline void s65x_keyboard_set_autorepeat(bool repeat);
inline void s65x_keyboard_set_map_lock(bool map);
inline void s65x_keyboard_set_swap_ctrl_caps(bool swap);
inline void s65x_keyboard_set_swap_alt_meta(bool swap);
bool s65x_keyboard_get_next_word(uint16_t *next_word) __attribute__((hot, section(".ramfunc")));
bool s65x_controller_keyboard_init(void) __attribute__((section(".slowfunc")));

#endif /* USER_S65X_CONTROLLER_KEYBOARD_H_ */
