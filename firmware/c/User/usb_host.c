/*
 * usb_host.c
 *
 *  Created on: 14 Aug 2024
 *      Author: joel
 */

#include "usb_host.h"

#include "ch32v20x.h"

#include "FreeRTOS.h"
#include "s65x_controller.h"
#include "task.h"

#include "utils.h"

#include "tusb.h"

static TaskHandle_t usb_host_task_handle = NULL;

#define USB_HOST_STACK_SIZE 256
StaticTask_t usb_host_task_buffer;
StackType_t usb_host_stack[USB_HOST_STACK_SIZE];

void usb_host_task(void *pvParameters) {
    // set task handle inside the task once task starts running
    usb_host_task_handle = xTaskGetCurrentTaskHandle();

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
        tuh_task();
    }

    // never reached
}

/*
__attribute((section(".ramfunc"))) void usbfs_irq_handler(void) {
    tuh_int_handler(BOARD_TUH_RHPORT);
}

__attribute((section(".ramfunc"))) void hcd_int_handler(uint8_t rhport, bool in_isr) {
}
*/

static bool usb_host_init_complete = false;
bool usb_host_init(void) {
    if (usb_host_init_complete == true)
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
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_USBFS, ENABLE); // USB FS

    NVIC_ClearPendingIRQ(USBFS_IRQn);
    NVIC_ClearPendingIRQ(USBFSWakeUp_IRQn);
    NVIC_EnableIRQ(USBFS_IRQn);
    NVIC_EnableIRQ(USBFSWakeUp_IRQn);

    if (usb_host_task_handle == NULL)
        if (xTaskCreateStatic(usb_host_task,
                              "usb_host",
                              USB_HOST_STACK_SIZE,
                              NULL,
                              configMAX_PRIORITIES - 2,
                              usb_host_stack,
                              &usb_host_task_buffer) == NULL)
            return false;

    if (tuh_init(BOARD_TUH_RHPORT) == false)
        return false;
    return true;
}
