/*
 * s65x_controller_port.c
 *
 *  Created on: 7 Jul 2024
 *      Author: joel
 */

#include "s65x_controller.h"
#include "s65x_controller_mouse.h"
#include "s65x_controller_keyboard.h"
#include "s65x_controller_pad.h"

#include "stddef.h"
#include "stdint.h"


static uint8_t controller_devices[NUM_CONTROLLER_DEVICES];

