/*
 * s65x_controller_keyboard.h
 *
 *  Created on: 7 Jul 2024
 *      Author: joel
 */

#ifndef USER_S65X_KEYBOARD_SCANCODES_H
#define USER_S65X_KEYBOARD_SCANCODES_H

/*
Scan code 	Key 	Scan code 	Key 	Scan code 	Key 	Scan code 	Key
                    0x01 	escape pressed 	0x02 	1 pressed 	0x03 	2 pressed
0x04 	3 pressed 	0x05 	4 pressed 	0x06 	5 pressed 	0x07 	6 pressed
0x08 	7 pressed 	0x09 	8 pressed 	0x0A 	9 pressed 	0x0B 	0 (zero) pressed
0x0C 	- pressed 	0x0D 	= pressed 	0x0E 	backspace pressed 	0x0F 	tab pressed
0x10 	Q pressed 	0x11 	W pressed 	0x12 	E pressed 	0x13 	R pressed
0x14 	T pressed 	0x15 	Y pressed 	0x16 	U pressed 	0x17 	I pressed
0x18 	O pressed 	0x19 	P pressed 	0x1A 	[ pressed 	0x1B 	] pressed
0x1C 	enter pressed 	0x1D 	left control pressed 	0x1E 	A pressed 	0x1F 	S pressed
0x20 	D pressed 	0x21 	F pressed 	0x22 	G pressed 	0x23 	H pressed
0x24 	J pressed 	0x25 	K pressed 	0x26 	L pressed 	0x27 	; pressed
0x28 	' (single quote) pressed 	0x29 	` (back tick) pressed 	0x2A 	left shift pressed 	0x2B 	\ pressed
0x2C 	Z pressed 	0x2D 	X pressed 	0x2E 	C pressed 	0x2F 	V pressed
0x30 	B pressed 	0x31 	N pressed 	0x32 	M pressed 	0x33 	, pressed
0x34 	. pressed 	0x35 	/ pressed 	0x36 	right shift pressed 	0x37 	(keypad) * pressed
0x38 	left alt pressed 	0x39 	space pressed 	0x3A 	CapsLock pressed 	0x3B 	F1 pressed
0x3C 	F2 pressed 	0x3D 	F3 pressed 	0x3E 	F4 pressed 	0x3F 	F5 pressed
0x40 	F6 pressed 	0x41 	F7 pressed 	0x42 	F8 pressed 	0x43 	F9 pressed
0x44 	F10 pressed 	0x45 	NumberLock pressed 	0x46 	ScrollLock pressed 	0x47 	(keypad) 7 pressed
0x48 	(keypad) 8 pressed 	0x49 	(keypad) 9 pressed 	0x4A 	(keypad) - pressed 	0x4B 	(keypad) 4 pressed
0x4C 	(keypad) 5 pressed 	0x4D 	(keypad) 6 pressed 	0x4E 	(keypad) + pressed 	0x4F 	(keypad) 1 pressed
0x50 	(keypad) 2 pressed 	0x51 	(keypad) 3 pressed 	0x52 	(keypad) 0 pressed 	0x53 	(keypad) . pressed
                        0x57 	F11 pressed
0x58 	F12 pressed
*/

typedef enum keyboard_scancode_e {
    KEYBOARD_SCANCODE_NONE = 0x00,

    // regular PS/2 keyboard scan codes - can probably be used as-is
    KEYBOARD_SCANCODE_ESC = 0x01,
    KEYBOARD_SCANCODE_1 = 0x02,
    KEYBOARD_SCANCODE_2 = 0x03,
    KEYBOARD_SCANCODE_3 = 0x04,
    KEYBOARD_SCANCODE_4 = 0x05,
    KEYBOARD_SCANCODE_5 = 0x06,
    KEYBOARD_SCANCODE_6 = 0x07,
    KEYBOARD_SCANCODE_7 = 0x08,
    KEYBOARD_SCANCODE_8 = 0x09,
    KEYBOARD_SCANCODE_9 = 0x0A,
    KEYBOARD_SCANCODE_0 = 0x0B,
    KEYBOARD_SCANCODE_MINUS = 0x0C,
    KEYBOARD_SCANCODE_EQUAL = 0x0D,
    KEYBOARD_SCANCODE_BKSP = 0x0E,

    KEYBOARD_SCANCODE_TAB = 0x0F,
    KEYBOARD_SCANCODE_Q = 0x10,
    KEYBOARD_SCANCODE_W = 0x11,
    KEYBOARD_SCANCODE_E = 0x12,
    KEYBOARD_SCANCODE_R = 0x13,
    KEYBOARD_SCANCODE_T = 0x14,
    KEYBOARD_SCANCODE_Y = 0x15,
    KEYBOARD_SCANCODE_U = 0x16,
    KEYBOARD_SCANCODE_I = 0x17,
    KEYBOARD_SCANCODE_O = 0x18,
    KEYBOARD_SCANCODE_P = 0x19,
    KEYBOARD_SCANCODE_OPENSQ = 0x1A,
    KEYBOARD_SCANCODE_CLOSESQ = 0x1B,
    KEYBOARD_SCANCODE_ENTER = 0x1C,

    KEYBOARD_SCANCODE_LCTRL = 0x1D,
    KEYBOARD_SCANCODE_A = 0x1E,
    KEYBOARD_SCANCODE_S = 0x1F,
    KEYBOARD_SCANCODE_D = 0x20,
    KEYBOARD_SCANCODE_F = 0x21,
    KEYBOARD_SCANCODE_G = 0x22,
    KEYBOARD_SCANCODE_H = 0x23,
    KEYBOARD_SCANCODE_J = 0x24,
    KEYBOARD_SCANCODE_K = 0x25,
    KEYBOARD_SCANCODE_L = 0x26,
    KEYBOARD_SCANCODE_SEMI = 0x27,
    KEYBOARD_SCANCODE_QUOTE = 0x28,
    KEYBOARD_SCANCODE_BACKTICK = 0x29,

    KEYBOARD_SCANCODE_LSHIFT = 0x2A,
    KEYBOARD_SCANCODE_BSLASH = 0x2B,
    KEYBOARD_SCANCODE_Z = 0x2C,
    KEYBOARD_SCANCODE_X = 0x2D,
    KEYBOARD_SCANCODE_C = 0x2E,
    KEYBOARD_SCANCODE_V = 0x2F,
    KEYBOARD_SCANCODE_B = 0x30,
    KEYBOARD_SCANCODE_N = 0x31,
    KEYBOARD_SCANCODE_M = 0x32,
    KEYBOARD_SCANCODE_COMMA = 0x33,
    KEYBOARD_SCANCODE_DOT = 0x34,
    KEYBOARD_SCANCODE_SLASH = 0x35,
    KEYBOARD_SCANCODE_RSHIFT = 0x36,

    KEYBOARD_SCANCODE_KPSTAR = 0x37,
    KEYBOARD_SCANCODE_LALT = 0x38,
    KEYBOARD_SCANCODE_SPACE = 0x39,
    KEYBOARD_SCANCODE_CAPSLOCK = 0x3A,

    KEYBOARD_SCANCODE_F1 = 0x3B,
    KEYBOARD_SCANCODE_F2 = 0x3C,
    KEYBOARD_SCANCODE_F3 = 0x3D,
    KEYBOARD_SCANCODE_F4 = 0x3E,
    KEYBOARD_SCANCODE_F5 = 0x3F,
    KEYBOARD_SCANCODE_F6 = 0x40,
    KEYBOARD_SCANCODE_F7 = 0x41,
    KEYBOARD_SCANCODE_F8 = 0x42,
    KEYBOARD_SCANCODE_F9 = 0x43,
    KEYBOARD_SCANCODE_F10 = 0x44,
    KEYBOARD_SCANCODE_NUMLOCK = 0x45,
    KEYBOARD_SCANCODE_SCRLOCK = 0x46,

    KEYBOARD_SCANCODE_KP7 = 0x47,
    KEYBOARD_SCANCODE_KP8 = 0x48,
    KEYBOARD_SCANCODE_KP9 = 0x49,
    KEYBOARD_SCANCODE_KPMINUS = 0x4A,
    KEYBOARD_SCANCODE_KP4 = 0x4B,
    KEYBOARD_SCANCODE_KP5 = 0x4C,
    KEYBOARD_SCANCODE_KP6 = 0x4D,
    KEYBOARD_SCANCODE_KPPLUS = 0x4E,
    KEYBOARD_SCANCODE_KP1 = 0x4F,
    KEYBOARD_SCANCODE_KP2 = 0x50,
    KEYBOARD_SCANCODE_KP3 = 0x51,
    KEYBOARD_SCANCODE_KP0 = 0x52,
    KEYBOARD_SCANCODE_KPDOT = 0x53,

    KEYBOARD_SCANCODE_EURO = 0x56,
    KEYBOARD_SCANCODE_F11 = 0x57,
    KEYBOARD_SCANCODE_F12 = 0x58,

    // extended PS/2 scan codes - THESE PROBABLY NEED CONVERSION

    KEYBOARD_SCANCODE_KPSLASH = 0x54,
    KEYBOARD_SCANCODE_KPENTER = 0x55,
    KEYBOARD_SCANCODE_PAUSE = 0x59,

    KEYBOARD_SCANCODE_RCTRL = 0x5A,
    KEYBOARD_SCANCODE_LMETA = 0x5B,
    KEYBOARD_SCANCODE_RMETA = 0x5C,
    KEYBOARD_SCANCODE_APPS = 0x5D,

    KEYBOARD_SCANCODE_POWER = 0x5E,
    KEYBOARD_SCANCODE_SLEEP = 0x5F,

    KEYBOARD_SCANCODE_RALT_ALTGR = 0x60,
    KEYBOARD_SCANCODE_PRTSCN = 0x61,

    // 0x62 to 0x66 unused

    KEYBOARD_SCANCODE_HOME = 0x67,
    KEYBOARD_SCANCODE_UP = 0x68,
    KEYBOARD_SCANCODE_PGUP = 0x69,
    KEYBOARD_SCANCODE_VOLUP = 0x6A,
    KEYBOARD_SCANCODE_LEFT = 0x6B,
    KEYBOARD_SCANCODE_VOLMUTE = 0x6C,
    KEYBOARD_SCANCODE_RIGHT = 0x6D,
    KEYBOARD_SCANCODE_VOLDOWN = 0x6E,
    KEYBOARD_SCANCODE_END = 0x6F,
    KEYBOARD_SCANCODE_DOWN = 0x70,
    KEYBOARD_SCANCODE_PGDN = 0x71,
    KEYBOARD_SCANCODE_INS = 0x72,
    KEYBOARD_SCANCODE_DEL = 0x73,

    // 0x74 to 0x7F unused
} __attribute__((__packed__)) keyboard_scancode_t;

#endif /* USER_S65X_KEYBOARD_SCANCODES_H */
