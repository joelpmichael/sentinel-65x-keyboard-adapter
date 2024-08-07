/*
 * usb_device.h
 *
 *  Created on: 7 Jul 2024
 *      Author: joel
 */

#ifndef USER_USB_DEVICE_H_
#define USER_USB_DEVICE_H_

#include "ch32v20x_usb.h"

#include "stdatomic.h"
#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"

// states as defined in Chapter 9
typedef enum usb_device_state_e {
    USB_DEVICE_STATE_DETACHED = 0,
    USB_DEVICE_STATE_ATTACHED,
    USB_DEVICE_STATE_POWERED,
    USB_DEVICE_STATE_DEFAULT,
    USB_DEVICE_STATE_ADDRESS,
    USB_DEVICE_STATE_CONFIGURED,
    USB_DEVICE_STATE_SUSPENDED,
} usb_device_state_t;

// CH32V203 number of endpoints
// the datasheet claims 16, but there's 8 configuration registers
// each configuration register supports an IN/OUT pair
// or a single double-buffered endpoint
#define NUM_USBD_ENDPOINTS 8

// CH32V203 types of endpoints
typedef enum usbd_endpoint_type_e {
    ENDPOINT_TYPE_NONE = 0,
    ENDPOINT_IN_OUT_SINGLE,
    ENDPOINT_IN_DOUBLE,
    ENDPOINT_OUT_DOUBLE,
} usbd_endpoint_type_t;

typedef enum usbd_enpoint_xfer_mode_e {
    ENDPOINT_MODE_BULK = 0b00,
    ENDPOINT_MODE_CONTROL = 0b01,
    ENDPOINT_MODE_ISO = 0b10,
    ENDPOINT_MODE_IRQ = 0b11,
} usbd_endpoint_xfer_mode_t;

typedef void (*in_xfer_cb)(void);
typedef void (*out_xfer_cb)(uint8_t *buf, uint16_t len);
typedef void (*err_cb)(void);
typedef void (*ovf_cb)(void);
typedef void (*sof_cb)(void);

typedef struct usb_endpoint_s {
    usbd_endpoint_type_t type;
    usbd_endpoint_xfer_mode_t mode;
    in_xfer_cb in_xfer;
    out_xfer_cb out_xfer;
    err_cb err;
    ovf_cb ovf;
    sof_cb sof;
    PUSB_ENDP_DESCR in_endpoint;
    PUSB_ENDP_DESCR out_endpoint;
    uint8_t *txd_buf;
    uint8_t *rxd_buf;
    uint16_t frame_maxlen;
    uint8_t ep_num;
} usb_endpoint_t;

bool usbd_register_endpoint(usb_endpoint_t *endpoint) __attribute__((section(".slowfunc")));
bool usb_device_init(void) __attribute__((section(".slowfunc")));
bool usbd_transmit(usb_endpoint_t *endpoint, uint16_t len);

#endif /* USER_USB_DEVICE_H_ */
