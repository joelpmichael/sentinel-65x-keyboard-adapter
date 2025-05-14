/*
 * usb_device.c
 *
 *  Created on: 14 Aug 2024
 *      Author: joel
 */

#include "usb_device.h"
#include "ch32v20x.h"

#include "FreeRTOS.h"
#include "task.h"

#include "tusb.h"

#include "utils.h"

static TaskHandle_t usb_device_task_handle = NULL;

#define USB_DEVICE_STACK_SIZE 256
StaticTask_t usb_device_task_buffer;
StackType_t usb_device_stack[USB_DEVICE_STACK_SIZE];

void usb_device_task(void *pvParameters) {
    // set task handle inside the task once task starts running
    usb_device_task_handle = xTaskGetCurrentTaskHandle();

    while (1) {
#ifdef DEBUG
        UBaseType_t hwm = uxTaskGetStackHighWaterMark(NULL);
        if (hwm < 32) {
            // drop to debugger in debug builds
            __EBREAK();
        }
        HeapStats_t hs;
        vPortGetHeapStats(&hs);
        if (hs.xAvailableHeapSpaceInBytes < 1024 || hs.xMinimumEverFreeBytesRemaining < 1024) {
            // drop to debugger in debug builds
            __EBREAK();
        }
#endif

        // put this thread to waiting state until there is new events
        tud_task();

        // following code only run if tud_task() process at least 1 event
        tud_cdc_write_flush();
    }
}

static bool usb_device_init_complete = false;
bool usb_device_init(void) {
    if (usb_device_init_complete == true)
        return true;

    // set system core clock divider for USB
    uint8_t usb_div;
    switch (SystemCoreClock) {
    case 48000000: {
        usb_div = RCC_USBCLKSource_PLLCLK_Div1;
        break;
    }
    case 96000000: {
        usb_div = RCC_USBCLKSource_PLLCLK_Div2;
        break;
    }
    case 144000000: {
        usb_div = RCC_USBCLKSource_PLLCLK_Div3;
        break;
    }
    default: {
        return false;
    }
    }
    RCC_USBCLKConfig(usb_div);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // AFIO is required by USB
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_AFIO, DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE); // FSDEV
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USB, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USB, DISABLE);

    EXTEN->EXTEN_CTR |= EXTEN_USBD_PU_EN; // enable USB-device pull-ups

    NVIC_ClearPendingIRQ(USB_HP_CAN1_TX_IRQn);
    NVIC_ClearPendingIRQ(USB_LP_CAN1_RX0_IRQn);
    NVIC_ClearPendingIRQ(USBWakeUp_IRQn);
    NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
    NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
    NVIC_EnableIRQ(USBWakeUp_IRQn);

    if (usb_device_task_handle == NULL)
        if (xTaskCreateStatic(usb_device_task,
                              "usb_device",
                              USB_DEVICE_STACK_SIZE,
                              NULL,
                              configMAX_PRIORITIES - 2,
                              usb_device_stack,
                              &usb_device_task_buffer) == NULL)
            return false;

    if (tud_init(BOARD_TUD_RHPORT) == false)
        return false;

    usb_device_init_complete = true;
    return true;
}

// Invoked when received new data
void tud_cdc_rx_cb(uint8_t itf) {
    if (tud_cdc_n_connected(itf)) {
        tud_cdc_n_write_str(itf, "tud_cdc_rx_cb\r\n");
        tud_cdc_n_write_flush(itf);
    }
}

// Invoked when received `wanted_char`
void tud_cdc_rx_wanted_cb(uint8_t itf, char wanted_char) {
    if (tud_cdc_n_connected(itf)) {
        tud_cdc_n_write_str(itf, "tud_cdc_rx_wanted_cb\r\n");
        tud_cdc_n_write_flush(itf);
    }
}

// Invoked when a TX is complete and therefore space becomes available in TX buffer
void tud_cdc_tx_complete_cb(uint8_t itf) {
    if (tud_cdc_n_connected(itf)) {
        //tud_cdc_n_write_str(itf, "tud_cdc_tx_complete_cb\r\n");
    }
}

// Invoked when line state DTR & RTS are changed via SET_CONTROL_LINE_STATE
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {
    if (tud_cdc_n_connected(itf)) {
        tud_cdc_n_write_str(itf, "tud_cdc_line_state_cb\r\n");
        if(dtr)
            tud_cdc_n_write_str(itf, "dtr set\r\n");
        if(rts)
            tud_cdc_n_write_str(itf, "rts set\r\n");
        tud_cdc_n_write_flush(itf);
    }
}

// Invoked when line coding is change via SET_LINE_CODING
void tud_cdc_line_coding_cb(uint8_t itf, cdc_line_coding_t const* p_line_coding) {
    if (tud_cdc_n_connected(itf)) {
        tud_cdc_n_write_str(itf, "tud_cdc_line_coding_cb\r\n");
        tud_cdc_n_write_flush(itf);
    }
}

// Invoked when received send break
// \param[in]  itf  interface for which send break was received.
// \param[in]  duration_ms  the length of time, in milliseconds, of the break signal. If a value of FFFFh, then the
//                          device will send a break until another SendBreak request is received with value 0000h.
void tud_cdc_send_break_cb(uint8_t itf, uint16_t duration_ms) {
    if (tud_cdc_n_connected(itf)) {
        tud_cdc_n_write_str(itf, "tud_cdc_send_break_cb\r\n");
        tud_cdc_n_write_flush(itf);
    }
}

__attribute__ ((used)) int _write (int fd, char *buf, int size) {
    (void)fd;
    if(tud_cdc_connected())
        tud_cdc_write(buf, (uint32_t)size);
    tud_cdc_write_flush();
    return size;
}