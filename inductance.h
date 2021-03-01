/* inductance.h
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
#ifndef INDUCTANCE_H
#define INDUCTANCE_H	1

#include <Arduino.h>
#include "joat.h"

// Pin selection for inductance measurement.
#define ind_out		A4		// Pulse
#define ind_in		8		// ICP1 to measure frequency of ringing.

// Units. The measurement is automatically scaled to these units.
#define ind_uH  0
#define ind_mH  1

// Note: there's no inductance_data_t; inductance measurement uses frequency structure.

extern void inductance_meter(void) __attribute__((noreturn));

#endif
