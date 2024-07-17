# Design Notes

An unstructured set of notes taken during prototyping, development, testing, and building. Probably contains swearing.

## CH32V203C8T6-EVT-R0 Dev Board

### Dev Board Power/Reset

- S1 (lower-right momentary) = reset. 10k pull-up, S1 parallel with 0.1u capacitor to ground.
- S2 (upper-right) = master power switch from 5V USB (ewww)
- Able to feed from 3V3 pins

### Dev Board USB Connections

- P6 (top USB-A) -> PA11/12 = USB1
- P7 (bottom USB-C) -> PB6/7 = USB2
- Do not use P8 (bottom USB-A)
- USB-C CC resistors R22/R23 are DNP? Cheap arseholes, it would have cost them $0.001.

### Dev Board Sentinel 65X Connections

- 3V3 -> 3V3
- PB0 -> SNES_CLOCK
- PB1 -> SNES_LATCH
- PB2 -> SNES_DATA0
- PB10 -> I2C_SCL
- PB11 -> I2C_SDA
- GND -> GND

### Dev Board 16-bit GPIO example

- PA0 to 7 (top left GPIO bank)
- PA8 to 10, 15 (bottom right GPIO bank)
- PB3 to 5, 8 (bottom right GPIO bank)

### Dev Board Crystals

- Y1 -> PD0/1 = 8MHz
- Y2 -> PC14/15 = 32.768kHz

### Dev Board Debugger Connection

- SWDIO -> PA13
- SWCLK -> PA14

### Dev Board Boot straps

- Boot0 -> Dedicated
- Boot1 -> PB2

## CH32V203C8T6 MCU notes

### MCU Power/Reset

### MCU USB Connections

When using the USB function, the CPU frequency must be 48MHz, 96MHz or 144MHz. When using the USB high speed function, the clock source of USBHSPLL can only be HSE. 

### MCU Clocks

### MCU Boot straps

- BOOT0: 0 = boot from program flash, 1 = boot-loader or SRAM
- BOOT1: 0 = boot from boot-loader, 1 = boot from SRAM

Program flash = map 0x0800_0000 to 0x0000_0000

Boot-loader = map 0x1FFF_8000 to 0x0000_0000

### MCU Peripherals

I2C1 SCL/SDA on PB6/PB7 (overload with USB) or PB8/PB9; I2C2 SCL/SDA on PB10/PB11

### MCU Memmory

In the non-zero-wait area FLASH and zero-wait area FLASH, user word read/write and vendor configuration word and Boot area read, the following operations need to be done, firstly, HCLK is divided into two (Related
peripheral clocks are also divided at the same time, the impact needs to be evaluated), and then restored after the FLASH operation is completed to ensure that the FLASH access clock frequency does not exceed 60Mhz \(FLASH_ CTLR register bit[25]-SCKMOD can configure FLASH access clock frequency as system clock or half of system clock, the default configuration of this bit is half of system clock).

### MCU Pin Connections

|Pin #|Pin Name|Function|GPIO Alt Functions|
|-----|--------|--------|-------------|
|1|VBAT|Power||
|2|PC13|Status LED||
|3|PC14|System LED||
|4|PC15|Not Connected (tied to GND)||
|5|OSC_IN|HSE Crystal||
|6|OSC_OUT|HSE Crystal||
|7|NRST|Reset||
|8|VSSA|Power||
|9|VDDA|Power||
|10|PA0|(GPIO Example)|ADC_IN0, TIM2_CH1|
|11|PA1|(GPIO Example)|ADC_IN1, TIM2_CH2|
|12|PA2|(GPIO Example)|ADC_IN2, TIM2_CH3|
|13|PA3|(GPIO Example)|ADC_IN3, TIM2_CH4|
|14|PA4|(GPIO Example)|ADC_IN4, SPI1_NSS|
|15|PA5|(GPIO Example)|ADC_IN5, SPI1_SCK|
|16|PA6|(GPIO Example)|ADC_IN6, SPI1_MISO|
|17|PA7|(GPIO Example)|ADC_IN7, SPI1_MOSI|
|18|PB0|SNES_CLOCK||
|19|PB1|SNES_LATCH||
|20|PB2|SNES_DATA (BOOT1) (Internal weak pull-down)||
|21|PB10|I2C_SCL||
|22|PB11|I2C_SDA||
|23|VSS_1|Power||
|24|VDD_VIO_1|Power||
|25|PB12|PS/2 Power Enable (P-FET gate)||
|26|PB13|PS/2 Clock||
|27|PB14|PS/2 Data||
|28|PB15|External Power Detect||
|29|PA8|(GPIO Example)|TIM1_CH1, MCO|
|30|PA9|(GPIO Example)|TIM1_CH2, USART1_TX|
|31|PA10|(GPIO Example)|TIM1_CH3, USART1_RX|
|32|PA11|USB1_D-||
|33|PA12|USB1_D+||
|34|PA13|SWDIO||
|35|VSS_2|Power||
|36|VDD_2|Power||
|37|PA14|SWCLK||
|38|PA15|(GPIO Example)|TIM2_CH1_1, SPI1_NSS_1|
|39|PB3|(GPIO Example)|TIM2_CH2_1, SPI1_SCK_1|
|40|PB4|(GPIO Example)|TIM3_CH1_2, SPI1_MISO_1|
|41|PB5|(GPIO Example)|TIM3_CH2_2, SPI1_MOSI_1|
|42|PB6|USB2_D-||
|43|PB7|USB2_D+||
|44|BOOT0|Boot loader control||
|45|PB8|(GPIO Example)|TIM4_CH3, I2C1_SCL_1, CAN1_RX_2|
|46|PB9|(GPIO Example)|TIM4_CH4, I2C1_SDA_1, CAN1_TX_2|
|47|VSS_3|Power||
|48|VDD_VIO_3|Power||

## Custom board notes

### Board Power/Reset

### Board USB Connections

USB-A on PA11/12, USB-C (with 5.1k pull-downs because not doing that is just rude) on PB6/7

### Board Crystals

- 8MHz HSE
- don't need a LSE because not doing RTC

### Board Debugger

- Easy connection to which debugger? WCH-LinkE?

### Board Boot straps

Use DPDT slide switch to pull Boot0 and Boot1 to desired state.

|State|Boot0|Boot1|
|-|-|-|
|Run|Low|Float|
|Load|High|Low|

## Firmware notes

### USB Host

Ugh, looks like I'll need to complete most of this code in both C and Rust.
