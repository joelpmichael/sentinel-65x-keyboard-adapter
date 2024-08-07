/*
 * usb_device_cdc-acm.c
 *
 *  Created on: 7 Jul 2024
 *      Author: joel
 */

#include "usb_device_cdc-acm.h"

#include "ch32v20x.h"
#include "ch32v20x_usb.h"
#include "usb_device.h"

#include "FreeRTOS.h"
#include "task.h"

static TaskHandle_t cdcacm_task_handle = NULL;

__attribute__((section(".ramfunc"))) static void cdcacm_in_cb(void) {
    // wake cdcacm_task if it's running (or hasn't been disabled)
    if (cdcacm_task_handle != NULL) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(cdcacm_task_handle, 0, eSetBits, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

__attribute__((section(".ramfunc"))) static void cdcacm_out_cb(uint8_t *buf, uint16_t len) {
    // wake cdcacm_task if it's running (or hasn't been disabled)
    if (cdcacm_task_handle != NULL) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(cdcacm_task_handle, (0 | (len << 16)), eSetBits, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

__attribute__((section(".ramfunc"))) static void cdcacm_err_cb(void) {
    // wake cdcacm_task if it's running (or hasn't been disabled)
    if (cdcacm_task_handle != NULL) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(cdcacm_task_handle, 0, eSetBits, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

__attribute__((section(".ramfunc"))) static void cdcacm_ovf_cb(void) {
    // wake cdcacm_task if it's running (or hasn't been disabled)
    if (cdcacm_task_handle != NULL) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(cdcacm_task_handle, 0, eSetBits, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

static usb_endpoint_t cdcacm_endpoint_bulk = {
    .type = ENDPOINT_IN_OUT_SINGLE,
    .mode = ENDPOINT_MODE_BULK,
    .in_xfer = cdcacm_in_cb,
    .out_xfer = cdcacm_out_cb,
    .err = cdcacm_err_cb,
    .ovf = cdcacm_ovf_cb,
    .sof = NULL,
    .in_endpoint = 0x00,
    .out_endpoint = 0x00,
    .txd_buf = NULL,
    .rxd_buf = NULL,
    .frame_maxlen = 64,
    .ep_num = 0,
};

static usb_endpoint_t cdcacm_endpoint_irq = {
    .type = ENDPOINT_IN_OUT_SINGLE,
    .mode = ENDPOINT_MODE_IRQ,
    .in_xfer = cdcacm_in_cb,
    .out_xfer = cdcacm_out_cb,
    .err = cdcacm_err_cb,
    .ovf = cdcacm_ovf_cb,
    .sof = NULL,
    .in_endpoint = 0x00,
    .out_endpoint = 0x00,
    .txd_buf = NULL,
    .rxd_buf = NULL,
    .frame_maxlen = 8,
    .ep_num = 0,
};

bool usb_device_cdcacm_init(void) {

    if (usbd_register_endpoint(&cdcacm_endpoint_bulk) == false)
        return false;
    if (usbd_register_endpoint(&cdcacm_endpoint_irq) == false)
        return false;

    return true;
}
