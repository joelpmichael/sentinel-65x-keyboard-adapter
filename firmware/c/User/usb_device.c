/*
 * usb_device.c
 *
 *  Created on: 7 Jul 2024
 *      Author: joel
 *
 * based on Rust implementation at https://github.com/ch32-rs/ch32-hal/blob/main/src/usbd.rs
 */

#include "usb_device.h"

#include "ch32v20x.h"
#include "ch32v20x_it.h"
#include "system_ch32v20x.h"

#include "utils.h"

#include "FreeRTOS.h"
#include "task.h"

static usb_device_state_t usbd_state = 0;
static usb_device_state_t usbd_suspended_state = 0;
static TaskHandle_t usbd_ep0_task_handle = NULL;

// shadow the 16-bit SRAM contents in main memory
//
// the BTABLE registers appear to use 128 bytes, but this is due to how
// the SRAM is connected to the system - the low 16 bits in each 32-bit
// address is SRAM, the high 16 bits always reads 0 and ignores writes.
// we have dedicated routines for manipulating the BTABLE, so only need
// to handle the data space of the remaining 448 bits.
// WARNING: using CAN1 and/or CAN2 will reduce the available SRAM by 128
// bytes each.
static uint8_t usbd_sram_shadow[512] __attribute__((aligned(4))) = {0};

// functions for copying in/out of USBD SRAM
// *buf must be within usbd_sram_shadow and 16-bit aligned
// len must be a multiple of 16 bits (2 bytes)
__attribute__((section(".ramfunc"), align(4))) static bool usbd_shadow2sram(uint8_t *buf, uint16_t length) {
    // copy from shadow to SRAM
    // sanity checks
    if ((buf < usbd_sram_shadow) || ((buf + length) >= (usbd_sram_shadow + 512)) || ((length & 0b1) != 0))
        return false;

    uint32_t offset = buf - usbd_sram_shadow;

    do {
        length -= 4;
        uint32_t *src_ptr = buf + length;
        uint16_t *dst_ptr = (USBD_CAN1_CAN2_SRAM_BASE + (offset * 2)) + (length * 2);
        *dst_ptr = (uint16_t)((uint32_t)(*src_ptr & 0xFFFF));
        *(dst_ptr + 2) = (uint16_t)((uint32_t)((*src_ptr & 0xFFFF0000) >> 16));
    } while (length > 0);

    return true;
}

__attribute__((section(".ramfunc"), align(4))) static bool usbd_sram2shadow(uint8_t *buf, uint16_t length) {
    // copy from SRAM to shadow
    // sanity checks
    // sanity checks
    if ((buf < usbd_sram_shadow) || ((buf + length) >= (usbd_sram_shadow + 512)) || ((length & 0b1) != 0))
        return false;

    uint32_t offset = buf - usbd_sram_shadow;

    do {
        length -= 4;
        uint16_t *src_ptr = (USBD_CAN1_CAN2_SRAM_BASE + (offset * 2)) + (length * 2);
        uint32_t *dst_ptr = buf + length;
        *dst_ptr = (uint32_t)(*src_ptr | (*(src_ptr + 2) << 16));
    } while (length > 0);

    return true;
}

// flags for seeing if CAN1/CAN2 are enabled
// CAN1 and CAN2 share the upper portion of USBD SRAM: CAN1 uses the last
// 128 bytes, CAN2 uses the next-last 128 bytes.  Luckily, CAN2 doesn't
// exist on the CH32V203...
// this is only set at init-time, so make sure CAN is enabled before USBD
static bool can1_enabled = false;
static bool can2_enabled = false;

const USB_DEV_DESCR usb_device_descriptor __attribute__((aligned(4), section(".slowconst"))) = {
    .bLength = sizeof(USB_DEV_DESCR),
    .bDescriptorType = USB_DESCR_TYP_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = USB_DEV_CLASS_COMMUNIC,
    .bDeviceSubClass = 0x02, // USB-CDC-ACM
    .bDeviceProtocol = 0x00, // no class specific protocol
    .bMaxPacketSize0 = 64,   // always 64 for USB2 devices
    .idVendor = 0xBEEF,
    .idProduct = 0xCEBD,
    .bcdDevice = 0x0001,
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 3,
    .bNumConfigurations = 1,
};

typedef enum usbd_ep0_wakeup_reason_e {
    EP0_WAKEUP_REASON_NONE = 0,
    EP0_WAKEUP_REASON_IRQ_TX = 1 << 0,
    EP0_WAKEUP_REASON_IRQ_RX = 1 << 1,
    EP0_WAKEUP_REASON_IRQ_ERR = 1 << 2,
    EP0_WAKEUP_REASON_IRQ_OVF = 1 << 3,
    EP0_WAKEUP_REASON_IRQ_SOF = 1 << 4,
    EP0_WAKEUP_REASON_IRQ_ESOF = 1 << 5,
} usbd_ep0_wakeup_reason_t;

__attribute__((section(".ramfunc"))) static void usbd_ep0_in_cb(void) {
    // wake usbd_ep0_task if it's running (or hasn't been disabled)
    if (usbd_ep0_task_handle != NULL) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(usbd_ep0_task_handle, EP0_WAKEUP_REASON_IRQ_TX, eSetBits, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

__attribute__((section(".ramfunc"))) static void usbd_ep0_out_cb(uint8_t *buf, uint16_t len) {
    // wake usbd_ep0_task if it's running (or hasn't been disabled)
    if (usbd_ep0_task_handle != NULL) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(usbd_ep0_task_handle, (EP0_WAKEUP_REASON_IRQ_RX | (len << 16)), eSetBits, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

__attribute__((section(".ramfunc"))) static void usbd_ep0_err_cb(void) {
    // wake usbd_ep0_task if it's running (or hasn't been disabled)
    if (usbd_ep0_task_handle != NULL) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(usbd_ep0_task_handle, EP0_WAKEUP_REASON_IRQ_ERR, eSetBits, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

__attribute__((section(".ramfunc"))) static void usbd_ep0_ovf_cb(void) {
    // wake usbd_ep0_task if it's running (or hasn't been disabled)
    if (usbd_ep0_task_handle != NULL) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(usbd_ep0_task_handle, EP0_WAKEUP_REASON_IRQ_OVF, eSetBits, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

static usb_endpoint_t *usbd_endpoints[NUM_USBD_ENDPOINTS] = {NULL};
static usb_endpoint_t usbd_ep0_endpoint = {
    .type = ENDPOINT_IN_OUT_SINGLE,
    .mode = ENDPOINT_MODE_CONTROL,
    .in_xfer = usbd_ep0_in_cb,
    .out_xfer = usbd_ep0_out_cb,
    .err = usbd_ep0_err_cb,
    .ovf = usbd_ep0_ovf_cb,
    .sof = NULL,
    .in_endpoint = 0x00,
    .out_endpoint = 0x00,
    .txd_buf = NULL,
    .rxd_buf = NULL,
    .frame_maxlen = 64,
    .ep_num = 0,
};

static uint8_t num_endpoints = 1;

__attribute__((section(".slowfunc"))) bool usbd_register_endpoint(usb_endpoint_t *endpoint) {
    uint8_t this_endpoint_num = 0;
    if (endpoint != &usbd_ep0_endpoint) {
        this_endpoint_num = num_endpoints;
        // don't increment num_endpoints until the end of configuration
    }
    if (this_endpoint_num >= NUM_USBD_ENDPOINTS)
        return false;

    // sanity-check endpoint struct
    // at a bare minimum, endpoints need either a TX or RX callback
    // and length needs to be even, greater than 0, and <= 512
    // when length > 64, the 5 LSBs need to be 0
    // additionally, this module requires lengths to be 32-bit aligned
    if ((endpoint->type == ENDPOINT_TYPE_NONE) || ((endpoint->in_xfer == NULL) && (endpoint->out_xfer == NULL)) || ((endpoint->frame_maxlen & 0b111) != 0) || (endpoint->frame_maxlen == 0) || (endpoint->frame_maxlen > 512))
        return false;

    // USBD BTABLE configuration
    // Only guaranteed the first 256 bytes of SRAM
    // First 64 bytes is the BTABLE
    // Next bytes is EP0 IN (tx)
    // Next bytes is EP0 OUT (rx)
    // Next bytes are further endpoints
    uint16_t usbd_btable_next_free = USBD_BTABLE_END;
    if (this_endpoint_num > 0) {
        usbd_btable_next_free += (usbd_ep0_endpoint.frame_maxlen * 2);

        for (uint8_t i = 1; i < NUM_USBD_ENDPOINTS; i++) {
            if (usbd_endpoints[i] == NULL)
                continue;
            usbd_btable_next_free += (usbd_endpoints[i]->frame_maxlen * 2);
        }
    }

    // check for conflicting CAN devices in required SRAM address space
    if ((usbd_btable_next_free + (endpoint->frame_maxlen * 2)) >= 256) {
        if (can2_enabled == true)
            return false;
    }
    if ((usbd_btable_next_free + (endpoint->frame_maxlen * 2)) >= 384) {
        if (can1_enabled == true)
            return false;
    }
    // check if it will fit into the SRAM at all
    if ((usbd_btable_next_free + (endpoint->frame_maxlen * 2)) >= 512)
        return false;

    // end of sanity checks, commit endpoint
    usbd_endpoints[this_endpoint_num] = endpoint;
    if (this_endpoint_num > 0)
        num_endpoints++;
    endpoint->ep_num = this_endpoint_num;
    USBD_BTABLE_EP(this_endpoint_num)->ADDR_TX = usbd_btable_next_free;
    endpoint->txd_buf = (uint8_t *)(usbd_sram_shadow + usbd_btable_next_free);
    USBD_BTABLE_EP(this_endpoint_num)->ADDR_RX = usbd_btable_next_free + endpoint->frame_maxlen;
    endpoint->rxd_buf = (uint8_t *)(usbd_sram_shadow + usbd_btable_next_free + endpoint->frame_maxlen);

    USBD_BTABLE_EP(this_endpoint_num)->COUNT_TX = 0;
    // determine stuffing of COUNT_RX bits
    // calculate USBD_COUNTx_RX high bits
    uint16_t count_rx = 0;
    if (endpoint->frame_maxlen < 64)
        count_rx = ((endpoint->frame_maxlen >> 1) << 10); // guaranteed to not be 0
    else {
        // when length >= 64, the 5 LSBs need to be 0 (divisible by 32)
        if ((endpoint->frame_maxlen & 0b11111) != 0)
            return false;
        count_rx = (1 << 15) | (((endpoint->frame_maxlen >> 5) - 1) << 10); // guaranteed to be <= 512
    }

    switch (endpoint->type) {
    case ENDPOINT_OUT_DOUBLE: {
        USBD_BTABLE_EP(this_endpoint_num)->COUNT_TX = count_rx;
        // intentional fall-through
    }
    case ENDPOINT_IN_OUT_SINGLE: {
        USBD_BTABLE_EP(this_endpoint_num)->COUNT_RX = count_rx;
        break;
    }
    case ENDPOINT_IN_DOUBLE: {
        USBD_BTABLE_EP(this_endpoint_num)->COUNT_RX = 0;
        break;
    }
    }

    memset32(endpoint->txd_buf, (0xF00FF000 | this_endpoint_num), endpoint->frame_maxlen);
    memset32(endpoint->rxd_buf, (0x0FF00F00 | (this_endpoint_num << 4)), endpoint->frame_maxlen);
    usbd_shadow2sram(endpoint->txd_buf, endpoint->frame_maxlen);
    usbd_shadow2sram(endpoint->rxd_buf, endpoint->frame_maxlen);

    return true;
}

__attribute__((section(".ramfunc"))) static bool usbd_reset(void) {
    // mask RESET interrupt and clear any pending USBD IRQs
    USBD->CNTR &= ~(USBD_CNTR_RESETM);
    USBD->ISTR = 0;
    NVIC_ClearPendingIRQ(USB_HP_CAN1_TX_IRQn);
    NVIC_ClearPendingIRQ(USB_LP_CAN1_RX0_IRQn);
    NVIC_ClearPendingIRQ(USBWakeUp_IRQn);

    // enter reset and clear all control bits
    USBD->CNTR = USBD_CNTR_FRES;

    // busy-poll for reset IRQ
    while ((USBD->ISTR & USBD_ISTR_RESET) == 0)
        ;

    // release reset - can't configure endpoint registers while in reset
    USBD->CNTR = 0;

    // busy-poll for end of reset
    while ((USBD->ISTR & USBD_ISTR_RESET) != 0)
        USBD->ISTR = 0;

    // Clear device address (should already be done) and enable device function
    USBD->DADDR = USBD_DADDR_EF;

    // reset all endpoints
    for (register uint8_t i = 0; i < num_endpoints; i++) {
        // make sure endpoint register is zeroed
        // any toggle-bits that are currently set will clear themselves
        // clear all the other bits so they reset to zero
        // WARNING: all other usage of USBD_EPR(x)->EPR = Y should use the USBD_EPR_INVARIANT(x) macro!
        USBD_EPR(i)->EPR &= (USBD_EPR_DTOG_RX | USBD_EPR_STAT_RX_MASK | USBD_EPR_DTOG_TX | USBD_EPR_STAT_RX_MASK);
        USBD_EPR(i)->EPR = ((USBD_EPR_INVARIANT(i) & ~(USBD_EPR_EA_MASK)) | i);
        USBD_BTABLE_EP(i)->COUNT_TX = 0;
        USBD_BTABLE_EP(i)->COUNT_RX = 0;

        if (usbd_endpoints[i] == NULL)
            continue;

        // determine stuffing of COUNT_RX bits
        // calculate USBD_COUNTx_RX high bits
        uint16_t count_rx = 0;
        if (usbd_endpoints[i]->frame_maxlen < 64)
            count_rx = ((usbd_endpoints[i]->frame_maxlen >> 1) << 10); // guaranteed to not be 0
        else {
            // when length >= 64, the 5 LSBs need to be 0 (divisible by 32)
            if ((usbd_endpoints[i]->frame_maxlen & 0b11111) != 0)
                return false;
            count_rx = (1 << 15) | (((usbd_endpoints[i]->frame_maxlen >> 5) - 1) << 10); // guaranteed to be <= 512
        }

        USBD_EPR(i)->EPR = ((USBD_EPR_INVARIANT(i) & ~(USBD_EPR_EPTYPE_MASK)) | usbd_endpoints[i]->mode);

        switch (usbd_endpoints[i]->type) {
        case ENDPOINT_IN_OUT_SINGLE: {
            USBD_EPR(i)->EPR = USBD_EPR_STAT_RX(i, USBD_EPR_STAT_RX_ACK);
            USBD_EPR(i)->EPR = USBD_EPR_STAT_TX(i, USBD_EPR_STAT_TX_NAK);

            USBD_BTABLE_EP(i)->COUNT_RX = count_rx;

            break;
        }
        case ENDPOINT_IN_DOUBLE: {
            break;
        }
        case ENDPOINT_OUT_DOUBLE: {
            break;
        }
        default: {
            break;
        }
        }
    }

    // set interrupts to be generated
    USBD->CNTR |= (USBD_CNTR_CTRM | USBD_CNTR_WKUPM | USBD_CNTR_SUSPM | USBD_CNTR_RESETM);

    // clear initial interrupts
    USBD->ISTR = 0;
    NVIC_ClearPendingIRQ(USB_HP_CAN1_TX_IRQn);
    NVIC_ClearPendingIRQ(USB_LP_CAN1_RX0_IRQn);
    NVIC_ClearPendingIRQ(USBWakeUp_IRQn);

    usbd_state = USB_DEVICE_STATE_DEFAULT;

    return true;
}

// USBD IRQ callbacks
__attribute__((section(".ramfunc"))) static void usbd_irq_cb(void) {
    register uint8_t endpoint = USBD->ISTR & USBD_ISTR_EP_ID_MASK;
    USBD->ISTR &= ~(USBD_ISTR_EP_ID_MASK);

    // figure out which IRQ was called
    // PROCESSING ORDER IS IMPORTANT!

    if ((USBD->ISTR & USBD_ISTR_SUSP) > 0) {
        // suspend interrupt received: ESOF for 3ms
        if (usbd_state != USB_DEVICE_STATE_SUSPENDED)
            usbd_suspended_state = usbd_state;
        usbd_state = USB_DEVICE_STATE_SUSPENDED;
        USBD->CNTR |= (USBD_CNTR_FSUSP | USBD_CNTR_LPMODE);
        USBD->ISTR &= ~(USBD_ISTR_SUSP);
    }

    if ((USBD->ISTR & USBD_ISTR_WKUP) > 0) {
        // wakeup frame received
        if (usbd_state == USB_DEVICE_STATE_SUSPENDED)
            usbd_state = usbd_suspended_state;
        USBD->CNTR &= ~(USBD_CNTR_FSUSP | USBD_CNTR_LPMODE);
        USBD->ISTR &= ~(USBD_ISTR_WKUP);
    }

    if ((USBD->ISTR & USBD_ISTR_ESOF) > 0) {
        // SOF missed
        USBD->ISTR &= ~(USBD_ISTR_ESOF);
    }

    if ((USBD->ISTR & USBD_ISTR_SOF) > 0) {
        // start of frame received
        // run endpoint's callback if set
        if (usbd_endpoints[endpoint]->sof != NULL)
            (usbd_endpoints[endpoint]->sof)();

        USBD->ISTR &= ~(USBD_ISTR_SOF);
    }

    if ((USBD->ISTR & USBD_ISTR_ERR) > 0) {
        // errored
        // run endpoint's callback if set
        if (usbd_endpoints[endpoint]->err != NULL)
            (usbd_endpoints[endpoint]->err)();
        USBD->ISTR &= ~(USBD_ISTR_ERR);
    }

    if ((USBD->ISTR & USBD_ISTR_PMAOVR) > 0) {
        // xfer buffer overflow (too large to RX)
        // run endpoint's callback if set
        if (usbd_endpoints[endpoint]->ovf != NULL)
            (usbd_endpoints[endpoint]->ovf)();
        USBD->ISTR &= ~(USBD_ISTR_PMAOVR);
    }

    if ((USBD->ISTR & USBD_ISTR_CTR) > 0) {
        // transfer completed successfully
        // be careful updating endpoint register, because some bits are toggles
        // run endpoint's callbacks if set
        if ((USBD_EPR(endpoint)->EPR & USBD_EPR_CTR_RX) != 0) {
            uint16_t count = (USBD_BTABLE_EP(endpoint)->COUNT_RX & 0x1FF);
            if (usbd_endpoints[endpoint] != NULL) {
                if (usbd_endpoints[endpoint]->rxd_buf >= usbd_sram_shadow && (usbd_endpoints[endpoint]->rxd_buf + count) < (usbd_sram_shadow + 512))
                    usbd_sram2shadow(usbd_endpoints[endpoint]->rxd_buf, usbd_endpoints[endpoint]->frame_maxlen);
                if (usbd_endpoints[endpoint]->out_xfer != NULL)
                    (usbd_endpoints[endpoint]->out_xfer)(usbd_endpoints[endpoint]->rxd_buf, count);
            }
            // clear CTR_RX bit
            USBD_EPR(endpoint)->EPR = (USBD_EPR_INVARIANT(endpoint) & ~(USBD_EPR_CTR_RX));
        }
        if ((USBD_EPR(endpoint)->EPR & USBD_EPR_CTR_TX) != 0) {
            if (usbd_endpoints[endpoint] != NULL) {
                if (usbd_endpoints[endpoint]->in_xfer != NULL)
                    (usbd_endpoints[endpoint]->in_xfer)();
            }
            USBD_EPR(endpoint)->EPR = (USBD_EPR_INVARIANT(endpoint) & ~(USBD_EPR_CTR_TX));
        }

        USBD->ISTR &= ~(USBD_ISTR_CTR);
    }

    if ((USBD->ISTR & USBD_ISTR_RESET) > 0) {
        // reset frame received
        usbd_reset();
        USBD->ISTR &= ~(USBD_ISTR_RESET);
    }
}

__attribute__((section(".ramfunc"))) static void usbd_hp_irq_cb(void) {
    __NOP();
}

__attribute__((section(".ramfunc"))) static void usbd_lp_irq_cb(void) {
    __NOP();
}

// endpoint->txd_buf must be populated before calling
bool usbd_transmit(usb_endpoint_t *endpoint, uint16_t len) {
    usbd_shadow2sram(endpoint->txd_buf, endpoint->frame_maxlen);
    USBD_BTABLE_EP(endpoint->ep_num)->COUNT_TX = (uint16_t)MIN((int32_t)len, (int32_t)endpoint->frame_maxlen);
    USBD_EPR(endpoint->ep_num)->EPR = USBD_EPR_STAT_TX(endpoint->ep_num, USBD_EPR_STAT_TX_ACK);
}

void usbd_ep0_handle_setup(PUSB_SETUP_REQ usb_setup_request) {
    bool setup_ok = false;
    switch (usb_setup_request->bRequest) {
    case USB_CLEAR_FEATURE: {
        if (usb_setup_request->bRequestType > 2)
            break;
        // handle USB_CLEAR_FEATURE
        setup_ok = true;
        break;
    }
    case USB_GET_CONFIGURATION: {
        if (usb_setup_request->bRequestType != 0b10000000)
            break;
        // handle USB_GET_CONFIGURATION
        setup_ok = true;
        break;
    }
    case USB_GET_DESCRIPTOR: {
        // sanity-check: GET_DESCRIPTOR is only valid for device-to-host, standard, device
        if (usb_setup_request->bRequestType != 0b10000000)
            break;
        setup_ok = true;
        uint8_t descriptor_type = (uint8_t)((uint16_t)(usb_setup_request->wValue & 0xFF00) >> 8);
        uint8_t descriptor_index = (uint8_t)((uint16_t)(usb_setup_request->wValue & 0x00FF));

        switch (descriptor_type) {
        case USB_DESCR_TYP_DEVICE: {
            memcpy32(usbd_ep0_endpoint.txd_buf, &usb_device_descriptor, (uint32_t)MIN((int32_t)usbd_ep0_endpoint.frame_maxlen, (int32_t)sizeof(USB_DEV_DESCR)));
            usbd_transmit(&usbd_ep0_endpoint, (uint16_t)MIN((int32_t)usbd_ep0_endpoint.frame_maxlen, (int32_t)sizeof(USB_DEV_DESCR)));
            break;
        }
        default: {
            setup_ok = false;
            break;
        }
        }
        break;
    }
    case USB_GET_INTERFACE: {
        break;
    }
    case USB_GET_STATUS: {
        break;
    }
    default: {
        break;
    }
    }
    if (setup_ok == false)
        usbd_reset();
}

void usbd_ep0_task(void *pvParameters) {
    // set task handle inside the task once task starts running
    usbd_ep0_task_handle = xTaskGetCurrentTaskHandle();

    // perform USBD reset to start device running
    while (usbd_reset() == false)
        vTaskDelay(pdMS_TO_TICKS(1000));

    BaseType_t wakeup_status = EP0_WAKEUP_REASON_NONE;
    while (1) {
        // wait until an EP0 IRQ wakes us up (roll over every 1s anyway)
        xTaskNotifyWait(0xFFFFFFFF, 0, &wakeup_status, pdMS_TO_TICKS(1000));

        if (wakeup_status == EP0_WAKEUP_REASON_NONE)
            continue;

        // TX data interrupt
        if ((wakeup_status & EP0_WAKEUP_REASON_IRQ_TX) != 0) {
            __NOP();
        }

        // RX data interrupt
        if ((wakeup_status & EP0_WAKEUP_REASON_IRQ_RX) != 0) {
            uint16_t len = (wakeup_status & 0xFFFF0000) >> 16;
            uint8_t *buf = usbd_endpoints[0]->rxd_buf;

            if ((USBD_EPR(0)->EPR & USBD_EPR_SETUP) != 0) {
                // SETUP frame
                if (len == 8)
                    // reset indentation!
                    usbd_ep0_handle_setup((PUSB_SETUP_REQ)buf);
            }
        }

        // ERR interrupt
        if ((wakeup_status & EP0_WAKEUP_REASON_IRQ_ERR) != 0) {
            __NOP();
        }

        // buffer-overflow interrupt
        if ((wakeup_status & EP0_WAKEUP_REASON_IRQ_OVF) != 0) {
            __NOP();
        }

        // start of frame interrupt
        if ((wakeup_status & EP0_WAKEUP_REASON_IRQ_SOF) != 0) {
            __NOP();
        }

        // start of frame error interrupt
        if ((wakeup_status & EP0_WAKEUP_REASON_IRQ_ESOF) != 0) {
            __NOP();
        }
    }
}

// flag for usb device init completion
static bool usb_device_init_complete = false;

// init USBD peripheral
//
// this MUST be called before any endpoints are added using
// usbd_register_endpoint() because it zeroes the USBD SRAM.
//
// you must also enable CAN1/CAN2 before enabling USBD, as this only checks
// once during init.
__attribute__((section(".slowfunc"))) bool usb_device_init(void) {
    // ensure init only runs once, turn it into a no-op if called again
    if (usb_device_init_complete == true)
        return true;

    // no way for us to tell between DETACHED/ATTACHED/POWERED
    usbd_state = USB_DEVICE_STATE_POWERED;

    // init USBD clocks
    // ensure SYS_CLOCK is either 48, 96, or 144MHz
    // and set divider based on clock speed
    uint8_t usb_clock_div = 0b00;

    switch (SystemCoreClock) {
    case 48000000: {
        // no division, already 48MHz clock
        usb_clock_div = 0b00;
        break;
    }
    case 96000000: {
        // div 2 for 48MHz USB clock
        usb_clock_div = 0b01;
        break;
    }
    case 144000000: {
        // div 3 for 48MHz USB clock
        usb_clock_div = 0b10;
        break;
    }
    case 240000000: {
        // div 5 for 48MHz USB clock
        // only for CH32V208D8W and co
        usb_clock_div = 0b11;
        break;
    }
    default: {
        // only supported clock speed are the 4 above!
        return false;
    }
    }

    // set USB divider RCC->CFGR0->USBPRE[1:0]
    RCC->CFGR0 |= (usb_clock_div << 22);

    // use USB-fullspeed (12M) mode
    EXTEN->EXTEN_CTR &= ~(EXTEN_USBD_LS);
    // enable internal USBD pull-up
    EXTEN->EXTEN_CTR |= EXTEN_USBD_PU_EN;

    // detect RCC status of CAN1/CAN2
    // make sure that CAN1/2 is enabled BEFORE USBD!
    if ((RCC->APB1PCENR & RCC_APB1Periph_CAN1) != 0)
        can1_enabled = true;
    if ((RCC->APB1PCENR & RCC_APB1Periph_CAN2) != 0)
        can2_enabled = true;

    // unmask clock and set/clear peripheral reset
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USB, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USB, DISABLE);

    // init USBD peripheral
    // clear all control bits, set force-reset bit
    USBD->CNTR = USBD_CNTR_FRES;

    // TODO: need to wait for 100ms here? Rust implementation does...
    /*
    Delay_Init();
    Delay_Ms(100);
    */

    // set USBD_BTABLE offset address relative to 0x40006000
    // entire BTABLE is 64 bytes of SRAM, but 128 bytes of address
    USBD->BTABLE = 0;

    // clear entire USBD SRAM
    memset32(USBD_CAN1_CAN2_SRAM_BASE, 0x00, 512);
    if (can2_enabled == false) {
        if (memset32(USBD_CAN1_CAN2_SRAM_BASE + 512, 0x00, 256) == NULL)
            return false;
    }
    if (can1_enabled == false) {
        if (memset32(USBD_CAN1_CAN2_SRAM_BASE + 768, 0x00, 256) == NULL)
            return false;
    }

    if (usbd_register_endpoint(&usbd_ep0_endpoint) == false)
        return false;

    // assign callbacks
    set_usbhp_can1tx_handler(usbd_irq_cb);
    set_usblp_can1rx0_handler(usbd_irq_cb);
    set_usbwkp_handler(usbd_irq_cb);

    // enable interrupts
    NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
    NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
    NVIC_EnableIRQ(USBWakeUp_IRQn);

    // start USBD EP0 handling task
    // IRQ-low priority task task
    if (usbd_ep0_task_handle == NULL)
        if (xTaskCreate(usbd_ep0_task,
                        "usbd_ep0",
                        128,
                        NULL,
                        configMAX_PRIORITIES - 2,
                        NULL) != pdPASS)
            return false;

    usb_device_init_complete = true;
    return true;
}
