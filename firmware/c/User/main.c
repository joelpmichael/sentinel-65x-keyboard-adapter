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

#include "custom_host.h"
#include "ps2_host.h"
#include "s65x_controller.h"
#include "usb_device.h"
#include "usb_host.h"

#include "FreeRTOS.h"
#include "task.h"

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

    // peripheral initial setup
    gpio_clock_init();
    if (s65x_controller_init() == false)
        s65x_controller_post_fail();
    if (ps2_host_init() == false)
        s65x_controller_post_fail();
    if (usb_device_init() == false)
        s65x_controller_post_fail();
    if (usb_host_init() == false)
        s65x_controller_post_fail();
    if (custom_host_init() == false)
        s65x_controller_post_fail();
    // Delay_Init();

    vTaskStartScheduler();

    while (1) {
        printf("shouldn't run at here!!\n");
    }
}
