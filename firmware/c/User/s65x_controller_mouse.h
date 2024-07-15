/*
 * s65x_controller_mouse.h
 *
 *  Created on: 7 Jul 2024
 *      Author: joel
 */

#ifndef USER_S65X_CONTROLLER_MOUSE_H_
#define USER_S65X_CONTROLLER_MOUSE_H_

#include "stdatomic.h"
#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"

bool s65x_mouse_get_next_word(uint16_t *next_word) __attribute__((hot, section(".ramfunc")));
bool s65x_controller_mouse_init(void) __attribute__((section(".slowfunc")));

#endif /* USER_S65X_CONTROLLER_MOUSE_H_ */
