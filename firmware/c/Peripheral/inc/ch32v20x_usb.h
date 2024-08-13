/********************************** (C) COPYRIGHT  *******************************
 * File Name          : ch32v20x_usb.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/01/30
 * Description        : This file contains all the functions prototypes for the USB
 *                      firmware library.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#ifndef __CH32V20X_USB_H
#define __CH32V20X_USB_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/* Header File */
#include "stdint.h"

/*******************************************************************************/
/* USB Communication Related Macro Definition */
#ifndef DEFAULT_ENDP0_SIZE
#define DEFAULT_ENDP0_SIZE 8 // default maximum packet size for endpoint 0
#endif
#ifndef MAX_PACKET_SIZE
#define MAX_PACKET_SIZE 64 // maximum packet size
#endif

/* USB PID */
#ifndef USB_PID_SETUP
#define USB_PID_NULL 0x00
#define USB_PID_SOF 0x05
#define USB_PID_SETUP 0x0D
#define USB_PID_IN 0x09
#define USB_PID_OUT 0x01
#define USB_PID_NYET 0x06
#define USB_PID_ACK 0x02
#define USB_PID_NAK 0x0A
#define USB_PID_STALL 0x0E
#define USB_PID_DATA0 0x03
#define USB_PID_DATA1 0x0B
#define USB_PID_PRE 0x0C
#endif

/* USB standard device request code */
#ifndef USB_GET_DESCRIPTOR
#define USB_GET_STATUS 0x00
#define USB_CLEAR_FEATURE 0x01
#define USB_SET_FEATURE 0x03
#define USB_SET_ADDRESS 0x05
#define USB_GET_DESCRIPTOR 0x06
#define USB_SET_DESCRIPTOR 0x07
#define USB_GET_CONFIGURATION 0x08
#define USB_SET_CONFIGURATION 0x09
#define USB_GET_INTERFACE 0x0A
#define USB_SET_INTERFACE 0x0B
#define USB_SYNCH_FRAME 0x0C
#endif

#define DEF_STRING_DESC_LANG 0x00
#define DEF_STRING_DESC_MANU 0x01
#define DEF_STRING_DESC_PROD 0x02
#define DEF_STRING_DESC_SERN 0x03

/* USB hub class request code */
#ifndef HUB_GET_DESCRIPTOR
#define HUB_GET_STATUS 0x00
#define HUB_CLEAR_FEATURE 0x01
#define HUB_GET_STATE 0x02
#define HUB_SET_FEATURE 0x03
#define HUB_GET_DESCRIPTOR 0x06
#define HUB_SET_DESCRIPTOR 0x07
#endif

/* USB HID class request code */
#ifndef HID_GET_REPORT
#define HID_GET_REPORT 0x01
#define HID_GET_IDLE 0x02
#define HID_GET_PROTOCOL 0x03
#define HID_SET_REPORT 0x09
#define HID_SET_IDLE 0x0A
#define HID_SET_PROTOCOL 0x0B
#endif

/* Bit Define for USB Request Type */
#ifndef USB_REQ_TYP_MASK
#define USB_REQ_TYP_IN 0x80
#define USB_REQ_TYP_OUT 0x00
#define USB_REQ_TYP_READ 0x80
#define USB_REQ_TYP_WRITE 0x00
#define USB_REQ_TYP_MASK 0x60
#define USB_REQ_TYP_STANDARD 0x00
#define USB_REQ_TYP_CLASS 0x20
#define USB_REQ_TYP_VENDOR 0x40
#define USB_REQ_TYP_RESERVED 0x60
#define USB_REQ_RECIP_MASK 0x1F
#define USB_REQ_RECIP_DEVICE 0x00
#define USB_REQ_RECIP_INTERF 0x01
#define USB_REQ_RECIP_ENDP 0x02
#define USB_REQ_RECIP_OTHER 0x03
#define USB_REQ_FEAT_REMOTE_WAKEUP 0x01
#define USB_REQ_FEAT_ENDP_HALT 0x00
#endif

/* USB Descriptor Type */
#ifndef USB_DESCR_TYP_DEVICE
#define USB_DESCR_TYP_DEVICE 0x01
#define USB_DESCR_TYP_CONFIG 0x02
#define USB_DESCR_TYP_STRING 0x03
#define USB_DESCR_TYP_INTERF 0x04
#define USB_DESCR_TYP_ENDP 0x05
#define USB_DESCR_TYP_QUALIF 0x06
#define USB_DESCR_TYP_SPEED 0x07
#define USB_DESCR_TYP_OTG 0x09
#define USB_DESCR_TYP_BOS 0X0F
#define USB_DESCR_TYP_HID 0x21
#define USB_DESCR_TYP_REPORT 0x22
#define USB_DESCR_TYP_PHYSIC 0x23
#define USB_DESCR_TYP_CS_INTF 0x24
#define USB_DESCR_TYP_CS_ENDP 0x25
#define USB_DESCR_TYP_HUB 0x29
#endif

/* USB Device Class */
#ifndef USB_DEV_CLASS_HUB
#define USB_DEV_CLASS_RESERVED 0x00
#define USB_DEV_CLASS_AUDIO 0x01
#define USB_DEV_CLASS_COMMUNIC 0x02
#define USB_DEV_CLASS_HID 0x03
#define USB_DEV_CLASS_MONITOR 0x04
#define USB_DEV_CLASS_PHYSIC_IF 0x05
#define USB_DEV_CLASS_POWER 0x06
#define USB_DEV_CLASS_IMAGE 0x06
#define USB_DEV_CLASS_PRINTER 0x07
#define USB_DEV_CLASS_STORAGE 0x08
#define USB_DEV_CLASS_HUB 0x09
#define USB_DEV_CLASS_VEN_SPEC 0xFF
#endif

/* USB Hub Class Request */
#ifndef HUB_GET_HUB_DESCRIPTOR
#define HUB_CLEAR_HUB_FEATURE 0x20
#define HUB_CLEAR_PORT_FEATURE 0x23
#define HUB_GET_BUS_STATE 0xA3
#define HUB_GET_HUB_DESCRIPTOR 0xA0
#define HUB_GET_HUB_STATUS 0xA0
#define HUB_GET_PORT_STATUS 0xA3
#define HUB_SET_HUB_DESCRIPTOR 0x20
#define HUB_SET_HUB_FEATURE 0x20
#define HUB_SET_PORT_FEATURE 0x23
#endif

/* Hub Class Feature Selectors */
#ifndef HUB_PORT_RESET
#define HUB_C_HUB_LOCAL_POWER 0
#define HUB_C_HUB_OVER_CURRENT 1
#define HUB_PORT_CONNECTION 0
#define HUB_PORT_ENABLE 1
#define HUB_PORT_SUSPEND 2
#define HUB_PORT_OVER_CURRENT 3
#define HUB_PORT_RESET 4
#define HUB_PORT_POWER 8
#define HUB_PORT_LOW_SPEED 9
#define HUB_C_PORT_CONNECTION 16
#define HUB_C_PORT_ENABLE 17
#define HUB_C_PORT_SUSPEND 18
#define HUB_C_PORT_OVER_CURRENT 19
#define HUB_C_PORT_RESET 20
#endif

/* USB HID Class Request Code */
#ifndef HID_GET_REPORT
#define HID_GET_REPORT 0x01
#define HID_GET_IDLE 0x02
#define HID_GET_PROTOCOL 0x03
#define HID_SET_REPORT 0x09
#define HID_SET_IDLE 0x0A
#define HID_SET_PROTOCOL 0x0B
#endif

/* USB CDC Class request code */
#ifndef CDC_GET_LINE_CODING
#define CDC_GET_LINE_CODING 0X21 /* This request allows the host to find out the currently configured line coding */
#define CDC_SET_LINE_CODING 0x20 /* Configures DTE rate, stop-bits, parity, and number-of-character */
#define CDC_SET_LINE_CTLSTE 0X22 /* This request generates RS-232/V.24 style control signals */
#define CDC_SEND_BREAK 0X23      /* Sends special carrier modulation used to specify RS-232 style break */
#endif

/* USB UDisk */
#ifndef USB_BO_CBW_SIZE
#define USB_BO_CBW_SIZE 0x1F
#define USB_BO_CSW_SIZE 0x0D
#endif
#ifndef USB_BO_CBW_SIG0
#define USB_BO_CBW_SIG0 0x55
#define USB_BO_CBW_SIG1 0x53
#define USB_BO_CBW_SIG2 0x42
#define USB_BO_CBW_SIG3 0x43
#define USB_BO_CSW_SIG0 0x55
#define USB_BO_CSW_SIG1 0x53
#define USB_BO_CSW_SIG2 0x42
#define USB_BO_CSW_SIG3 0x53
#endif

/*******************************************************************************/
/* USBFS Related Register Macro Definition */

/* R8_USB_CTRL */
#define USBFS_UC_HOST_MODE 0x80
#define USBFS_UC_LOW_SPEED 0x40
#define USBFS_UC_DEV_PU_EN 0x20
#define USBFS_UC_SYS_CTRL_MASK 0x30
#define USBFS_UC_SYS_CTRL0 0x00
#define USBFS_UC_SYS_CTRL1 0x10
#define USBFS_UC_SYS_CTRL2 0x20
#define USBFS_UC_SYS_CTRL3 0x30
#define USBFS_UC_INT_BUSY 0x08
#define USBFS_UC_RESET_SIE 0x04
#define USBFS_UC_CLR_ALL 0x02
#define USBFS_UC_DMA_EN 0x01

/* R8_USB_INT_EN */
#define USBFS_UIE_DEV_NAK 0x40
#define USBFS_UIE_FIFO_OV 0x10
#define USBFS_UIE_HST_SOF 0x08
#define USBFS_UIE_SUSPEND 0x04
#define USBFS_UIE_TRANSFER 0x02
#define USBFS_UIE_DETECT 0x01
#define USBFS_UIE_BUS_RST 0x01

/* R8_USB_DEV_AD */
#define USBFS_UDA_GP_BIT 0x80
#define USBFS_USB_ADDR_MASK 0x7F

/* R8_USB_MIS_ST */
#define USBFS_UMS_SOF_PRES 0x80
#define USBFS_UMS_SOF_ACT 0x40
#define USBFS_UMS_SIE_FREE 0x20
#define USBFS_UMS_R_FIFO_RDY 0x10
#define USBFS_UMS_BUS_RESET 0x08
#define USBFS_UMS_SUSPEND 0x04
#define USBFS_UMS_DM_LEVEL 0x02
#define USBFS_UMS_DEV_ATTACH 0x01

/* R8_USB_INT_FG */
#define USBFS_U_IS_NAK 0x80     // RO, indicate current USB transfer is NAK received
#define USBFS_U_TOG_OK 0x40     // RO, indicate current USB transfer toggle is OK
#define USBFS_U_SIE_FREE 0x20   // RO, indicate USB SIE free status
#define USBFS_UIF_FIFO_OV 0x10  // FIFO overflow interrupt flag for USB, direct bit address clear or write 1 to clear
#define USBFS_UIF_HST_SOF 0x08  // host SOF timer interrupt flag for USB host, direct bit address clear or write 1 to clear
#define USBFS_UIF_SUSPEND 0x04  // USB suspend or resume event interrupt flag, direct bit address clear or write 1 to clear
#define USBFS_UIF_TRANSFER 0x02 // USB transfer completion interrupt flag, direct bit address clear or write 1 to clear
#define USBFS_UIF_DETECT 0x01   // device detected event interrupt flag for USB host mode, direct bit address clear or write 1 to clear
#define USBFS_UIF_BUS_RST 0x01  // bus reset event interrupt flag for USB device mode, direct bit address clear or write 1 to clear

/* R8_USB_INT_ST */
#define USBFS_UIS_IS_NAK 0x80     // RO, indicate current USB transfer is NAK received for USB device mode
#define USBFS_UIS_TOG_OK 0x40     // RO, indicate current USB transfer toggle is OK
#define USBFS_UIS_TOKEN_MASK 0x30 // RO, bit mask of current token PID code received for USB device mode
#define USBFS_UIS_TOKEN_OUT 0x00
#define USBFS_UIS_TOKEN_IN 0x20
#define USBFS_UIS_TOKEN_SETUP 0x30
// bUIS_TOKEN1 & bUIS_TOKEN0: current token PID code received for USB device mode
//   00: OUT token PID received
//   10: IN token PID received
//   11: SETUP token PID received
#define USBFS_UIS_ENDP_MASK 0x0F  // RO, bit mask of current transfer endpoint number for USB device mode
#define USBFS_UIS_H_RES_MASK 0x0F // RO, bit mask of current transfer handshake response for USB host mode: 0000=no response, time out from device, others=handshake response PID received

/* R32_USB_OTG_CR */
#define USBFS_CR_SESS_VTH 0x20
#define USBFS_CR_VBUS_VTH 0x10
#define USBFS_CR_OTG_EN 0x08
#define USBFS_CR_IDPU 0x04
#define USBFS_CR_CHARGE_VBUS 0x02
#define USBFS_CR_DISCHAR_VBUS 0x01

/* R32_USB_OTG_SR */
#define USBFS_SR_ID_DIG 0x08
#define USBFS_SR_SESS_END 0x04
#define USBFS_SR_SESS_VLD 0x02
#define USBFS_SR_VBUS_VLD 0x01

/* R8_UDEV_CTRL */
#define USBFS_UD_PD_DIS 0x80    // disable USB UDP/UDM pulldown resistance: 0=enable pulldown, 1=disable
#define USBFS_UD_DP_PIN 0x20    // ReadOnly: indicate current UDP pin level
#define USBFS_UD_DM_PIN 0x10    // ReadOnly: indicate current UDM pin level
#define USBFS_UD_LOW_SPEED 0x04 // enable USB physical port low speed: 0=full speed, 1=low speed
#define USBFS_UD_GP_BIT 0x02    // general purpose bit
#define USBFS_UD_PORT_EN 0x01   // enable USB physical port I/O: 0=disable, 1=enable

/* R8_UEP4_1_MOD */
#define USBFS_UEP1_RX_EN 0x80   // enable USB endpoint 1 receiving (OUT)
#define USBFS_UEP1_TX_EN 0x40   // enable USB endpoint 1 transmittal (IN)
#define USBFS_UEP1_BUF_MOD 0x10 // buffer mode of USB endpoint 1
#define USBFS_UEP4_RX_EN 0x08   // enable USB endpoint 4 receiving (OUT)
#define USBFS_UEP4_TX_EN 0x04   // enable USB endpoint 4 transmittal (IN)
#define USBFS_UEP4_BUF_MOD 0x01

/* R8_UEP2_3_MOD */
#define USBFS_UEP3_RX_EN 0x80   // enable USB endpoint 3 receiving (OUT)
#define USBFS_UEP3_TX_EN 0x40   // enable USB endpoint 3 transmittal (IN)
#define USBFS_UEP3_BUF_MOD 0x10 // buffer mode of USB endpoint 3
#define USBFS_UEP2_RX_EN 0x08   // enable USB endpoint 2 receiving (OUT)
#define USBFS_UEP2_TX_EN 0x04   // enable USB endpoint 2 transmittal (IN)
#define USBFS_UEP2_BUF_MOD 0x01 // buffer mode of USB endpoint 2

/* R8_UEP5_6_MOD */
#define USBFS_UEP6_RX_EN 0x80   // enable USB endpoint 6 receiving (OUT)
#define USBFS_UEP6_TX_EN 0x40   // enable USB endpoint 6 transmittal (IN)
#define USBFS_UEP6_BUF_MOD 0x10 // buffer mode of USB endpoint 6
#define USBFS_UEP5_RX_EN 0x08   // enable USB endpoint 5 receiving (OUT)
#define USBFS_UEP5_TX_EN 0x04   // enable USB endpoint 5 transmittal (IN)
#define USBFS_UEP5_BUF_MOD 0x01 // buffer mode of USB endpoint 5

/* R8_UEP7_MOD */
#define USBFS_UEP7_RX_EN 0x08   // enable USB endpoint 7 receiving (OUT)
#define USBFS_UEP7_TX_EN 0x04   // enable USB endpoint 7 transmittal (IN)
#define USBFS_UEP7_BUF_MOD 0x01 // buffer mode of USB endpoint 7

/* R8_UEPn_TX_CTRL */
#define USBFS_UEP_T_AUTO_TOG 0x08 // enable automatic toggle after successful transfer completion on endpoint 1/2/3: 0=manual toggle, 1=automatic toggle
#define USBFS_UEP_T_TOG 0x04      // prepared data toggle flag of USB endpoint X transmittal (IN): 0=DATA0, 1=DATA1
#define USBFS_UEP_T_RES_MASK 0x03 // bit mask of handshake response type for USB endpoint X transmittal (IN)
#define USBFS_UEP_T_RES_ACK 0x00
#define USBFS_UEP_T_RES_NONE 0x01
#define USBFS_UEP_T_RES_NAK 0x02
#define USBFS_UEP_T_RES_STALL 0x03
// bUEP_T_RES1 & bUEP_T_RES0: handshake response type for USB endpoint X transmittal (IN)
//   00: DATA0 or DATA1 then expecting ACK (ready)
//   01: DATA0 or DATA1 then expecting no response, time out from host, for non-zero endpoint isochronous transactions
//   10: NAK (busy)
//   11: STALL (error)
// host aux setup

/* R8_UEPn_RX_CTRL, n=0-7 */
#define USBFS_UEP_R_AUTO_TOG 0x08 // enable automatic toggle after successful transfer completion on endpoint 1/2/3: 0=manual toggle, 1=automatic toggle
#define USBFS_UEP_R_TOG 0x04      // expected data toggle flag of USB endpoint X receiving (OUT): 0=DATA0, 1=DATA1
#define USBFS_UEP_R_RES_MASK 0x03 // bit mask of handshake response type for USB endpoint X receiving (OUT)
#define USBFS_UEP_R_RES_ACK 0x00
#define USBFS_UEP_R_RES_NONE 0x01
#define USBFS_UEP_R_RES_NAK 0x02
#define USBFS_UEP_R_RES_STALL 0x03
// RB_UEP_R_RES1 & RB_UEP_R_RES0: handshake response type for USB endpoint X receiving (OUT)
//   00: ACK (ready)
//   01: no response, time out to host, for non-zero endpoint isochronous transactions
//   10: NAK (busy)
//   11: STALL (error)

/* R8_UHOST_CTRL */
#define USBFS_UH_PD_DIS 0x80    // disable USB UDP/UDM pulldown resistance: 0=enable pulldown, 1=disable
#define USBFS_UH_DP_PIN 0x20    // ReadOnly: indicate current UDP pin level
#define USBFS_UH_DM_PIN 0x10    // ReadOnly: indicate current UDM pin level
#define USBFS_UH_LOW_SPEED 0x04 // enable USB port low speed: 0=full speed, 1=low speed
#define USBFS_UH_BUS_RESET 0x02 // control USB bus reset: 0=normal, 1=force bus reset
#define USBFS_UH_PORT_EN 0x01   // enable USB port: 0=disable, 1=enable port, automatic disabled if USB device detached

/* R32_UH_EP_MOD */
#define USBFS_UH_EP_TX_EN 0x40    // enable USB host OUT endpoint transmittal
#define USBFS_UH_EP_TBUF_MOD 0x10 // buffer mode of USB host OUT endpoint
// bUH_EP_TX_EN & bUH_EP_TBUF_MOD: USB host OUT endpoint buffer mode, buffer start address is UH_TX_DMA
//   0 x:  disable endpoint and disable buffer
//   1 0:  64 bytes buffer for transmittal (OUT endpoint)
//   1 1:  dual 64 bytes buffer by toggle bit bUH_T_TOG selection for transmittal (OUT endpoint), total=128bytes
#define USBFS_UH_EP_RX_EN 0x08    // enable USB host IN endpoint receiving
#define USBFS_UH_EP_RBUF_MOD 0x01 // buffer mode of USB host IN endpoint
// bUH_EP_RX_EN & bUH_EP_RBUF_MOD: USB host IN endpoint buffer mode, buffer start address is UH_RX_DMA
//   0 x:  disable endpoint and disable buffer
//   1 0:  64 bytes buffer for receiving (IN endpoint)
//   1 1:  dual 64 bytes buffer by toggle bit bUH_R_TOG selection for receiving (IN endpoint), total=128bytes

/* R16_UH_SETUP */
#define USBFS_UH_PRE_PID_EN 0x0400 // USB host PRE PID enable for low speed device via hub
#define USBFS_UH_SOF_EN 0x0004     // USB host automatic SOF enable

/* R8_UH_EP_PID */
#define USBFS_UH_TOKEN_MASK 0xF0 // bit mask of token PID for USB host transfer
#define USBFS_UH_ENDP_MASK 0x0F  // bit mask of endpoint number for USB host transfer

/* R8_UH_RX_CTRL */
#define USBFS_UH_R_AUTO_TOG 0x08 // enable automatic toggle after successful transfer completion: 0=manual toggle, 1=automatic toggle
#define USBFS_UH_R_TOG 0x04      // expected data toggle flag of host receiving (IN): 0=DATA0, 1=DATA1
#define USBFS_UH_R_RES 0x01      // prepared handshake response type for host receiving (IN): 0=ACK (ready), 1=no response, time out to device, for isochronous transactions

/* R8_UH_TX_CTRL */
#define USBFS_UH_T_AUTO_TOG 0x08 // enable automatic toggle after successful transfer completion: 0=manual toggle, 1=automatic toggle
#define USBFS_UH_T_TOG 0x04      // prepared data toggle flag of host transmittal (SETUP/OUT): 0=DATA0, 1=DATA1
#define USBFS_UH_T_RES 0x01      // expected handshake response type for host transmittal (SETUP/OUT): 0=ACK (ready), 1=no response, time out from device, for isochronous transactions

/*******************************************************************************/
/* USBD Related Register Macro Definition */

/* R16_USBD_CNTR */
#define USBD_CNTR_CTRM (1 << 15)      // Correct transfer interrupt: 1 = enable
#define USBD_CNTR_PMAOVRM (1 << 14)   // Packet buffer overflow interrupt: 1 = enable
#define USBD_CNTR_ERRM (1 << 13)      // Error interrupt: 1 = enable
#define USBD_CNTR_WKUPM (1 << 12)     // Wake-up interrupt: 1 = enable
#define USBD_CNTR_SUSPM (1 << 11)     // Suspend interrupt: 1 = enable
#define USBD_CNTR_RESETM (1 << 10)    // Reset interrupt: 1 = enable
#define USBD_CNTR_SOFM (1 << 9)       // Start of frame interrupt: 1 = enable
#define USBD_CNTR_ESOFM (1 << 8)      // Start of frame error interrupt: 1 = enable
#define USBD_CNTR_MODE_1WIRE (1 << 7) // 1-wire mode (only supported on CH32F20x_D8 family)
#define USBD_CNTR_RESUME (1 << 4)     // Wake-up request control: 1 = Output wake-up signal
#define USBD_CNTR_FSUSP (1 << 3)      // Mask suspend detection control
#define USBD_CNTR_LPMODE (1 << 2)     // Low-power mode control: 1 = low-power mode
#define USBD_CNTR_PDWN (1 << 1)       // Power down mode
#define USBD_CNTR_FRES (1 << 0)       // Force USB reset control: 1 = Forcibly reset the USBD module. The USBD module will remain in reset until software clears this bit

/* R16_USBD_ISTR */
#define USBD_ISTR_CTR (1 << 15)     // Correct transfer interrupt
#define USBD_ISTR_PMAOVR (1 << 14)  // Packet buffer overflow interrupt
#define USBD_ISTR_ERR (1 << 13)     // Error interrupt
#define USBD_ISTR_WKUP (1 << 12)    // Wakeup interrupt
#define USBD_ISTR_SUSP (1 << 11)    // Suspend interrupt
#define USBD_ISTR_RESET (1 << 10)   // Reset interrupt
#define USBD_ISTR_SOF (1 << 9)      // SOF interrupt
#define USBD_ISTR_ESOF (1 << 8)     // ESOF interrupt
#define USBD_ISTR_DIR (1 << 4)      // Data transfer direction: 0 = Data transfer from the USBD module to the PC host
#define USBD_ISTR_EP_ID_MASK 0b1111 // Endpoint ID of the interrupt

/* R16_USBD_FNR */
#define USBD_FNR_RXDP (1 << 15)          // DP line level status
#define USBD_FNR_RXDM (1 << 14)          // DM line level status
#define USBD_FNR_LCK (1 << 13)           // SOF packet count stop lock
#define USBD_FNR_LSOF_MASK (0b11 << 11)  // Lost SOF frame count
#define USBD_FNR_FN_MASK (0b11111111111) // SOF frame number

/* R16_USBD_DADDR */
#define USBD_DADDR_EF (1 << 7)          // USB function enable
#define USBD_DADDR_ADD_MASK (0b1111111) // USB device address

/* R16_USBD_BTABLE */
#define USBD_BTABLE_BLABLE_MASK (0b1111111111111000) // offset address from 0x40006000 of the packet buffer description table

/* btable structure */

/* BTABLE mapping */
#define USBD_BTABLE_BASE (USBD_CAN1_CAN2_SRAM_BASE)
#define USBD_BTABLE_EP(x) ((USBD_BTABLE_TypeDef *)(USBD_BTABLE_BASE + (x * 16))) // 32-bit absolute memory address
#define USBD_BTABLE_END (USBD->BTABLE + (8 * 8))                                 // 16-bit relative address
#define USBD_BTABLE_COUNT_RX_MASK 0x3FF

/* R16_USBD_EPR[0:7] */
#define USBD_EPR(x) ((USBD_EPR_TypeDef *)(USBD_EP_BASE + (x * 4)))
#define USBD_EPR_CTR_RX (1 << 15)              // Correct reception flag (OUT/SETUP).
#define USBD_EPR_DTOG_RX (1 << 14)             // toggle DATA0-1 for RX
#define USBD_EPR_STAT_RX_DISABLED (0b00 << 12) // RX Disabled (ignores all RX)
#define USBD_EPR_STAT_RX_STALL (0b01 << 12)    // RX Stall (error)
#define USBD_EPR_STAT_RX_NAK (0b10 << 12)      // RX NAK (not ready for RX)
#define USBD_EPR_STAT_RX_ACK (0b11 << 12)      // RX ACK (ok for RX)
#define USBD_EPR_STAT_RX_MASK (0b11 << 12)     // RX reception status
#define USBD_EPR_SETUP (1 << 11)               // SETUP frame received flag
#define USBD_EPR_EPTYPE_BULK (0b00 << 9)       // Bulk endpoint
#define USBD_EPR_EPTYPE_CONTROL (0b01 << 9)    // Control endpoint
#define USBD_EPR_EPTYPE_ISO (0b10 << 9)        // Isochronous endpoint
#define USBD_EPR_EPTYPE_INTERRUPT (0b11 << 9)  // Interrupt endpoint
#define USBD_EPR_EPTYPE_MASK (0b11 << 9)       // Endpoint type
#define USBD_EPR_EP_KIND (1 << 8)              // Endpoint type control bits
#define USBD_EPR_CTR_TX (1 << 7)               // Correct transmission flag (IN)
#define USBD_EPR_DTOG_TX (1 << 6)              // toggle DATA0-1 for TX
#define USBD_EPR_STAT_TX_DISABLED (0b00 << 4)  // TX Disabled (ignores all TX)
#define USBD_EPR_STAT_TX_STALL (0b01 << 4)     // TX Stall (error)
#define USBD_EPR_STAT_TX_NAK (0b10 << 4)       // TX NAK (not ready for TX)
#define USBD_EPR_STAT_TX_ACK (0b11 << 4)       // TX ACK (ok for TX)
#define USBD_EPR_STAT_TX_MASK (0b11 << 4)      // TX status
#define USBD_EPR_EA_MASK (0b1111 << 0)         // Endpoint address

// EPR with toggle-bits unset and clear-bits uncleared
#define USBD_EPR_INVARIANT(x) ((USBD_EPR(x)->EPR & ~(USBD_EPR_DTOG_RX | USBD_EPR_STAT_RX_MASK | USBD_EPR_DTOG_TX | USBD_EPR_STAT_TX_MASK)) | (USBD_EPR_CTR_RX | USBD_EPR_CTR_TX))

// eeeeeeeeeeeeeeeeeeeeeeee this is fucking disgusting
#define USBD_EPR_STAT_RX(x, y) ((USBD_EPR_INVARIANT(x)) | ((USBD_EPR(x)->EPR & USBD_EPR_STAT_RX_MASK) ^ y))
#define USBD_EPR_STAT_TX(x, y) ((USBD_EPR_INVARIANT(x)) | ((USBD_EPR(x)->EPR & USBD_EPR_STAT_TX_MASK) ^ y))

/*******************************************************************************/
/* Struct Definition */

/* USB Setup Request */
typedef struct __attribute__((packed)) _USB_SETUP_REQ {
    uint8_t bRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} USB_SETUP_REQ, *PUSB_SETUP_REQ;

/* USB Device Descriptor */
typedef struct __attribute__((packed)) _USB_DEVICE_DESCR {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
} USB_DEV_DESCR, *PUSB_DEV_DESCR;

/* USB Configuration Descriptor */
typedef struct __attribute__((packed)) _USB_CONFIG_DESCR {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wTotalLength;
    uint8_t bNumInterfaces;
    uint8_t bConfigurationValue;
    uint8_t iConfiguration;
    uint8_t bmAttributes;
    uint8_t MaxPower;
} USB_CFG_DESCR, *PUSB_CFG_DESCR;

/* USB Interface Descriptor */
typedef struct __attribute__((packed)) _USB_INTERF_DESCR {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
} USB_ITF_DESCR, *PUSB_ITF_DESCR;

/* USB Endpoint Descriptor */
typedef struct __attribute__((packed)) _USB_ENDPOINT_DESCR {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint8_t wMaxPacketSizeL;
    uint8_t wMaxPacketSizeH;
    uint8_t bInterval;
} USB_ENDP_DESCR, *PUSB_ENDP_DESCR;

/* USB Configuration Descriptor Set */
typedef struct __attribute__((packed)) _USB_CONFIG_DESCR_LONG {
    USB_CFG_DESCR cfg_descr;
    USB_ITF_DESCR itf_descr;
    USB_ENDP_DESCR endp_descr[1];
} USB_CFG_DESCR_LONG, *PUSB_CFG_DESCR_LONG;

/* USB HUB Descriptor */
typedef struct __attribute__((packed)) _USB_HUB_DESCR {
    uint8_t bDescLength;
    uint8_t bDescriptorType;
    uint8_t bNbrPorts;
    uint8_t wHubCharacteristicsL;
    uint8_t wHubCharacteristicsH;
    uint8_t bPwrOn2PwrGood;
    uint8_t bHubContrCurrent;
    uint8_t DeviceRemovable;
    uint8_t PortPwrCtrlMask;
} USB_HUB_DESCR, *PUSB_HUB_DESCR;

/* USB HID Descriptor */
typedef struct __attribute__((packed)) _USB_HID_DESCR {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdHID;
    uint8_t bCountryCode;
    uint8_t bNumDescriptors;
    uint8_t bDescriptorTypeX;
    uint8_t wDescriptorLengthL;
    uint8_t wDescriptorLengthH;
} USB_HID_DESCR, *PUSB_HID_DESCR;

/* USB UDisk */
typedef struct __attribute__((packed)) _UDISK_BOC_CBW {
    uint32_t mCBW_Sig;
    uint32_t mCBW_Tag;
    uint32_t mCBW_DataLen;
    uint8_t mCBW_Flag;
    uint8_t mCBW_LUN;
    uint8_t mCBW_CB_Len;
    uint8_t mCBW_CB_Buf[16];
} UDISK_BOC_CBW, *PXUDISK_BOC_CBW;

/* USB UDisk */
typedef struct __attribute__((packed)) _UDISK_BOC_CSW {
    uint32_t mCBW_Sig;
    uint32_t mCBW_Tag;
    uint32_t mCSW_Residue;
    uint8_t mCSW_Status;
} UDISK_BOC_CSW, *PXUDISK_BOC_CSW;

#ifdef __cplusplus
}
#endif

#endif /*_CH32V20X_USB_H */
