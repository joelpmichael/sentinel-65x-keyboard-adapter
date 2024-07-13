# Sentinel 65X Controller Port Keyboard Protocol

## Controller Port Wire Protocol

Refer to the [Sentinel 65X Controller Port Protocol](sentinel-65x-controller-port-protocol.md) specification for further information.

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

|Bit|15|\[14:8]|7|6|5|4|\[3:0]|
|-|-|-|-|-|-|-|-|
|**Description**|Key State|Scan Code|Ctrl|Shift|Alt|Meta|Signature|

### Key State

### Signature - Low byte \[3:0]

Always set to `0b0010` on scan code.

### Modifier Keys - Low Byte \[7:4]

Meta refers to the "Windows" key found on most modern Windows PC keyboards, the "Command" key found on all Macintosh keyboards, or the various logo or meta keys found on other kinds of keyboards.

Low byte \[7:4] is similar to the USB Keyboard modifier key status byte (ref: <https://wiki.osdev.org/USB_Human_Interface_Devices#USB_keyboard>), with left and right modifiers logically-OR'ed together.

### Scan Codes - High Byte

Key State bit \[15] is clear on key-down, set on key-up.

High byte \[15:8] is similar to the PS/2 Keyboard Scan Code Set 1 single-byte codes (ref: <https://wiki.osdev.org/PS/2_Keyboard#Scan_Code_Set_1>), where key-down scan codes clear bit \[15] and key-up scan codes set bit \[15]. Multi-byte PS/2 scan codes are mapped according to the following table, setting and clearing bit \[15] on key-up and key-down as appropriate:

* **TO-DO:** Expand this to complete set of extended PS/2 and USB scan codes.

|Sentinel 65X Scan Code|PS/2 Scan Code|Key name|
|-|-|-|
||0xE0, 0x1C|Keypad Enter|

### Special Register Values

`0x0002` No keys pressed (Mode 0 and 1) or no key state change (Mode 2 and 3)

## Keyboard Fault Codes

Sentinel 65X keyboard adapters always support the controller port [extended fault code specification](sentinel-65x-controller-port-protocol.md#device-fault-codes) by setting the low byte \[7:0] to `0x2F`. The following high bytes \[15:8] are defined for keyboard errors:

|Byte|Error|
|-|-|
|0x01|Keyboard rollover error: too many simultaneous keys pressed, some key-down events may be lost.|
|0x02|Keyboard buffer full: some events were lost|

## Keyboard Controller Algorithm

1. Set Keyboard State Register to `0xFFFF`
1. Set up interrupt for rising edge on LATCH line
    1. Load keyboard current state into Keyboard State Register
1. Set up interrupt for rising edge on CLOCK line
    1. If state register `&` `0x8000` `==` `0x0000`, then set DATA line high, else set DATA line low. The data line logic level is inverted from the bit value.
    1. `Left-shift` state register by `1` bit
    1. State register `|=` `1`
1. Wait for interrupts
