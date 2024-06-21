# Sentinel 65X Controller Port Protocol

## Controller Port Wire Protocol

The controller port wire protocol of the Sentinel 65X is very similar to the SNES controller protocol (ref: <https://gamesx.com/controldata/snesdat.htm>)

### Clock

* **TO-DO:** Verify that a 100kHz clock will work with an ATMEGA328-based Arduino as a typical "slow" controller port device

To support slow or complex controller port devices, clock rate is limited to a maximum of 100kHz. At least 16 clock cycles of low-to-high-to-low will be sent at the clock frequency after each `LATCH` 

**Rising Edge:** Signal for the controller port device to process and latch-out the next data bit to the `DATA` lines from the controller port device's Controller Port State Register.

**High:** The controller port device processes and latches next data bit from its Controller Port State Register to the `DATA` line. Once the `DATA` line is stable, the controller port device can continue processing. Sentinel 65X may process any application code during this time.

**Falling Edge:** The controller port device must keep its `DATA` line stable until next rising edge. Signal to Sentinel 65X to sample the `DATA0` and `DATA1` lines.

**Low:** Sentinel 65X samples the `DATA0` and `DATA1` lines and shifts in the logic level into the Controller Port State Registers. After all data bits are received, the Sentinel 65X will pulse the `LATCH` line high as soon as possible after the `CLOCK` falling edge. Once the `DATA0` and `DATA1` lines are sampled and processed, the Sentinel 65X may continue processing application code.

### Latch

**Rising Edge:** Signal for the controller port device to start loading the output into the controller port device's Controller Port State Register. The rising edge of `LATCH` will be sent by the Sentinel 65X as soon as possible after the falling edge of `CLOCK` to give the controller port device as much time as possible to load the next data bits into the controller port device's Controller Port State Register.

**High:** The controller port device loads the next data bits into its Controller Port State Register. `LATCH` will be held high by the Sentinel 65X for a minimum of 1uS.

**Falling Edge:** The controller port device can continue to load the next data bits into its Controller Port State Register, or process data bits, until the next rising edge of `CLOCK` or `LATCH`.

**Low:** The controller port device can continue to load the next data bits into its Controller Port State Register, or process data bits, until the next rising edge of `CLOCK` or `LATCH`.

### Data

* **TO-DO:** Verify logic-high and logic-low maps to 0 and 1, <https://www.nesdev.org/wiki/Controller_port_pinout> suggests they are inverted: `The signal read by the CPU is logically inverted from the signal input on the D0-4 lines. A low voltage on D0 will be read as a 1 bit from $4016/4017.`

The `DATA0` and `DATA1` lines are 3.3V CMOS logic level. When the Sentinel 65X samples the `DATA0` and `DATA1` lines after the falling edge of `CLOCK`, logic high will be interpreted as a 1 bit, logic low will be interpreted as a 0 bit.

### Waveform view

![Sentinel 65X Controller Port waveform, across CLOCK, LATCH, and DATA lines](sentinel-65x-controller-port-waveform.png)

## Controller Port State Registers

### Register Map

|Bit|\(N * 8) + \[15:8]|\(N * 8) +\[7:4]|\(N * 8) +\[3:0]|N * 8|
|-|-|-|-|-|
|**Description**|Data MSB|Data LSB|Signature|Extra Data|

### Data - \(N * 8) + \[15:4]

The first byte and high 4 bits of the second byte contains data significant to the device connected.

### Signature - \(N * 8) + \[3:0]

The low 4 bits of the second byte contains the device signature, which identifies the type of device connected which in turn identifies how many bits of data must be received and how to process the data. Controller port peripherals currently supported by the Sentinel 65X are listed in the following table:

|Signature Bits|Peripheral|Notes|
|-|-|-|
|0b0000|[SNES Controller](https://www.nesdev.org/wiki/SNES_controller)||
|0b0001|[SNES Mouse](https://www.nesdev.org/wiki/Super_NES_Mouse)|16 bits extra data|
|0b0010|[Sentinel 65X Keyboard](sentinel-65x-keyboard-protocol.md)|Scan codes|
|0b0011|[Sentinel 65X Keyboard](sentinel-65x-keyboard-protocol.md)|Error codes|
|0b1111|Controller port device fault||

### Special Register Values

`0xFFFF` Controller Port State Register is unset, do not process this data. Controller port device may be unplugged, powered off, has underflowed its controller port state register, or has otherwise failed.

## Sentinel 65X controller port read algorithm

1. Drive `LATCH` high for 1us
1. Zero controller port state registers
1. Set bit counter to `0`
1. Start a 100kHz clock that drives the `CLOCK` line, starting logic-low
1. Wait for falling edge on `CLOCK` line
1. Sample `DATA0` and `DATA1` lines - logic high translates to a 1 bit, logic low translates to a 0 bit.
1. Controller Port State Registers `|=` `DATA0` and `DATA1` lines logic level
1. Increment bit counter
1. When bit counter `==` `16`, check low 4 LSBs of the Controller Port State Registers to determine type of device connected to controller port
1. If device type requires more than 16 bits of data to be read, repeat steps 5 to 8 until all data is read
1. When all data is read, copy controller port state registers to memory
1. Repeat steps 1 to 3
1. Stop the clock and process the memory copy of the controller port state registers (i.e. draw the rest of the owl)
1. Continue from step 4
