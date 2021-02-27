# Joat - Jack-of-all-trades

Jack-of-all-trades is a little development tool with several useful features. It is based on an
arduino nano and uses a 2x16 character display.

# UNDER CONSTRUCTION

Current status:
* frequency meter: working
* capacitance meter: working not accurate for values < 1nF
* AVR programmer s/w might be working but Vcc supply to AVR is too low

## Features

* frequency meter
* capacitance meter
* AVR programmer (SPI)

Planned:
* resetting the fuses and erasing AVR microcontrollers using the HVP technique.
* inductance meter
* anything else I can think of that will fit in the flash

## How it works

A standard 2x16 LCD or VFD display provides visual output. The LiquidCrystal class is
used in 4-bit mode with no read pin, thus requiring 6 pins (D2..D7).

Timer1 runs at the full CPU frequency of 16 MHz. This timer provides all the timing for
the application. The standard timing functions from wiring.c are not used. The file timing.cpp
provides a compatibility layer, but for accuracy it is better to use the raw ticks. Time measurement
is passive (no interrupts), which means you have to read the time every 4 ms or less. That is
not usually a problem. Time is monotonically increasing using a 64-bit variable, which is good for the
next 36000 years :-) But of course you can truncate the value to 32 bits or even less if you want.

Two buttons control the operation. The buttons are both connected to analogue pin 6 via resistors.
With no buttons pressed, the intput voltage is about 5v. With button 1 pressed the level drops to about 2.5v.
Button 2 connects the analogue input to ground. The button() function reads the input in a loop until it
is stable, then decides which button. If it is different from the last time, the new value is returned,
Otherwise "none". Note: the "new" value could also be "none". When a change of state is detected, a hold-off
time is started to avoid double-clicks caused by switch bounce.

The main program displays a friendy message, then waits for button input. One button steps through the
modes one at a time. The other button selects the displayed mode and switches to it.

The only way to get back to the main program is to reset the arduino. This avoids having to
disable all the hardware when switching from one mode to another. It also means that less stack is needed
because the functions of the individual features are marked with the noreturn attribute.

The global data for the individual features are gathered together in a union to save RAM space. All
strings for the display are placed in the flash using F(), PSTR() etc.

### Frequency meter

Originally developed by the author as part of the analogue-synth module in
https://github.com/TheLancashireman/synthesiser.

The signal to measure (assumed to be a pulse train or square wave with fairly sharp rise/fall times)
is fed to the input capture pin of timer1 (pin 8 on the nano).

The input capture interrupt stores the captured timer value and increments a capture counter.
The timer overflow interrupt increments a counter. At regular intervals, the capture value and
counters are sampled. The total time since the last sample is calculated from the differences
between the capture values and overflow counts. The frequency is the number of captures
divided by the time in which they occurred. If no captures occur within two seconds, the
frequency is displayed as 0 Hz, this giving a minimum frequency range of 0.5 Hz. The range could be
extended by increasing the wait time.

### Capacitance meter

The capacitance measurement functionality is based on code that was orignally found at
https://www.circuitbasics.com/how-to-make-an-arduino-capacitance-meter/

After more research the original was found at
https://wordpress.codewrite.co.uk/pic/2014/01/25/capacitance-meter-mk-ii/index.html

The calculation was eventually published under GPLv3 at https://github.com/codewrite/arduino-capacitor
This version gave unstable results on my hardware.

The feature in Joat is derived from the original code from www.circuitbasics.com.

### Inductance meter

Inspired by https://www.edabbles.com/2020/06/16/measuring-inductance-with-arduino-nano/

Not implemented yet.

### AVR programmer

A heavily modified version of the ArduinoISP sketch that is part of the arduino 1.8.13 release.

The original has a BSD license - see the source file and http://www.opensource.org/licenses/bsd-license.php
for details.

### AVR HVP

Inspired by ... (tbd)

Not implemented yet.

## License

Copyright David Haworth and others.

ToDo: BSD license for the AVR programmer.

This file is part of Joat.

Joat is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Joat is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Joat.  If not, see <http://www.gnu.org/licenses/>.


GPLv3 to be inserted here. The AVR programmer functionality is BSD.
