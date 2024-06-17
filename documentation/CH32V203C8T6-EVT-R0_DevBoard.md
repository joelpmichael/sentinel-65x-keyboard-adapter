# Instructions for using the CH32V203C8T6-EVT-R0 dev board

Initial development was done on this board, as it contains everything required to run a USB keyboard adapter. You can find these dev boards on AliExpress, e.g. <https://www.aliexpress.com/item/1005005069239098.html> and some of them come with extra WCH32V203C8T6 chips if you want to fab your own board and perform surface-mount assembly yourself.

## Power

Ensure the power switch S2 (top slide switch) is set to OFF. System 3.3V power will be supplied from the Sentinel 65X.

## USB Connections

Connect the USB keyboard to P6 (top USB-A connector), and connect a USB-C cable to P7 (bottom USB-C connector). Configuration interface is available on the USB-C connector, but power-only is fine. You may want to fit 0603 size 5.1k resistors in R22/R23 to make intelligent USB-C power supplies and cables work.

## Debugger Connection

Connect debugger's SWDIO to PA13, SWCLK to PA14 - Right pin bank, right column, below 3V3 and GND, above NRST.

## Sentinel 65X Connection

Connect to Sentinel 65X Controller 1 port. Connect SNES_CLK to PB0, SNES_LATCH to PB1, SNES_DATA0 to PB2, I2C_SCL to PB10, I2C_SDA to PB11, GND to GND, and 3.3V to 3.3V.

## LED Connection

Connect PC13 to LED1 (both in right GPIO bank left column) for Status LED

## PS/2 Keyboard Connection

The PS/2 keyboard mode is not (officially) supported. You may be able to get it running using similar level-shifting components and connections as the dedicated adapter board.

## GPIO Keyboard Example

There are 16 GPIO pins available for use in custom firmware.
