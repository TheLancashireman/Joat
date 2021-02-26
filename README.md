# Joat - Jack-of-all-trades

# UNDER CONSTRUCTION

Jack-of-all-trades is a little development tool with several features. It is based on an
arduino nano and uses a 2x16 character display.

## Features

* frequency meter

Planned:
* programming AVR microcontrollers using the serial/SPI technique.
* resetting the fuses and erasing AVR microcontrollers using the HVP technique.
* a capacitance meter
* an inductance meter
* anything else I can think of that will fit in the flash

## How it works

A standard 2x16 LCD of VFD display provides visual output. The LiquidCrystal class is
used in 4-bit mode with no read pin, this requiring 6 pins D2..D7

Two buttons control the operation. The buttons are both connected to analogue pin 6 via resistors.
With no buttons pressed, the intput voltage is about 5v. With button 1 pressed the level drops to about 2.5v.
Button 2 connects the analogue input to ground. The button() function reads the input in a loop until it
is stable, then decides which button. If it is different from the last time, the new value is returned,
Otherwise "none". Note: the "new" value could also be "none". When a change of state is detected, a hold-off
time is started to avoid double-clicks caused by switch bounce.

The main program displays a friendy message, then waits for button input. One button steps through the
modes one at a time. The other button selects the displayed mode and switches to it.

The only way to get back to the main program is to reset the arduino. This avoids having to
disable all the hardware when swithing from one mode to another.

### Frequency meter

Originally developed by the author as part of the analogue-synth module in
https://github.com/TheLancashireman/synthesiser.

Timer1 runs at the full CPU clock frequency of 16 MHz. The signal to measure (assumed to be a
pulse train or square wave with fairly sharp rise/fall times) is fed to the input capture pin
of timer1 (pin 8 on the nano).

The input capture interrupt stores the captured timer value and increments a capture counter.
The timer overflow interrupt increments a counter. At regular intervals, the capture value and
counters are sampled. The total time since the last sample is calculated from the differences
between the capture values and overflow counts. The frequency is the number of captures
divided by the time in which they occurred. If no captures occur within two seconds, the
frequency is displayed as 0 Hz, this giving a minimum frequency range of 0.5 Hz. The range could be
extended by increasing the wait time.

### Capacitance meter

The code was stolen from https://www.circuitbasics.com/how-to-make-an-arduino-capacitance-meter/

After more research the original was found at
https://wordpress.codewrite.co.uk/pic/2014/01/25/capacitance-meter-mk-ii/index.html

The calculation was eventually published under GPLv3 at https://github.com/codewrite/arduino-capacitor

### Inductance meter

Inspired by https://www.edabbles.com/2020/06/16/measuring-inductance-with-arduino-nano/


### AVR programmer

Based on the ArduinoISP sketch that is part of the arduino 1.8.13 release. BSD license -
see the source file and http://www.opensource.org/licenses/bsd-license.php for details.

### AVT HVP

Inspired by ... (tbd)

## License

GPLv3 to be inserted here. The AVR programmer functionality is BSD.
