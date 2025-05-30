/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32v20x_rtc.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/01/06
 * Description        : This file provides all the RTC firmware functions.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "ch32v20x_rtc.h"

/* RTC_Private_Defines */
#define RTC_LSB_MASK ((uint32_t)0x0000FFFF)  /* RTC LSB Mask */
#define PRLH_MSB_MASK ((uint32_t)0x000F0000) /* RTC Prescaler MSB Mask */

/*********************************************************************
 * @fn      RTC_ITConfig
 *
 * @brief   Enables or disables the specified RTC interrupts.
 *
 * @param   RTC_IT - specifies the RTC interrupts sources to be enabled or disabled.
 *            RTC_IT_OW - Overflow interrupt
 *            RTC_IT_ALR - Alarm interrupt
 *            RTC_IT_SEC - Second interrupt
 *
 * @return  NewState - new state of the specified RTC interrupts(ENABLE or DISABLE).
 */
void RTC_ITConfig (uint16_t RTC_IT, FunctionalState NewState) {
    if (NewState != DISABLE) {
        RTC->CTLRH |= RTC_IT;
    } else {
        RTC->CTLRH &= (uint16_t)~RTC_IT;
    }
}

/*********************************************************************
 * @fn      RTC_EnterConfigMode
 *
 * @brief   Enters the RTC configuration mode.
 *
 * @return  none
 */
void RTC_EnterConfigMode (void) {
    RTC->CTLRL |= RTC_CTLRL_CNF;
}

/*********************************************************************
 * @fn      RTC_ExitConfigMode
 *
 * @brief   Exits from the RTC configuration mode.
 *
 * @return  none
 */
void RTC_ExitConfigMode (void) {
    RTC->CTLRL &= (uint16_t) ~((uint16_t)RTC_CTLRL_CNF);
}

/*********************************************************************
 * @fn      RTC_GetCounter
 *
 * @brief   Gets the RTC counter value
 *
 * @return  RTC counter value
 */
uint32_t RTC_GetCounter (void) {
    uint16_t high1a = 0, high1b = 0, high2a = 0, high2b = 0;
    uint16_t low1 = 0, low2 = 0;

    do {
        high1a = RTC->CNTH;
        high1b = RTC->CNTH;
    } while (high1a != high1b);

    do {
        low1 = RTC->CNTL;
        low2 = RTC->CNTL;
    } while (low1 != low2);

    do {
        high2a = RTC->CNTH;
        high2b = RTC->CNTH;
    } while (high2a != high2b);

    if (high1b != high2b) {
        do {
            low1 = RTC->CNTL;
            low2 = RTC->CNTL;
        } while (low1 != low2);
    }

    return (((uint32_t)high2b << 16) | low2);
}

/*********************************************************************
 * @fn      RTC_SetCounter
 *
 * @brief   Sets the RTC counter value.
 *
 * @param   CounterValue - RTC counter new value.
 *
 * @return  RTC counter value
 */
void RTC_SetCounter (uint32_t CounterValue) {
    RTC_EnterConfigMode();
    RTC->CNTH = CounterValue >> 16;
    RTC->CNTL = (CounterValue & RTC_LSB_MASK);
    RTC_ExitConfigMode();
}

/*********************************************************************
 * @fn      RTC_SetPrescaler
 *
 * @brief   Sets the RTC prescaler value
 *
 * @param   PrescalerValue - RTC prescaler new value
 *
 * @return  none
 */
void RTC_SetPrescaler (uint32_t PrescalerValue) {
    RTC_EnterConfigMode();
    RTC->PSCRH = (PrescalerValue & PRLH_MSB_MASK) >> 16;
    RTC->PSCRL = (PrescalerValue & RTC_LSB_MASK);
    RTC_ExitConfigMode();
}

/*********************************************************************
 * @fn      RTC_SetAlarm
 *
 * @brief   Sets the RTC alarm value
 *
 * @param   AlarmValue - RTC alarm new value
 *
 * @return  none
 */
void RTC_SetAlarm (uint32_t AlarmValue) {
    RTC_EnterConfigMode();
    RTC->ALRMH = AlarmValue >> 16;
    RTC->ALRML = (AlarmValue & RTC_LSB_MASK);
    RTC_ExitConfigMode();
}

/*********************************************************************
 * @fn      RTC_GetDivider
 *
 * @brief   Gets the RTC divider value
 *
 * @return  RTC Divider value
 */
uint32_t RTC_GetDivider (void) {
    uint16_t high1a = 0, high1b = 0, high2a = 0, high2b = 0;
    uint16_t low1 = 0, low2 = 0;

    do {
        high1a = RTC->DIVH;
        high1b = RTC->DIVH;
    } while (high1a != high1b);

    do {
        low1 = RTC->DIVL;
        low2 = RTC->DIVL;
    } while (low1 != low2);

    do {
        high2a = RTC->DIVH;
        high2b = RTC->DIVH;
    } while (high2a != high2b);

    if (high1b != high2b) {
        do {
            low1 = RTC->DIVL;
            low2 = RTC->DIVL;
        } while (low1 != low2);
    }

    return ((((uint32_t)high2b & (uint32_t)0x000F) << 16) | low2);
}

/*********************************************************************
 * @fn      RTC_WaitForLastTask
 *
 * @brief   Waits until last write operation on RTC registers has finished
 *          Note-
 *          This function must be called before any write to RTC registers.
 * @return  none
 */
void RTC_WaitForLastTask (void) {
    while ((RTC->CTLRL & RTC_FLAG_RTOFF) == (uint16_t)RESET) {
    }
}

/*********************************************************************
 * @fn      RTC_WaitForSynchro
 *
 * @brief   Waits until the RTC registers are synchronized with RTC APB clock
 *          Note-
 *          This function must be called before any read operation after an APB reset
 *          or an APB clock stop.
 *
 * @return  none
 */
void RTC_WaitForSynchro (void) {
    RTC->CTLRL &= (uint16_t)~RTC_FLAG_RSF;
    while ((RTC->CTLRL & RTC_FLAG_RSF) == (uint16_t)RESET) {
    }
}

/*********************************************************************
 * @fn      RTC_GetFlagStatus
 *
 * @brief   Checks whether the specified RTC flag is set or not
 *
 * @param   RTC_FLAG- specifies the flag to check
 *            RTC_FLAG_RTOFF - RTC Operation OFF flag
 *            RTC_FLAG_RSF - Registers Synchronized flag
 *            RTC_FLAG_OW - Overflow flag
 *            RTC_FLAG_ALR - Alarm flag
 *            RTC_FLAG_SEC - Second flag
 *
 * @return  The new state of RTC_FLAG (SET or RESET)
 */
FlagStatus RTC_GetFlagStatus (uint16_t RTC_FLAG) {
    FlagStatus bitstatus = RESET;
    if ((RTC->CTLRL & RTC_FLAG) != (uint16_t)RESET) {
        bitstatus = SET;
    } else {
        bitstatus = RESET;
    }
    return bitstatus;
}

/*********************************************************************
 * @fn      RTC_ClearFlag
 *
 * @brief   Clears the RTC's pending flags
 *
 * @param   RTC_FLAG - specifies the flag to clear
 *            RTC_FLAG_RSF - Registers Synchronized flag
 *            RTC_FLAG_OW - Overflow flag
 *            RTC_FLAG_ALR - Alarm flag
 *            RTC_FLAG_SEC - Second flag
 *
 * @return  none
 */
void RTC_ClearFlag (uint16_t RTC_FLAG) {
    RTC->CTLRL &= (uint16_t)~RTC_FLAG;
}

/*********************************************************************
 * @fn      RTC_GetITStatus
 *
 * @brief   Checks whether the specified RTC interrupt has occurred or not
 *
 * @param   RTC_IT - specifies the RTC interrupts sources to check
 *            RTC_FLAG_OW - Overflow interrupt
 *            RTC_FLAG_ALR - Alarm interrupt
 *            RTC_FLAG_SEC - Second interrupt
 *
 * @return  The new state of the RTC_IT (SET or RESET)
 */
ITStatus RTC_GetITStatus (uint16_t RTC_IT) {
    ITStatus bitstatus = RESET;

    bitstatus = (ITStatus)(RTC->CTLRL & RTC_IT);
    if (((RTC->CTLRH & RTC_IT) != (uint16_t)RESET) && (bitstatus != (uint16_t)RESET)) {
        bitstatus = SET;
    } else {
        bitstatus = RESET;
    }
    return bitstatus;
}

/*********************************************************************
 * @fn      RTC_ClearITPendingBit
 *
 * @brief   Clears the RTC's interrupt pending bits
 *
 * @param   RTC_IT - specifies the interrupt pending bit to clear
 *            RTC_FLAG_OW - Overflow interrupt
 *            RTC_FLAG_ALR - Alarm interrupt
 *            RTC_FLAG_SEC - Second interrupt
 *
 * @return  none
 */
void RTC_ClearITPendingBit (uint16_t RTC_IT) {
    RTC->CTLRL &= (uint16_t)~RTC_IT;
}

#if defined(CH32V20x_D8) || defined(CH32V20x_D8W)
/*******************************************************************************
 * @fn            Calibration_LSI
 *
 * @brief         LSI calibration
 *
 * @param         cali_Lv : calibration level
 *                Level_32  -   1.2ms 1100ppm
 *                Level_64  -   2.2ms 1000ppm
 *                Level_128 -   4.2ms 800ppm
 *
 * @return        None
 */
void Calibration_LSI (Cali_LevelTypeDef cali_Lv) {
    uint32_t i;
    int32_t cnt_offset;
    int32_t Freq = 0;
    uint8_t retry = 0;
    uint8_t retry_all = 0;
    uint32_t cnt_32k = 0;
    Freq = SystemCoreClock;
    // Coarse tuning
    OSC->LSI32K_CAL_CFG &= ~RB_OSC_CNT_VLU;
    OSC->LSI32K_CAL_CFG |= 0;
    while (1) {
        retry_all++;
        while (1) {
            OSC->LSI32K_CAL_CTRL |= RB_OSC_CAL_EN;
            OSC->LSI32K_CAL_STATR |= RB_OSC_CAL_CNT_OV;
            OSC->LSI32K_CAL_STATR |= RB_OSC_CAL_IF_END;
            while (!(OSC->LSI32K_CAL_STATR & RB_OSC_CAL_IF_END))
                ;
            i = OSC->LSI32K_CAL_STATR;
            OSC->LSI32K_CAL_CTRL &= ~RB_OSC_CAL_EN;
            OSC->LSI32K_CAL_CTRL |= RB_OSC_CAL_EN;
            OSC->LSI32K_CAL_STATR |= RB_OSC_CAL_CNT_OV;
            OSC->LSI32K_CAL_STATR |= RB_OSC_CAL_IF_END;
            cnt_32k = RTC_GetCounter();
            while (RTC_GetCounter() == cnt_32k)
                ;
            OSC->LSI32K_CAL_STATR |= RB_OSC_CAL_CNT_OV;
            while (OSC->LSI32K_CAL_STATR & RB_OSC_CAL_IF_END)
                ;
            while (!(OSC->LSI32K_CAL_STATR & RB_OSC_CAL_IF_END))
                ;
            i = OSC->LSI32K_CAL_STATR;
            cnt_offset = (i & 0x3FFF) + OSC->LSI32K_CAL_OV_CNT * 0x3FFF - 2000 * (Freq / 1000) / CAB_LSIFQ;
            if (((cnt_offset > -(20 * (Freq / 1000) / 36000)) && (cnt_offset < (20 * (Freq / 1000) / 36000))) || retry > 2)
                break;
            retry++;
            cnt_offset = (cnt_offset > 0) ? (((cnt_offset * 2) / (40 * (Freq / 1000) / 36000)) + 1) / 2 : (((cnt_offset * 2) / (40 * (Freq / 1000) / 36000)) - 1) / 2;
            OSC->LSI32K_TUNE += cnt_offset;
        }
        OSC->LSI32K_CAL_CFG &= ~RB_OSC_CNT_VLU;
        OSC->LSI32K_CAL_CFG |= 2;
        OSC->LSI32K_CAL_CTRL &= ~RB_OSC_CAL_EN;
        OSC->LSI32K_CAL_CTRL |= RB_OSC_CAL_EN;
        OSC->LSI32K_CAL_STATR |= RB_OSC_CAL_IF_END;
        OSC->LSI32K_CAL_STATR |= RB_OSC_CAL_CNT_OV;

        // Fine tuning
        // After configuring the fine-tuning parameters, discard the two captured values (software behavior) and judge once, only one time is left here
        while (!(OSC->LSI32K_CAL_STATR & RB_OSC_CAL_IF_END))
            ;
        i = OSC->LSI32K_CAL_STATR;
        OSC->LSI32K_CAL_CTRL &= ~RB_OSC_CAL_EN;
        OSC->LSI32K_CAL_CTRL |= RB_OSC_CAL_EN;
        OSC->LSI32K_CAL_STATR |= RB_OSC_CAL_IF_END;
        OSC->LSI32K_CAL_STATR |= RB_OSC_CAL_CNT_OV;
        cnt_32k = RTC_GetCounter();
        while (RTC_GetCounter() == cnt_32k)
            ;
        OSC->LSI32K_CAL_STATR |= RB_OSC_CAL_CNT_OV;
        while (OSC->LSI32K_CAL_STATR & RB_OSC_CAL_IF_END)
            ;
        while (!(OSC->LSI32K_CAL_STATR & RB_OSC_CAL_IF_END))
            ;
        i = OSC->LSI32K_CAL_STATR;
        cnt_offset = (i & 0x3FFF) + OSC->LSI32K_CAL_OV_CNT * 0x3FFF - 8000 * (1 << 2) * (Freq / 1000000) / 256 * 1000 / (CAB_LSIFQ / 256);
        cnt_offset = (cnt_offset > 0) ? ((((cnt_offset * 2 * 100) / (748 * ((1 << 2) / 4) * (Freq / 1000) / 36000)) + 1) / 2) : ((((cnt_offset * 2 * 100) / (748 * ((1 << 2) / 4) * (Freq / 1000) / 36000)) - 1) / 2);
        if ((cnt_offset > 0) && (((OSC->LSI32K_TUNE >> 5) + cnt_offset) > 0x7FF)) {
            if (retry_all > 2) {
                OSC->LSI32K_TUNE |= (0xFF << 5);
            } else {
                OSC->LSI32K_TUNE = (OSC->LSI32K_TUNE & 0x1F) | (0x3FF << 5);
                continue;
            }
        } else if ((cnt_offset < 0) && ((OSC->LSI32K_TUNE >> 5) < (-cnt_offset))) {
            if (retry_all > 2) {
                OSC->LSI32K_TUNE &= 0x1F;
            } else {
                OSC->LSI32K_TUNE = (OSC->LSI32K_TUNE & 0x1F) | (0x7F << 5);
                continue;
            }
        } else {
            OSC->LSI32K_TUNE += (cnt_offset << 5);
        }
        OSC->LSI32K_CAL_CFG &= ~RB_OSC_CNT_VLU;
        OSC->LSI32K_CAL_CFG |= cali_Lv;
        OSC->LSI32K_CAL_CTRL &= ~RB_OSC_CAL_EN;
        OSC->LSI32K_CAL_CTRL |= RB_OSC_CAL_EN;
        OSC->LSI32K_CAL_STATR |= RB_OSC_CAL_IF_END;
        OSC->LSI32K_CAL_STATR |= RB_OSC_CAL_CNT_OV;
        // Fine tuning
        // After configuring the fine-tuning parameters, discard the two captured values (software behavior) and judge once, only one time is left here
        while (!(OSC->LSI32K_CAL_STATR & RB_OSC_CAL_IF_END))
            ;
        i = OSC->LSI32K_CAL_STATR;
        OSC->LSI32K_CAL_CTRL &= ~RB_OSC_CAL_EN;
        OSC->LSI32K_CAL_CTRL |= RB_OSC_CAL_EN;
        OSC->LSI32K_CAL_STATR |= RB_OSC_CAL_IF_END;
        OSC->LSI32K_CAL_STATR |= RB_OSC_CAL_CNT_OV;
        cnt_32k = RTC_GetCounter();
        while (RTC_GetCounter() == cnt_32k)
            ;
        OSC->LSI32K_CAL_STATR |= RB_OSC_CAL_CNT_OV;
        while (OSC->LSI32K_CAL_STATR & RB_OSC_CAL_IF_END)
            ;
        while (!(OSC->LSI32K_CAL_STATR & RB_OSC_CAL_IF_END))
            ;
        OSC->LSI32K_CAL_CTRL &= ~RB_OSC_CAL_EN;
        i = OSC->LSI32K_CAL_STATR;
        cnt_offset = (i & 0x3FFF) + OSC->LSI32K_CAL_OV_CNT * 0x3FFF - 8000 * (1 << cali_Lv) * (Freq / 1000000) / 256 * 1000 / (CAB_LSIFQ / 256);
        cnt_offset = (cnt_offset > 0) ? ((((cnt_offset * 2 * 100) / (748 * ((1 << cali_Lv) / 4) * (Freq / 1000) / 36000)) + 1) / 2) : ((((cnt_offset * 2 * 100) / (748 * ((1 << cali_Lv) / 4) * (Freq / 1000) / 36000)) - 1) / 2);
        if ((cnt_offset > 0) && (((OSC->LSI32K_TUNE >> 5) + cnt_offset) > 0x7FF)) {
            if (retry_all > 2) {
                OSC->LSI32K_TUNE |= (0xFF << 5);
                return;
            } else {
                OSC->LSI32K_TUNE = (OSC->LSI32K_TUNE & 0x1F) | (0x3FF << 5);
                continue;
            }
        } else if ((cnt_offset < 0) && ((OSC->LSI32K_TUNE >> 5) < (-cnt_offset))) {
            if (retry_all > 2) {
                OSC->LSI32K_TUNE &= 0x1F;
                return;
            } else {
                OSC->LSI32K_TUNE = (OSC->LSI32K_TUNE & 0x1F) | (0x3F << 5);
                continue;
            }
        } else {
            OSC->LSI32K_TUNE += (cnt_offset << 5);
            return;
        }
    }
}

#endif
