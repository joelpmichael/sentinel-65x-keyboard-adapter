/*
 * s65x_controller_pad.h
 *
 *  Created on: 7 Jul 2024
 *      Author: joel
 */

#ifndef USER_S65X_CONTROLLER_PAD_H_
#define USER_S65X_CONTROLLER_PAD_H_

#include "stdatomic.h"
#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"

bool s65x_pad_get_next_word(uint16_t *next_word);
bool s65x_controller_pad_init(void);

#endif /* USER_S65X_CONTROLLER_PAD_H_ */
