/*
 * s65x_controller.h
 *
 *  Created on: 7 Jul 2024
 *      Author: joel
 */

#ifndef USER_S65X_CONTROLLER_H_
#define USER_S65X_CONTROLLER_H_

#undef HAS_CONTROLLER_PAD
#undef HAS_CONTROLLER_MOUSE
#define HAS_CONTROLLER_KEYBOARD

typedef enum controller_devices_e {
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
} controller_devices_t;

#endif /* USER_S65X_CONTROLLER_H_ */
