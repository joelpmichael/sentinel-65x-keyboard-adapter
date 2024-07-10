/*
 * usb_host.h
 *
 *  Created on: 7 Jul 2024
 *      Author: joel
 */

#ifndef USER_USB_HOST_H_
#define USER_USB_HOST_H_

#include "stdatomic.h"
#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"

#undef HAS_USB_PAD
#undef HAS_USB_MOUSE
#define HAS_USB_KEYBOARD

#ifndef HAS_USB_PAD
#ifndef HAS_USB_MOUSE
#ifndef HAS_USB_KEYBOARD
#error At least one of -DHAS_USB_PAD, -DHAS_USB_MOUSE, or -DHAS_USB_KEYBOARD must be enabled
#endif
#endif
#endif

typedef enum usb_devices_e {
#ifdef HAS_USB_PAD
    USB_PAD,
#endif
#ifdef HAS_USB_MOUSE
    USB_MOUSE,
#endif
#ifdef HAS_USB_KEYBOARD
    USB_KEYBOARD,
#endif
    NUM_USB_DEVICES
} usb_devices_t;

bool usb_host_init(void);

#endif /* USER_USB_HOST_H_ */
