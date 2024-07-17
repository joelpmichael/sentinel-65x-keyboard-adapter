# Keyboard Adapter Configuration

## Configuration Options

### Keyboard Mode

**Mode 0:** A single key-down scan code is repeated while the key is held down, until the corresponding key-up scan code is sent. When no keys are pressed, scan code `0x0000` is sent. When the AltGr option is enabled (e.g. when using an ISO keyboard) and the AltGr modifier key is pressed, the AltGr scan code is sent immediately before the key-down scan code but not on repeated scan codes.

**Mode 1:** Multiple key-down scan codes are repeated in the order of keys being pressed, until the corresponding key-up scan code is sent. When no keys are pressed, scan code `0x0000` is sent.

**Mode 2:** Key-down and key-up scan codes are sent in the order of keys being pressed. When there is no change to scan codes from the last latch, scan code `0x0000` is sent.

**Mode 3:** Raw scan codes are sent only on change, modifiers are not set. When there is no change to scan codes from the last latch, scan code `0x0000` is sent.

### Auto-repeat

Generate periodic key-up and key-down virtual key-press events while keys are held down when in Mode 0 and Mode 1.

### Map lock to modifier

Set shift-modifier based on Caps-, Num-, and Scroll-lock status. The Sentinel 65X does not need to keep the state of lock keys when this configuration option is enabled.

### Swap Left-Ctrl and Caps

Swap the scan codes of Left-Ctrl and Caps Lock keys. Some people prefer this keyboard layout.

### Swap Alt and Meta

Swap the scan codes of both Alt and Meta keys. This is useful when swapping between Macintosh and Windows keyboard layouts where the respective Meta and Alt keys are swapped.

### Right Alt is AltGr

Use right-alt key as AltGr, for use on ISO (European) keyboards.

### Option bit 7

(TODO)

### Scan Code Mapping

Arbitrary re-mapping of scan codes.

## USB (Serial) Interface

## I2C Interface

### Configuration Register: Address 0x00

|Bit|7|6|5|4|3|2|\[1:0]|
|-|-|-|-|-|-|-|-|
|**Description**|(unused)|Use AltGr|Swap Alt-Meta|Swap LCtrl-Caps|Auto-Repeat|Map Lock-Modifer|Mode|

### Scan Code Mapping: Address 0x01 to 0x7F

When set to 0, will reset the mapping to default. Otherwise, maps the scan code to the specified value.
