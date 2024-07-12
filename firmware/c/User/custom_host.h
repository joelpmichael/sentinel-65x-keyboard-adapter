/*
 * custom_host.h
 *
 *  Created on: 12 Jul 2024
 *      Author: joel
 */

#ifndef USER_CUSTOM_HOST_H_
#define USER_CUSTOM_HOST_H_

#include "stdatomic.h"
#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"

typedef enum custom_devices_e {
#ifdef HAS_CUSTOM_PAD
    CUSTOM_PAD,
#endif
#ifdef HAS_CUSTOM_MOUSE
    CUSTOM_MOUSE,
#endif
#ifdef HAS_CUSTOM_KEYBOARD
    CUSTOM_KEYBOARD,
#endif
    NUM_CUSTOM_DEVICES
} custom_devices_t;

bool custom_host_init(void);

#endif /* USER_CUSTOM_HOST_H_ */
