/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/12/29
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 ******************************************************************************
 * Modified by Joel Michael <joelpmichael@gmail.com> for the
 * Sentinel 65X keyboard adapter project. See LICENSE.md in the top-level
 * project directory.
 ******************************************************************************/

/*
 *@Note
 *
 * Pre- main() code runs from startup_ch32v20x_D8.S!
 *
 * Vector table offset 0 is the CPU entry point, which jumps to ASM function
 * handle_reset(). handle_reset() calls SystemInit() from system_ch32v20x.c,
 * then returns from the exception handler to main().
 *
 */

#include "ch32v20x_conf.h"

#include "s65x_controller.h"

#include "custom_host.h"
#include "ps2_host.h"
#include "usb_host.h"

#include "usb_device.h"

#include "FreeRTOS.h"
#include "debug.h"
#include "task.h"

#include "tusb.h"

// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *report, uint16_t len) {
    uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);

    switch (itf_protocol) {
    case HID_ITF_PROTOCOL_KEYBOARD:
        TU_LOG2("HID receive boot keyboard report\r\n");
        break;

    case HID_ITF_PROTOCOL_MOUSE:
        TU_LOG2("HID receive boot mouse report\r\n");
        break;

    default:
        // Generic report requires matching ReportID and contents with previous parsed report info
        break;
    }

    // continue to request to receive report
    if (!tuh_hid_receive_report(dev_addr, instance)) {
        printf("Error: cannot request to receive report\r\n");
    }
}

void gpio_clock_init(void) __attribute__((section(".slowfunc")));
void gpio_clock_init(void) {
    // only init clocks; individual GPIOs are configured elsewhere
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA, DISABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOB, DISABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOC, DISABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // AFIO needed for USB to work!
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_AFIO, DISABLE);
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    // core clock is already set during pre-main() code
    // SystemCoreClockUpdate() updates a variable used throughout the SDK
    SystemCoreClockUpdate();

    // enable "Flash Enhanced Read Mode"
    // FLASH_Enhance_Mode(ENABLE);

    // shared peripheral initial setup
    gpio_clock_init();

    // set USB lines to output-low-pushpull-highspeed first, prior to USB init
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12; // USB1 DM/DP
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_InitStructure.GPIO_Pin);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // USB2 DM/DP
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_InitStructure.GPIO_Pin);

    // S65X SNES controller port setup
    if (s65x_controller_init() == false)
        s65x_controller_post_fail();

    // only enable PS/2 init if a PS/2 device will be used
#ifdef HAS_PS2_HOST
#undef HAS_PS2_HOST
#endif
#ifdef HAS_PS2_KEYBOARD
#ifndef HAS_PS2_HOST
#define HAS_PS2_HOST
#endif
#endif
#ifdef HAS_PS2_MOUSE
#ifndef HAS_PS2_HOST
#define HAS_PS2_HOST
#endif
#endif
#ifdef HAS_PS2_HOST
    // PS/2 host GPIO setup
    if (ps2_host_init() == false)
        s65x_controller_post_fail();
#endif

// only enable custom host init if a custom device will be used
#ifdef HAS_CUSTOM_HOST
#undef HAS_CUSTOM_HOST
#endif
#ifdef HAS_CUSTOM_KEYBOARD
#ifndef HAS_CUSTOM_HOST
#define HAS_CUSTOM_HOST
#endif
#endif
#ifdef HAS_CUSTOM_MOUSE
#ifndef HAS_CUSTOM_HOST
#define HAS_CUSTOM_HOST
#endif
#endif
#ifdef HAS_CUSTOM_PAD
#ifndef HAS_CUSTOM_HOST
#define HAS_CUSTOM_HOST
#endif
#endif
#ifdef HAS_CUSTOM_HOST
    // custom device setup
    if (custom_host_init() == false)
        s65x_controller_post_fail();
#endif

// only enable USB host init if a USB device will be used
#ifdef HAS_USB_HOST
#undef HAS_USB_HOST
#endif
#ifdef HAS_USB_KEYBOARD
#ifndef HAS_USB_HOST
#define HAS_USB_HOST
#endif
#endif
#ifdef HAS_USB_MOUSE
#ifndef HAS_USB_HOST
#define HAS_USB_HOST
#endif
#endif
#ifdef HAS_USB_PAD
#ifndef HAS_USB_HOST
#define HAS_USB_HOST
#endif
#endif
#ifdef HAS_USB_HOST
    // USB host controller setup
    if (usb_host_init() == false)
        s65x_controller_post_fail();
#endif

    // USB device setup for config interface
    if (usb_device_init() == false)
        s65x_controller_post_fail();

    vTaskStartScheduler();

    while (1) {
        s65x_controller_run_fail();
    }
}

// static memory allocations
static StaticTask_t xIdleTaskTCB, xTimerTaskTCB;
static StackType_t xIdleTaskStack[configMINIMAL_STACK_SIZE];
static StackType_t xTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

__attribute__((section(".slowfunc"))) void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                                                         StackType_t **ppxIdleTaskStackBuffer,
                                                                         uint32_t *pulIdleTaskStackSize) {
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = &xIdleTaskStack[0];
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

__attribute__((section(".slowfunc"))) void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                                                          StackType_t **ppxTimerTaskStackBuffer,
                                                                          uint32_t *pulTimerTaskStackSize) {
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = &xTimerTaskStack[0];
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

// idle hook, just wait for interrupts
__attribute__((section(".ramfunc"))) void
vApplicationIdleHook(void) {
    __WFI();
}

__attribute__((section(".slowfunc"))) void vApplicationMallocFailedHook(void) {
    s65x_controller_run_fail();
}

__attribute__((section(".slowfunc"))) void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    s65x_controller_run_fail();
}

// custom malloc/free overrides, use FreeRTOS to malloc/free
__attribute__((always_inline)) inline void free(void *ptr) {
    vPortFree(ptr);
}

__attribute__((always_inline, malloc)) inline void *malloc(size_t size) {
    if (size == 0)
        return NULL;
    return pvPortMalloc(size);
}
