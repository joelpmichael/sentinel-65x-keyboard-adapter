# Instructions for using the CH32V203C8T6-EVT-R0 dev board

Initial development was done on this board, as it contains everything required to run a USB keyboard adapter. You can find these dev boards on AliExpress, e.g. <https://www.aliexpress.com/item/1005005069239098.html> and some of them come with extra WCH32V203C8T6 chips if you want to fab your own board and perform surface-mount assembly yourself.

## Power

Ensure the power switch `S2` (top slide switch) is set to ON. System 3.3V power will be supplied from the USB-C port.

## USB Connections

Connect the USB keyboard to `P6` (top USB-A connector), and connect a USB-C cable to `P7` (bottom USB-C connector). A configuration interface is available on the USB-C connector using a standard USB-CDC-ACM providing a command-line interface. You can also connect the USB-C port to a power-only source. You may want to fit 0603 size 5.1k resistors in `R22`/`R23` to make intelligent USB-C power supplies and cables work.

## Debugger Connection

Connect debugger's `SWDIO` to `PA13`, `SWCLK` to `PA14` - right pin bank, right column, below `3V3` and `GND`, above `NRST`. Connect debugger's `RST` to `NRST` and `GND` to `GND` - right pin bank, right column.

## Sentinel 65X Connection

**IMPORTANT NOTE FOR SENTINEL 65X PROTOTYPE 4 OWNERS:** Prototopye 4 uses 3.3V on the SNES controller port, and was changed in Prototype 4.1 to use 5V. If you own a first-release Prototype 4 board which has not been modified to use 5V on the SNES controller ports, you can omit the level shifters on the `CLOCK`, `LATCH`, and `DATA` lines.

Connect SNES interface cable to Sentinel 65X Controller 1 port. Connect `+5V` to a level-shifter board "high" side voltage reference, `GND` to the level-shifter board GND reference, `SNES_CLK` to `PB0`, `SNES_LATCH` to `PB1`, `SNES_DATA0` to `PB2` via the level shifter data lines, `GND` from the level shifter to `GND`, and "low" side voltage reference to `3V3`.

## LED Connection

Connect PC13 to LED1 (both in right GPIO bank left column) for Status LED

## PS/2 Keyboard Connection

The PS/2 keyboard mode is not (officially) supported. You may be able to get it running using similar level-shifting components and connections as the dedicated adapter board.

## GPIO Keyboard Example

There are 19 GPIO pins available for use in custom firmware. These pins have been chosen to provide many of the peripheral functions available on the CH32V203 chip, including ADC, I2C, SPI, and timers.
