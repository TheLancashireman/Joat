/* capacitance.cpp
 *
 * (c) David Haworth, Jon Nethercott
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
 * This code was adapted from the third example at
 *   https://www.circuitbasics.com/how-to-make-an-arduino-capacitance-meter/
 *
 * After more research I found the original author, Jon Nethercott, at
 *   https://wordpress.codewrite.co.uk/pic/2014/01/25/capacitance-meter-mk-ii/index.html
 *
 * Jon published a capacitor class for Arduino under GPLv3:
 *   https://github.com/codewrite/arduino-capacitor
 *
 * Unfortunately the class didn't give stable measurements on my hardware, so I
 * extracted the caclulation from the original code into this file.
*/
#include <Arduino.h>
#include "joat.h"
#include "timing.h"
#include "capacitance.h"

#define cdata	joat_data.cap_data

static void cap_init(void);
static void display_capacitance(void);

void capacitance_meter(void)
{
	cap_init();

	for (;;)
	{
		pinMode(cap_in, INPUT);
		digitalWrite(cap_out, HIGH);
		int val = analogRead(cap_in);
		digitalWrite(cap_out, LOW);

		if (val < 1000)
		{
			pinMode(cap_in, OUTPUT);

			cdata.ms = val;
			cdata.unit = cap_pF;
			cdata.capacitance = (double)val * cap_in_to_gnd / (double)(cap_max_adc - val);
		}
		else
		{
			pinMode(cap_in, OUTPUT);
			delay(1);
			pinMode(cap_out, INPUT_PULLUP);
			uint32_t u1 = (uint32_t)read_ticks();
			uint32_t t;
			int digVal;

			do {
				digVal = digitalRead(cap_out);
				t = (uint32_t)read_ticks() - u1;
			} while ( (digVal < 1) && (t < 400000L) );

			pinMode(cap_out, INPUT);
			val = analogRead(cap_out);
			digitalWrite(cap_in, HIGH);

			uint32_t u = ticks_to_micros(t);
			uint32_t dischargeTime = (u / 1000) * 5;
			delay(dischargeTime);
			pinMode(cap_out, OUTPUT);
			digitalWrite(cap_out, LOW);
			digitalWrite(cap_in, LOW);

			cdata.ms = val;
			cdata.capacitance = -(double)u / cap_R_pullup / log(1.0 - (double)val / (double)cap_max_adc);

			if ( cdata.capacitance > 1000.0)
			{
				cdata.capacitance = cdata.capacitance / 1000.0;
				cdata.unit = cap_uF;
			}
			else
			{
				cdata.unit = cap_nF;
			}
		}

		display_capacitance();

		tick_delay(MILLIS_TO_TICKS(500));
	}
}

static void cap_init(void)
{
	pinMode(cap_out, OUTPUT);
	pinMode(cap_in, OUTPUT);
}

static void display_capacitance(void)
{
	uint8_t np = 0;

	lcd->setCursor(0,1);

	if ( cdata.capacitance < 10.0 )
		np += lcd->print(cdata.capacitance, 3);
	else
		np += lcd->print(cdata.capacitance, 2);

	if ( cdata.unit == cap_pF )
		np += lcd->print(F("pF "));
	else if ( cdata.unit == cap_nF )
		np += lcd->print(F("nF "));
	else
		np += lcd->print(F("uF "));

	np += lcd->print(cdata.ms);
	np += lcd->print(F("ms"));

	fill_spaces(16-np);
}
