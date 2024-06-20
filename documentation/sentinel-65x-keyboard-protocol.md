# Sentinel 65X Keyboard Protocol

## Keyboard Wire Protocol

The keyboard wire protocol of the Sentinel 65X is very similar to the SNES controller protocol (ref: <https://gamesx.com/controldata/snesdat.htm>)

### Clock

* **TO-DO:** Verify that a 100kHz clock will work with an ATMEGA-based Arduino as a typical "slow" keyboard controller

To support slow or complex keyboard controllers, clock rate is limited to a maximum of 100kHz.

**Rising Edge:** Signal for the keyboard to process and latch-out the next data bit to the `DATA` line from the keyboard's Keyboard State Register.

**High:** The keyboard processes and latches next data bit from the Keyboard State Register to the `DATA` line. Once the `DATA` line is stable, the keyboard can continue processing scan codes. Sentinel 65X may process any application code during this time.

**Falling Edge:** Keyboard must keep `DATA` line stable until next rising edge. Signal to Sentinel 65X to sample the `DATA` line.

**Low:** Sentinel 65X samples the `DATA` line and shifts in the logic level into its Keyboard State Register. After 16 bits are received, the Sentinel 65X will pulse the `LATCH` line high as soon as possible after the `CLOCK` falling edge. Once the `DATA` line is sampled and processed, the Sentinel 65X may continue processing application code.

### Latch

**Rising Edge:** Signal for the keyboard to start loading the next scan-code into the keyboard's Keyboard State Register. The rising edge of `LATCH` will be sent by the Sentinel 65X as soon as possible after the falling edge of `CLOCK` to give the keyboard as much time as possible to load the next scan code into the keyboard's Keyboard State Register.

**High:** The keyboard loads the next scan code into the Keyboard State Register. `LATCH` will be held high by the Sentinel 65X for a minimum of 1uS.

**Falling Edge:** The keyboard can continue to load the next scan code into the Keyboard State Register, or process scan codes, until the next rising edge of `CLOCK`.

**Low:** The keyboard can continue to load the next scan code into the Keyboard State Register, or process scan codes, until the next rising edge of `CLOCK` or `LATCH`.

### Data

The `DATA` line is a 3.3V CMOS logic level. When the Sentinel 65X samples the `DATA` line after the falling edge of `CLOCK`, logic high will be interpreted as a 1 bit, logic low will be interpreted as a 0 bit.

## Keyboard Modes

### Mode 0

A single key-down scan code is repeated while the key is held down, until the corresponding key-up scan code is sent. When no keys are pressed, scan code `0x0000` is sent.

This mode requires the Sentinel 65X to process scan codes and return characters, and is most useful for text-mode applications.

### Mode 1

Multiple key-down scan codes are repeated in the order of keys being pressed, until the corresponding key-up scan code is sent. When no keys are pressed, scan code `0x0000` is sent.

This mode requires the application to process scan codes.

### Mode 2

Key-down and key-up scan codes are sent in the order of keys being pressed. When there is no change to scan codes from the last latch, scan code `0x0000` is sent.

This mode requires the application to process scan codes.

### Mode 3

Raw scan codes are sent only on change, modifiers are not set. When there is no change to scan codes from the last latch, scan code `0x0000` is sent.

This mode requires the application to process scan codes, and is most useful for interactive gaming using keyboard controls.

## Caps Lock, Num Lock, and Scroll Lock

As there is no communication channel from the Sentinel 65X back to the keyboard, it is up to the keyboard to implement Caps Lock, Num Lock, and Scroll Lock status indications. This may cause status indications to be out of sync with the lock status the Sentinel 65X knows about. Keyboards should start up with all locks off.

Alternatively, keyboards may process lock scan codes internally and map to modifier keys in order to avoid synchronisation issues between lock status and lock indicators.

## Keyboard State Register

### Register Map

|Bit|15|14|13|12|11|10|9|8|7|\[6:0]|
|-|-|-|-|-|-|-|-|-|-|-|
|**Description**|Left Ctrl|Left Shift|Left Alt|Left Meta|Right Ctrl|Right Shift|Right Alt|Right Meta|Key State|Scan Code|

### Modifier Keys - High Byte

Meta refers to the "Windows" key found on most modern Windows PC keyboards, the "Command" key found on all Macintosh keyboards, or the various logo or meta keys found on other kinds of keyboards.

High byte \[15:8] is identical to the USB Keyboard modifier key status byte (ref: <https://wiki.osdev.org/USB_Human_Interface_Devices#USB_keyboard>)

### Scan Codes - Low Byte

Key State bit \[7] is clear on key-down, set on key-up.

Low byte \[7:0] is identical to the PS/2 Keyboard Scan Code Set 1 single-byte codes (ref: <https://wiki.osdev.org/PS/2_Keyboard#Scan_Code_Set_1>), where key-down scan codes clear bit \[7] and key-up scan codes set bit \[7]. Multi-byte PS/2 scan codes are mapped according to the following table, setting and clearing bit 7 on key-up and key-down as appropriate:

* **TO-DO:** Expand this to complete set of extended PS/2 and USB scan codes.

|Sentinel 65X Scan Code|PS/2 Scan Code|Key name|
|-|-|-|
||0xE0, 0x1C|Keypad Enter|

### Special Register Values

`0x0000` No keys pressed (Mode 0 and 1) or no key state change (Mode 2 and 3)

`0xFF01` Keyboard rollover error: too many simultaneous keys pressed, some key-down events may be lost.

`0xFF02` Keyboard POST error: keyboard has failed and is unusable. Scan code will be repeated continuously until keyboard is power-cycled and successfully starts.

`0xFF03` Keyboard error: undefined error, some scan codes may be lost.

`0xFFFF` State register is unset, do not process this scan code

## Keyboard controller algorithm

1. Set keyboard state register to `0xFFFF`
1. Set up interrupt for rising edge on LATCH line
    1. Load keyboard current state into shift register
1. Set up interrupt for rising edge on CLOCK line
    1. If state register `&` `0x8000`, then set DATA line high, else set DATA line low
    1. `Left-shift` state register by `1` bit
    1. State register `|=` `1`
1. Wait for interrupts

## Sentinel 65X keyboard read algorithm

1. Start a 100kHz clock that drives the `CLOCK` line, starting logic-low
1. Drive `LATCH` high for 1us
1. Set keyboard state register to `0x0000`
1. Set bit counter to `0`
1. Wait for falling edge on `CLOCK` line
1. Sample `DATA` line - logic high translates to a 1 bit, logic low translates to a 0 bit.
1. Keyboard State Register `|=` `DATA` line logic level
1. Increment bit counter
1. If bit counter `<` `16`
    1. `Left-shift` state register by `1` bit
    1. Go to main routine step 5
1. If bit counter `>=` `16`
    1. Process Keyboard State Register (i.e. draw the rest of the owl). Stop the clock driving the `CLOCK` line if necessary.
    1. Go to main routine step 2
    