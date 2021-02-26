/* capacitance.h
 *
 * (c) David Haworth
 *
 * This file is part of Joat
 *
 * Joat is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Joat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Joat.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Joat is written for an Arduino Nano
*/
#ifndef CAPACITANCE_H
#define CAPACITANCE_H	1

#include <Arduino.h>
#include "joat.h"

// Pin selection for capacitance measurement.
// For polarised capacitors, the + terminal goes to cap_out
#define cap_out		A2
#define cap_in		A0

// Maximum ADC value. Might be different on some boards.
#define cap_max_adc				1023

// Calibration values. You might need to adjust these for your board.
#define cap_in_stray_to_gnd		26.3	// Original code
#define cap_R_pullup			34.8	// Original code

#if 0
// Calibration values for other boards. See also Jon Nethercott's library
#define cap_in_stray_to_gnd		24.48	// Original code
#define cap_R_pullup			34.8	// Original code
#endif

// Stray capacitance
#define cap_in_to_gnd           cap_in_stray_to_gnd

// Units. The measurement is automatically scaled to these units.
#define cap_pF  0
#define cap_nF  1
#define cap_uF  2

typedef struct capacitance_data_s
{
	double capacitance;
	uint16_t ms;
	uint8_t unit;
} capacitance_data_t;

extern void capacitance_meter(void) __attribute__((noreturn));

#endif
