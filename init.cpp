/* init.cpp - My Very Own init() Function(tm).
 *
 * (c) David Haworth
 *
 *	This file is part of Joat.
 *
 *	Joat is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	Joat is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with Joat.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <Arduino.h>
#include <wiring_private.h>

/* init() - a minimal version of the init() function in wiring.c
 *
 * Using this init function prevents the accidental inclusion of the Arduino version in wiring.c.
 * It avoids attaching the default arduino interrupts and means that accidental use of
 * millis(), micros() and related functions gets detected.
 *
 * Timer initialisation is done in timing.c
*/

void init(void)
{
	sbi(ADCSRA, ADPS2);		// Prescaler for 16 MHz
	sbi(ADCSRA, ADPS1);
	sbi(ADCSRA, ADPS0);
	sbi(ADCSRA, ADEN);		// Enable a/d converter
}
