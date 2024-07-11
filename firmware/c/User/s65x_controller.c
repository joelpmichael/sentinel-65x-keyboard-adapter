/*
 * s65x_controller.c
 *
 *  Created on: 7 Jul 2024
 *      Author: Joel Michael <joelpmichael@gmail.com>
 *
 * C implementation of the Sentinel 65X Controller Port protocol
 *
 */

#include "ch32v20x_conf.h"

#include "s65x_controller.h"

#include "s65x_controller_keyboard.h"
#include "s65x_controller_mouse.h"
#include "s65x_controller_pad.h"

#include "FreeRTOS.h"
#include "task.h"

// flag for controller port init completion
static bool s65x_controller_init_complete = false;

static TaskHandle_t s65x_next_word_task_handle = NULL;
static uint16_t s65x_next_word = 0xFFFF;
static s65x_controller_devices_t next_device = 0;

void s65x_next_word_task(void *pvParameters) {
    s65x_next_word_task_handle = xTaskGetCurrentTaskHandle();

    // cycle through all controller devices until one returns data
    while (1) {
#ifdef HAS_CONTROLLER_PAD
        if (next_device == CONTROLLER_PAD) {
        }
#endif
#ifdef HAS_CONTROLLER_MOUSE
        if (next_device == CONTROLLER_MOUSE) {
        }
#endif
#ifdef HAS_CONTROLLER_KEYBOARD
        if (next_device == CONTROLLER_KEYBOARD) {
        }
#endif
        if (next_device == NUM_CONTROLLER_DEVICES) {
            next_device = 0;
            continue;
        }
    }
}

static volatile uint16_t s65x_current_word = 0xFFFF;
void s65x_controller_on_clock(void) {
    if (s65x_current_word & 0x8000 == 0)
        GPIO_WriteBit(GPIOB, GPIO_Pin_2, Bit_SET);
    else
        GPIO_WriteBit(GPIOB, GPIO_Pin_2, Bit_RESET);

    s65x_current_word <<= 1;
    s65x_current_word |= 1;
}

void s65x_controller_on_latch(void) {
    s65x_current_word = s65x_next_word;
    if (s65x_next_word_task_handle != NULL) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(s65x_next_word_task_handle, 0, eIncrement, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/*********************************************************************
 * @fn      s65x_controller_init
 *
 * @brief   Init GPIO pins for SNES port communication
 *
 * @return  none
 */
bool s65x_controller_init(void) {
    // ensure init only runs once, turn it into a no-op if called again
    if (s65x_controller_init_complete == true)
        return true;

    // set up GPIO pins
    // PB0 = SNES_CLOCK (input EXTI)
    // PB1 = SNES_LATCH (input EXTI)
    // PB2 = SNES_DATA (output push-pull)

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_StructInit(&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // lowest maximum speed = highest drive level
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // set EXTI callbacks
    set_exti_handler(0, s65x_controller_on_clock);
    set_exti_handler(1, s65x_controller_on_latch);

    // setup EXTI
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    EXTI_StructInit(&EXTI_InitStructure);
    EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init(&EXTI_InitStructure);

    EXTI_ClearITPendingBit(EXTI_Line0 | EXTI_Line1);

    NVIC_ClearPendingIRQ(EXTI0_IRQn);
    NVIC_ClearPendingIRQ(EXTI1_IRQn);

    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);

    if (xTaskCreate(s65x_next_word_task,
                    "s65x_next_word",
                    64,
                    NULL,
                    1,
                    NULL) != pdPASS)
        return false;

    s65x_controller_init_complete = true;
    return true;
}

void s65x_controller_post_fail(void) {
    while (1) {
        s65x_next_word_task_handle = NULL;
        s65x_next_word = 0xFEFF;
        __WFI();
    }
}

void s65x_controller_run_fail(void) {
    while (1) {
        s65x_next_word_task_handle = NULL;
        s65x_next_word = 0xFDFF;
        __WFI();
    }
}
