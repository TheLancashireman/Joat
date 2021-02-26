/* timing.cpp - monotonically-increasing time
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
#include "timing.h"

uint64_t timing_time;
uint16_t timing_last_t1;

/* read_ticks() returns an ever increasing time
 *
 * The resolution of the time depends on the scaling of timer 1. With a prescaler of 1,
 * the resolution on a nano is 62.5 ns (16 MHz clock)
 *
 * For read_ticks() to work correctly, you have to call it at least once for each wrap-around of
 * timer 1. At 16 MHz, that's every 4096 us - a tad over 4 ms.
 *
 * Using raw ticks is a bit more efficient than using units like milliseconds and microseconds,
 * because usually you can persuade the compiler to do the computation for you.
*/
uint64_t read_ticks(void)
{
	cli();
	uint16_t t1 = TCNT1;
	uint16_t diff = t1 - timing_last_t1;
	uint64_t retval = timing_time + diff;
	timing_time = retval;
	timing_last_t1 = t1;
	sei();
	return retval;
}

/* tick_delay() does nothing for the specified number of ticks
*/
void tick_delay(uint64_t dly)
{
	uint64_t t0 = read_ticks();

	while ( (read_ticks() - t0) < dly )
	{
		/* Twiddle thumbs */
	}
}

/* init_timing() - initialise timer 1 for timing operation
*/
void init_timing(void)
{
	 TCCR1A = 0;				/* Normal port operation */
	 TCCR1B = 0x01;				/* Enable counter, prescaler = 1; WGM12/3 = 0 */
	 TCCR1C = 0;				/* Probably not needed */
	 TIMSK1 = 0;				/* Disable all the interrupts */
	 TIFR1 = 0x27;				/* Clear all pending interrupts */
	 TCNT1 = 0;
}

/* Arduino compatibility functions
*/
void delay(unsigned long ms)
{
	tick_delay(MILLIS_TO_TICKS(ms));
}

void delayMicroseconds(unsigned int us)
{
	tick_delay(MICROS_TO_TICKS(us));
}

/* Conversion functions - are these needed?
*/
uint64_t micros_to_ticks(uint64_t micros)
{
	return (micros * HZ) / 1000000;
}

uint64_t millis_to_ticks(uint32_t millis)
{
	return micros_to_ticks((uint64_t)millis * 1000);
}
