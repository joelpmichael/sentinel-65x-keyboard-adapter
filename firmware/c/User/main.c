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
#include "task.h"

void gpio_clock_init(void) __attribute__((section(".slowfunc")));
void gpio_clock_init(void) {
    // only init clocks; individual GPIOs are configured elsewhere
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void) {
    // Clock setup

    // core clock is already set during pre-main() code
    // SystemCoreClockUpdate() updates a variable used throughout the SDK
    SystemCoreClockUpdate();

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    // shared peripheral initial setup
    gpio_clock_init();

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

    // USB device setup for config interface
    if (usb_device_init() == false)
        s65x_controller_post_fail();

    // Delay_Init();

    vTaskStartScheduler();

    while (1) {
        s65x_controller_run_fail();
    }
}

// idle hook, just wait for interrupts
void vApplicationIdleHook(void) {
    __WFI();
}

__attribute__((section(".slowfunc"))) void vApplicationMallocFailedHook(void) {
    s65x_controller_run_fail();
}

__attribute__((section(".slowfunc"))) void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    s65x_controller_run_fail();
}

// custom malloc/free overrides, use FreeRTOS to malloc/free
__attribute__((section(".ramfunc"))) void free(void *ptr) {
    if (ptr == NULL)
        return;
    vPortFree(ptr);
}

__attribute__((section(".ramfunc"), malloc, malloc(free))) void *malloc(size_t size) {
    if (size == 0)
        return NULL;
    return pvPortMalloc(size);
}
