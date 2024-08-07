/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32v20x_it.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : This file contains the headers of the interrupt handlers.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#ifndef __CH32V20x_IT_H
#define __CH32V20x_IT_H

#include "debug.h"

typedef void (*exti_handler)(void);
void set_exti_handler(uint8_t exti_num, exti_handler handler) __attribute__((section(".slowfunc")));
void clear_exti_handler(uint8_t exti_num) __attribute__((section(".slowfunc")));

typedef void (*usbhp_can1tx_handler)(void);
typedef void (*usblp_can1rx0_handler)(void);
typedef void (*usbwkp_handler)(void);
typedef void (*usbfs_handler)(void);
typedef void (*usbfswkp_handler)(void);
void set_usbhp_can1tx_handler(usbhp_can1tx_handler handler) __attribute__((section(".slowfunc")));
void clear_usbhp_can1tx_handler(void) __attribute__((section(".slowfunc")));
void set_usblp_can1rx0_handler(usblp_can1rx0_handler handler) __attribute__((section(".slowfunc")));
void clear_usblp_can1rx0_handler(void) __attribute__((section(".slowfunc")));
void set_usbwkp_handler(usbwkp_handler handler) __attribute__((section(".slowfunc")));
void clear_usbwkp_handler(void) __attribute__((section(".slowfunc")));
void set_usbfs_handler(usbfs_handler handler) __attribute__((section(".slowfunc")));
void clear_usbfs_handler(void) __attribute__((section(".slowfunc")));
void set_usbfswkp_handler(usbfswkp_handler handler) __attribute__((section(".slowfunc")));
void clear_usbfswkp_handler(void) __attribute__((section(".slowfunc")));

#endif /* __CH32V20x_IT_H */
