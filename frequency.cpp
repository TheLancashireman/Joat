/* frequency.cpp - frequency measurement using timer1
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
 *	along with Joat  If not, see <http://www.gnu.org/licenses/>.
*/
#include <Arduino.h>
#include "joat.h"
#include "timing.h"
#include "frequency.h"

#define ICP1	8	// Input capture 1 is on pin 8/PB0

#define fdata	joat_data.freq_data

static void display_freq(double f);

/* ISR(TIMER1_OVF_vect) - interrupt handler for the timer overflow
 *
 * Increment a counter
*/
ISR(TIMER1_OVF_vect)
{
	fdata.n_oflo++;
}

/* ISR(TIMER1_CAPT_vect) - interrupt handler for the capture interrupt
 *
 * Store the capture time and increment a counter
*/
ISR(TIMER1_CAPT_vect)
{
	fdata.cap = ICR1;		// Read the time of the capture
	fdata.n_cap++;
}

/* freq() - calculate the signal frequency
 *
 * Using the difference between the capture time (from the ISR) and the last known capture time,
 * along with the number of overflows, the interval can be calculated.
 * The number of captures in that interval is also known, so the average frequency can be calculated.
*/
void frequency_meter(void)
{
	uint64_t t;
	uint64_t t0;
	uint32_t elapsed;
	uint8_t nc, no;
	uint16_t v;

	freq_init();
	t0 = read_ticks();

	for (;;)
	{
		t = read_ticks();
		elapsed = t - t0;
		t0 = t;

		fdata.update_interval += elapsed;

		cli();
		nc = fdata.n_cap;
		if ( nc > 0 )		// If there's been at least one capture, read and reset the interrupt handlers' data
		{
			v = fdata.cap;
			no = fdata.n_oflo;
			fdata.n_cap = 0;
			fdata.n_oflo = 0;
		}
		sei();

		if ( nc > 0 )		// If there's been at least one capture, accumulate the time and no of captures.
		{
			fdata.total_time += (uint32_t)v  - (uint32_t)fdata.last_cap + (uint32_t)no * 65536ul;
			fdata.total_cap += nc;
			fdata.last_cap = v;

			// Once per second, calculate and display the frequency
			if ( fdata.update_interval > MILLIS_TO_TICKS(1000) )
			{
				double f = ((double)fdata.total_cap * 16000000.0) / (double)fdata.total_time;
				display_freq(f);

				fdata.total_cap = 0;
				fdata.total_time = 0;
				fdata.update_interval = 0;
			}
		}
		else if ( fdata.update_interval > MILLIS_TO_TICKS(2000) )
		{
			// More than 2 seconds without a pulse; assume 0.0 Hz
			display_freq(0.0);
			fdata.total_cap = 0;
			fdata.total_time = 0;
			fdata.update_interval = 0;
		}
	}
}

static void display_freq(double f)
{
	uint8_t np;
	lcd->setCursor(0, 1);
	np = lcd->print(f, ((f < 100.0) ? 3 : 2));
	np += lcd->print(F("Hz"));
	fill_spaces(16 - np);
}

void freq_init(void)
{
	pinMode(ICP1, INPUT);		// Set up the T1 input capture pin for frequency measurement
	TCCR1B |= 0x40;				// Input capture on leading edge
	TIMSK1 |= 0x21;				// Enable input capture and overflow interrupts
}
