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

#include "s65x_keyboard_scancodes.h"

#define S65X_SIGNATURE_KEYBOARD 0b0010

#define S65X_ERROR_KEYBOARD_ROLLOVER 0x01
#define S65X_ERROR_KEYBOARD_BUFFER 0x02

typedef struct __attribute((packed)) s65x_keyboard_s {
    uint8_t signature : 4;
    bool mod_meta : 1;
    bool mod_alt : 1;
    bool mod_shift : 1;
    bool mod_ctrl : 1;
    keyboard_scancode_t scan_code : 7;
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

void s65x_keyboard_set_mode(s65x_keyboard_mode_t mode);

#define KEYBOARD_AUTOREPEAT_DELAY 500 // msec
#define KEYBOARD_AUTOREPEAT_RATE 50   // msec
void s65x_keyboard_set_autorepeat(bool repeat);

void s65x_keyboard_remap(keyboard_scancode_t from_scancode, keyboard_scancode_t to_scancode);
inline static void s65x_keyboard_remap_reset(keyboard_scancode_t from_scancode) {
    s65x_keyboard_remap(from_scancode, from_scancode);
};
void s65x_keyboard_set_map_lock(bool map);
inline static void s65x_keyboard_set_swap_ctrl_caps(bool swap) {
    if (swap == true) {
        // swap left-ctrl and capslock
        s65x_keyboard_remap(KEYBOARD_SCANCODE_LCTRL, KEYBOARD_SCANCODE_CAPSLOCK);
        s65x_keyboard_remap(KEYBOARD_SCANCODE_CAPSLOCK, KEYBOARD_SCANCODE_LCTRL);
    } else {
        // un-swap left-ctrl and capslock
        s65x_keyboard_remap_reset(KEYBOARD_SCANCODE_LCTRL);
        s65x_keyboard_remap_reset(KEYBOARD_SCANCODE_CAPSLOCK);
    }
};
inline static void s65x_keyboard_set_swap_alt_meta(bool swap) {
    if (swap == true) {
        // swap alt and meta
        s65x_keyboard_remap(KEYBOARD_SCANCODE_LALT, KEYBOARD_SCANCODE_LMETA);
        s65x_keyboard_remap(KEYBOARD_SCANCODE_LMETA, KEYBOARD_SCANCODE_LALT);
        s65x_keyboard_remap(KEYBOARD_SCANCODE_RALT_ALTGR, KEYBOARD_SCANCODE_RMETA);
        s65x_keyboard_remap(KEYBOARD_SCANCODE_RMETA, KEYBOARD_SCANCODE_RALT_ALTGR);
    } else {
        // un-swap alt and meta
        s65x_keyboard_remap_reset(KEYBOARD_SCANCODE_LALT);
        s65x_keyboard_remap_reset(KEYBOARD_SCANCODE_LMETA);
        s65x_keyboard_remap_reset(KEYBOARD_SCANCODE_RALT_ALTGR);
        s65x_keyboard_remap_reset(KEYBOARD_SCANCODE_RMETA);
    }
};
void s65x_keyboard_set_altgr_mode(bool enable);
bool s65x_keyboard_get_next_word(uint16_t *next_word) __attribute__((hot, section(".ramfunc")));
bool s65x_controller_keyboard_init(void) __attribute__((section(".slowfunc")));

#endif /* USER_S65X_CONTROLLER_KEYBOARD_H_ */
