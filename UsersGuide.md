# The Joat - User's Guide

## General setup

Use breadboard hookup wires (preferably short ones) to connect the sensor points (J1 and J2) to
the pins of the 40-way ZIF socket. The component under test can be easily inserted into the
ZIF socket at the correct place.

On power-up the display shows an identification message. Press the SCROLL button to step through the modes menu.
When the desired mode shows on the display, press the OK button.

To get back to the modes menu, press the reset button on the Arduino. It is wise to remove the device
from the ZIF socket before pressing reset.

## Sensor point descriptions

* J1.1 - 12v supply controlled by Arduino (HVP mode)
* J1.2 - Input to Schmitt trigger/comparator
* J1.3 - Output of Schmitt trigger/comparator
* J1.4 - A1 - DVM top right
* J1.5 - D10 - RESET control in serial programming mode

* J1.6 - 5v supply controller by Arduino (Serial/HVP mode)
* J1.7 - No connection (to be defined)
* J1.8 - D13/SCK - clock signal in serial programming mode
* J1.9 - D12/MISO - data signal in serial programming mode
* J1.10 - D11/MOSI - data signal in serial programming mode

* J2.1 - A0 - Negative connection for capacitance meter - DVM top left
* J2.2 - A2 - Positive connection for capacitance meter - DVM top right
* J2.3 - D8/ICP1 - input for frequency and inductance modes (TTL level)
* J2.4 - Inductance meter comparator output
* J2.5 - A3 - DVM bottom right

* J2.6 - C5 = 0.15 uF
* J2.7 - C4 = 0.22 uF
* J2.8 - C3 = 0.47 uF
* J2.9 - C2 = 1.0 uF
* J2.10 - C1 = 2.2 uF

* J3.1 - Analogue ground
* J3.2 - Analogue ground
* J3.3 - LC trigger
* J3.4 - LC trigger

* J4.1 - Gnd
* J4.2 - Gnd
* J4.3 - Gnd
* J4.4 - Gnd
* J4.5 - Gnd

## Capacitance meter

Connect J2.1 and J2.2 to the capacitor to test.

Select capacitance meter from modes menu.  The display shows the value of the capacitor.

## Frequency meter

If the signal to measure is a TTL level signal with fast rise and fall times, connect it directly
to J2.3

If the signal is low-level or has slow rise or fall times, connect it to J1.2 and connect J1.3 to J2.3.
Adjust the trigger level using the potentionmeter.

Select frequency meter from the modes menu. The display shows the frequency.

## Inductance meter

This mode is under development.

Connect J2.4 to J2.3. Connect the coil to test between J2.5 and ground (e.g. J2.10)

Connect the range capacitor across the coil (how? ... to be defined)

Select inductance meter from the modes menu.

Select the range capacitor from the inductance menu using the SCROLL button, When the correct value
shows, press OK.

The display shows the value at the lower left. Upper left is the measured frequency.
Upper right is the number of discarded cycles. Lower right is the number of cycles averaged.

## AVR programmer

Do not insert the AVR device into the ZIF socket until prompted.

Connect the Arduino USB port to your PC. Set up avrdude to use 19200 baud.

For an ATTiny device (8-pin), connect according to the following list:
* J1.5 to ATTiny pin 1
* J1.6 to ATTiny pin 8
* J1.8 to ATTiny pin 7
* J1.9 to ATTiny pin 6
* J1.10 to ATTiny pin 5
* J4.x (Gnd) to ATTiny pin 4

For other AVR devices, connect as described in the data sheet.

Select AVR Programmer from the modes menu.

Insert the AVR device when prompted, then press OK.

The display shows "Vcc on"  and a heartbeat sign (.oOo). The Joat is now under the control of avrdude. When
programming is complete, remove the AVR device when prompted and press OK.

ToDo: an emergency exit button to turn off Vcc, in case communication with avrdude fails.

## AVR high-voltage programmer (HVP)

To be defined.


