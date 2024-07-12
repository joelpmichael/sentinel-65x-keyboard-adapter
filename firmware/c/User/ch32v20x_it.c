/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32v20x_it.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/12/29
 * Description        : Main Interrupt Service Routines.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "ch32v20x_it.h"
#include "ch32v20x_conf.h"

// All interrupt handler names can be found in startup_ch32v20x_*.S

void NMI_Handler(void) __attribute__((interrupt()));
void HardFault_Handler(void) __attribute__((interrupt()));
void EXTI0_IRQHandler(void) __attribute__((interrupt()));
void EXTI1_IRQHandler(void) __attribute__((interrupt()));
void EXTI2_IRQHandler(void) __attribute__((interrupt()));
void EXTI3_IRQHandler(void) __attribute__((interrupt()));
void EXTI4_IRQHandler(void) __attribute__((interrupt()));
void EXTI9_5_IRQHandler(void) __attribute__((interrupt()));
void EXTI15_10_IRQHandler(void) __attribute__((interrupt()));

/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles NMI exception.
 *
 * @return  none
 */
void NMI_Handler(void) {
    while (1) {
    }
}

/*******************************************************************************
 * @fn        HardFault_Handler
 *
 * @brief     This function handles Hard Fault exception.
 *
 * @return    None
 */
void HardFault_Handler(void) {
    while (1) {
    }
}

// GPIO EXTI handlers
// The CH32V20x has a pretty nice GPIO EXTI system
// EXTI[4:0] each have dedicated IRQ lines, EXTI[9:5] share a single IRQ line
// and EXTI[15:10] share a single IRQ line.
// GPIO pins PA[n], PB[n], PC[n], PD[n] and PE[n] are all muxed into EXTI[n]
#define EXTI_COUNT 16

static exti_handler exti_handlers[EXTI_COUNT] = {NULL};

// set EXTI handler for EXTI number
void set_exti_handler(uint8_t exti_num, exti_handler handler) {
    if (exti_num >= EXTI_COUNT)
        return;
    exti_handlers[exti_num] = handler;
}

// clear EXTI handler for EXTI number by setting to NULL
void clear_exti_handler(uint8_t exti_num) {
    set_exti_handler(exti_num, NULL);
}

// run EXTI handler callback if set
// called from IRQ handler
void run_exti_handler(uint8_t exti_num) {
    // ensure not trying to run an undefined EXTI
    if (exti_num >= EXTI_COUNT)
        return;

    // check if this EXTI has actually fired (for EXTI9_5 and EXTI15_10)
    if (EXTI_GetITStatus(1 << exti_num) == RESET)
        return;

    // this EXTI has fired, run callback if set
    if (exti_handlers[exti_num] != NULL)
        (exti_handlers[exti_num])();

    // clear this EXTI
    EXTI_ClearITPendingBit(1 << exti_num);
}

/*********************************************************************
 * @fn      EXTI0_IRQHandler
 *
 * @brief   This function handles EXTI0 interrupt.
 *
 * @return  none
 */
void EXTI0_IRQHandler(void) {
    run_exti_handler(0);
    NVIC_ClearPendingIRQ(EXTI0_IRQn);
}

/*********************************************************************
 * @fn      EXTI1_IRQHandler
 *
 * @brief   This function handles EXTI1 interrupt.
 *
 * @return  none
 */
void EXTI1_IRQHandler(void) {
    run_exti_handler(1);
    NVIC_ClearPendingIRQ(EXTI1_IRQn);
}

/*********************************************************************
 * @fn      EXTI2_IRQHandler
 *
 * @brief   This function handles EXTI2 interrupt.
 *
 * @return  none
 */
void EXTI2_IRQHandler(void) {
    run_exti_handler(2);
    NVIC_ClearPendingIRQ(EXTI2_IRQn);
}

/*********************************************************************
 * @fn      EXTI3_IRQHandler
 *
 * @brief   This function handles EXTI3 interrupt.
 *
 * @return  none
 */
void EXTI3_IRQHandler(void) {
    run_exti_handler(3);
    NVIC_ClearPendingIRQ(EXTI3_IRQn);
}

/*********************************************************************
 * @fn      EXTI4_IRQHandler
 *
 * @brief   This function handles EXTI4 interrupt.
 *
 * @return  none
 */
void EXTI4_IRQHandler(void) {
    run_exti_handler(4);
    NVIC_ClearPendingIRQ(EXTI4_IRQn);
}

/*********************************************************************
 * @fn      EXTI9_5_IRQHandler
 *
 * @brief   This function handles EXTI9_5 interrupt.
 *
 * @return  none
 */
void EXTI9_5_IRQHandler(void) {
    for (uint8_t i = 5; i <= 9; i++) {
        run_exti_handler(i);
    }
    NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
}

/*********************************************************************
 * @fn      EXTI15_10_IRQHandler
 *
 * @brief   This function handles EXTI15_10 interrupt.
 *
 * @return  none
 */
void EXTI15_10_IRQHandler(void) {
    for (uint8_t i = 10; i <= 15; i++) {
        run_exti_handler(i);
    }
    NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
}
