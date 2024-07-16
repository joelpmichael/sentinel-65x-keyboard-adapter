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

// task handle for s65x_next_word_task
static TaskHandle_t s65x_next_word_task_handle = NULL;
// next word to be sent, retrieved by s65x_next_word_task
static uint16_t s65x_next_word = 0xFFFF;

// task to retrieve the next word to be sent by the controller
void s65x_next_word_task(void *pvParameters) {
    // set task handle inside the task once task starts running
    s65x_next_word_task_handle = xTaskGetCurrentTaskHandle();

    // round-robin position in enabled controller devices
    s65x_controller_devices_t next_device = 0;

    // cycle through all controller devices until one returns data
    while (1) {
#ifdef HAS_CONTROLLER_PAD
#error -DHAS_CONTROLLER_PAD not yet implemented
        if (next_device == CONTROLLER_PAD) {
            // PAD returns 3 words per poll
            for (uint8_t i = 0; i < 3; i++) {
                if (s65x_pad_get_next_word(&s65x_next_word) == true) {
                    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
                }
            }
            next_device++;
        }
#endif
#ifdef HAS_CONTROLLER_MOUSE
#error -DHAS_CONTROLLER_MOUSE not yet implemented
        if (next_device == CONTROLLER_MOUSE) {
            // MOUSE returns 2 words per poll
            for (uint8_t i = 0; i < 2; i++) {
                if (s65x_mouse_get_next_word(&s65x_next_word) == true) {
                    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
                }
            }
            next_device++;
        }
#endif
#ifdef HAS_CONTROLLER_KEYBOARD
        if (next_device == CONTROLLER_KEYBOARD) {
            // KEYBOARD returns 1 word per poll
            if (s65x_keyboard_get_next_word(&s65x_next_word) == true) {
                xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
            }
            next_device++;
        }
#endif
        if (next_device == NUM_CONTROLLER_DEVICES) {
            next_device = 0;
        }
    }
}

// current word being transmitted
static volatile uint16_t s65x_current_word = 0xFFFF;

// IRQ callback
// called on rising edge of CLOCK
void s65x_controller_on_clock(void) __attribute__((hot, section(".ramfunc")));
void s65x_controller_on_clock(void) {
    // disable interrupts, run entire IRQ handler inside a critical section
    portENTER_CRITICAL();

    // transmit MSB-first
    // signal level is inverted from bit value
    if ((s65x_current_word & 0x8000) == 0)
        GPIO_WriteBit(GPIOB, GPIO_Pin_2, Bit_SET);
    else
        GPIO_WriteBit(GPIOB, GPIO_Pin_2, Bit_RESET);

    // who needs a counter? just bit-shift and always grab the MSB
    s65x_current_word <<= 1;

    // fill with 1's, if the buffer underflows it will send
    // 0xFFFF which is the UNDERFLOW error
    s65x_current_word |= 1;

    // enable interrupts
    portEXIT_CRITICAL();
}

// IRQ callback
// called on rising edge of LATCH
void s65x_controller_on_latch(void) __attribute__((hot, section(".ramfunc")));
void s65x_controller_on_latch(void) {
    // disable interrupts while setting the current word
    portENTER_CRITICAL();

    // copy next word to current word
    s65x_current_word = s65x_next_word;

    // enable interrupts
    portEXIT_CRITICAL();

    // wake s65x_next_word_task if it's running (or hasn't been disabled)
    if (s65x_next_word_task_handle != NULL) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(s65x_next_word_task_handle, 0, eNoAction, &xHigherPriorityTaskWoken);
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
    // PB0 = SNES_CLOCK (input floating EXTI0 rising-edge)
    // PB1 = SNES_LATCH (input floating EXTI1 rising-edge)
    // PB2 = SNES_DATA (output push-pull)

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_StructInit(&GPIO_InitStructure);

    // allowed to modify multiple pins at once, hooray!
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

    // setup EXTI lines
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    EXTI_StructInit(&EXTI_InitStructure);
    EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init(&EXTI_InitStructure);

    // clear pending EXTI in case it's been set
    EXTI_ClearITPendingBit(EXTI_Line0 | EXTI_Line1);
    NVIC_ClearPendingIRQ(EXTI0_IRQn);
    NVIC_ClearPendingIRQ(EXTI1_IRQn);

    // enable IRQ
    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);

    // create task to perform fetching of the next word to be sent
    // see s65x_next_word_task() for details
    if (s65x_next_word_task_handle == NULL)
        if (xTaskCreate(s65x_next_word_task,
                        "s65x_next_word",
                        64,
                        NULL,
                        1,
                        NULL) != pdPASS)
            return false;

            // init peripherals that the controller emulates
#ifdef HAS_CONTROLLER_PAD
    if (s65x_controller_pad_init() == false)
        return false;
#endif
#ifdef HAS_CONTROLLER_MOUSE
    if (s65x_controller_mouse_init() == false)
        return false;
#endif
#ifdef HAS_CONTROLLER_KEYBOARD
    if (s65x_controller_keyboard_init() == false)
        return false;
#endif

    // init completed successfully
    s65x_controller_init_complete = true;
    return true;
}

// if the startup fails for whatever reason, there might be enough init code
// run to signal the POST fail code forever
void s65x_controller_post_fail(void) {
    while (1) {
        // never run the next-word task again
        s65x_next_word_task_handle = NULL;
        // repeat the POST fail code forever
        s65x_next_word = 0xFEFF;
#ifdef DEBUG
        // drop to debugger in debug builds
        __EBREAK();
#endif
        // do nothing else until an IRQ occurs
        __WFI();
    }
}

// if the system fails during run-time, spin here and send the RUN fail code
void s65x_controller_run_fail(void) {
    while (1) {
        // never run the next-word task again
        s65x_next_word_task_handle = NULL;
        // repeat the RUN fail code forever
        s65x_next_word = 0xFDFF;
#ifdef DEBUG
        // drop to debugger in debug builds
        __EBREAK();
#endif
        // do nothing else until an IRQ occurs
        __WFI();
    }
}
