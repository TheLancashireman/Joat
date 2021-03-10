/* inductance.cpp
 *
 * (c) David Haworth
 *
 * This file is part of Joat
 *
 * Joat free software: you can redistribute it and/or modify
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
 * Joat is an Arduino sketch, written for an Arduino Nano
*/
/*
 * This inductance meter is inspired by https://www.edabbles.com/2020/06/16/measuring-inductance-with-arduino-nano/
 *
 * The circuit is similar; The differences are:
 *	- Joat uses an LM393 (dual comparator) instead of the LM339 (quad)
 *	- Joat uses the input capture pin to measure the LC ringing frequency
 *	- The display and general framework is completely different.
 *
 * Using input capture means that the average frequency is computed automatically.
 * The equations for computing the inductance are the same but the implementation is totally different.
 *
 * For an LC resonant circuit:	f = 1 / 2 * pi * sqrt(LC)
 * C in Farads, f in Hertz, L in Henrys
 *
 * Therefore:	L = 1 / (4 * pi**2 * f**2 * C)
 *
 * Using a calculation constant K = 1 / (4 * p**2 * C)
 * the final calculation is L = K / (f * f)
*/
#include <Arduino.h>
#include "joat.h"
#include "timing.h"
#include "inductance.h"
#include "frequency.h"

#define idata	joat_data.freq_data

#define C1					2.2e-6					// ToDo measure capacitors using Joat's capacitance meter
#define C2					1.0e-6
#define C3					0.47e-6
#define C4					0.22e-6
#define C5					0.15e-6
#define Pi					3.14159265

#define MIN_DISCARD		1						// Minimum number of oscillation cycles to ignore
#define MAX_DISCARD		3						// Maximum number of oscillation cycles to ignore
#define MIN_SAMPLES		2						// Need at least this many cycles
#define MAX_SAMPLES		3						// Number of oscillation cycles at which to stop measurement
#define MAX_TICKS		MILLIS_TO_TICKS(500) 	// Maximum measurement time


static void ind_init(void);
static void trigger_LC(void);
static void discharge_LC(void);
static uint8_t measure_oscillation(void);
static double select_capacitor(void);

static void calculate_inductance(double cc);
static void display_error(uint8_t err);

void inductance_meter(void)
{
	uint8_t err;
	double calc_constant;	// Constant for calculation that depends on chosen capacitor

	ind_init();

	calc_constant = select_capacitor();

	for (;;)
	{
		// Trigger the LC circuit
		trigger_LC();

		// Measure the oscillation
		err = measure_oscillation();

		// Switch on the discharge
		discharge_LC();

		if ( err == 0 )
		{
			// Enough oscillations captured; calculate and display the inductance
			calculate_inductance(calc_constant);
		}
		else
		{
			// Display an error code
			display_error(err);
		}

		for ( int i = 0; i < 100; i++ )
		{
			if ( button() == btn_change )
			{
				calc_constant = select_capacitor();
				break;
			}

			tick_delay(MILLIS_TO_TICKS(10));
		}
	}
}

/* select_capacitor() - select the capacitor and return the calculation constant
*/
double select_capacitor(void)
{
	double c;
	uint8_t update = 1;
	uint8_t b;

	lcd->setCursor(0, 1);
	fill_spaces(16 - lcd->print(F("Capacitor:")));
	
	do
	{
		if ( update )
		{
			lcd->setCursor(11, 1);
			switch (idata.capacitor_no)
			{
			case 2:		c = C2;		lcd->print(F("1.0 "));	break;
			case 3:		c = C3;		lcd->print(F("0.47"));	break;
			case 4:		c = C4;		lcd->print(F("0.22"));	break;
			case 5:		c = C5;		lcd->print(F("0.15"));	break;
			default:	c = C1;		lcd->print(F("2.2 "));	idata.capacitor_no = 1;	break;
			}
			update = 0;
		}

		b = button();

		if ( b == btn_change )
		{
			idata.capacitor_no++;
			update = 1;
		}
	} while ( b != btn_ok );

	return 1.0 / (4.0 * Pi * Pi * c);
}

/* measure_oscillation() - measure the oscillation frequency
 *
 * Returns a non-zero number on error:
 *	1 = no capture events seen
 *	2 = only one capture event seen
 *
 * If the return value is zero, the average period can be calculated from idata.total_time and idata.total_cap
*/
static uint8_t measure_oscillation(void)
{
	uint64_t t0;
	uint32_t elapsed;
	uint16_t first_cap;
	uint8_t first_ncap;
	uint8_t first_oflo;
	uint16_t last_cap;
	uint8_t last_ncap;
	uint8_t last_oflo;
	uint8_t nc, no;
	uint16_t v;

	t0 = read_ticks();

	// Wait for the first capture and discard it.
	// The time is the time since the last measurement, so is useless.
	// Wait for no longer than 1 second
	do {
		cli();
		first_ncap = idata.n_cap;
		first_cap = idata.cap;			// Starting point of the measurement
		first_oflo = idata.n_oflo_cap;
		sei();

		elapsed = read_ticks() - t0;
	} while ( first_ncap < idata.n_discard  && elapsed < MAX_TICKS );

	// No captures seen ==> no ocscillation.
	if ( first_ncap < idata.n_discard )
	{
		if ( idata.n_discard > MIN_DISCARD )
			idata.n_discard--;
		return 1;
	}

	t0 = read_ticks();

	do {
		cli();
		last_ncap = idata.n_cap;
		last_cap = idata.cap;
		last_oflo = idata.n_oflo_cap;
		sei();
		nc = last_ncap - first_ncap;

		elapsed = read_ticks() - t0;
	} while ( nc < MAX_SAMPLES  && elapsed < MAX_TICKS );

	if ( nc > 0 )		// If there's been at least one capture we can perform a calculation
	{
		no = (last_oflo - first_oflo);
		idata.total_time = (uint32_t)last_cap  - (uint32_t)first_cap + (uint32_t)no * 65536ul;
		idata.total_cap = nc;

		if ( nc > MIN_SAMPLES )
		{
			if ( idata.n_discard < MAX_DISCARD )
				idata.n_discard++;
		}
	}
	else
	{
		if ( idata.n_discard > MIN_DISCARD )
			idata.n_discard--;
		return 2;		// There was one oscillation cycle but no more ==> error
	}

	return 0;
}

/* Calculate the frequency of oscillation from the time and number of captures,
 * then use that to calculate the inductance.
 * f = cycles / time_in_secs  = cycles / (time_in_ticks/ticks_per_sec) = (cycles * ticks_per_sec) / time_in_ticks
 * L = K / (f*f)
*/
static void calculate_inductance(double cc)
{
	double f = ((double)idata.total_cap * 16.0e6) / (double)idata.total_time;
	double L = cc / f / f;
	int np;

	lcd->setCursor(0, 0);
	np = lcd->print(F("f="));
	np += lcd->print(f, 1);
	fill_spaces(13-np);
	lcd->print(F("d="));
	lcd->print(idata.n_discard);
	
	lcd->setCursor(0, 1);
	if ( L >= 1.0 )
	{
		np = lcd->print(L, 3);
		np += lcd->print(F("H"));
	}
	else if ( L >= 1.0e-3 )
	{
		np = lcd->print(L*1.0e3, 3);
		np += lcd->print(F("mH"));
	}
	else if ( L >= 1.0e-6 )
	{
		np = lcd->print(L*1.0e6, 3);
		np += lcd->print(F("uH"));
	}
	else
	{
		np = lcd->print(L*1.0e9, 3);
		np += lcd->print(F("nH"));
	}
	fill_spaces(13 - np);
	lcd->print(F("n="));
	if ( idata.total_cap < 10 )
		lcd->print(idata.total_cap);
	else
		lcd->print('#');
}

/* display_error() - display the error code
*/
void display_error(uint8_t err)
{
	lcd->setCursor(11,1);
	lcd->print(F("Err:"));
	if ( err < 10 )
		lcd->print((char)(err + 0x30));
	else
		lcd->print('?');
}

/* trigger_LC() - hit the LC with a short pulse to start the ringing.
*/
static void trigger_LC(void)
{
	pinMode(ind_out, OUTPUT);
	digitalWrite(ind_out, HIGH);
	tick_delay(MILLIS_TO_TICKS(5));
	idata.n_cap = 0;
	pinMode(ind_out, INPUT);
}

/* discharge_LC() - allow remaining charge to dissipate.
 *
 * How does this work, considering the diode? Maybe on the negative-going half-cycles?
 * Note: This state remains during the calculation and the loop delay
*/
static void discharge_LC(void)
{
	pinMode(ind_out, OUTPUT);
	digitalWrite(ind_out, LOW);
}

/* ind_init() - initialise for inductance measurement
 *
 * Initialise frequency measurement
 *
 * The trigger pin doesn't need initialisation; its state (INPUT/OUTPUT) changes during measurement
*/
static void ind_init(void)
{
	freq_init();
	idata.n_discard = MIN_DISCARD;
	idata.capacitor_no = 1;
}
