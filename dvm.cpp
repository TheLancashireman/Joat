/* dvm.cpp
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
#include <Arduino.h>
#include "joat.h"
#include "timing.h"
#include "dvm.h"

static void dvm_init(void);
static void display_voltage(uint8_t pin, uint8_t x, uint8_t y);

void dvm(void)
{
	dvm_init();

	for (;;)
	{
		(void)analogRead(dvm_1);	//	Allow input multiplexer to settle
		tick_delay(MILLIS_TO_TICKS(10));
		display_voltage(dvm_1, 0, 0);

		(void)analogRead(dvm_2);	//	Allow input multiplexer to settle
		tick_delay(MILLIS_TO_TICKS(10));
		display_voltage(dvm_2, 11, 0);

		(void)analogRead(dvm_3);	//	Allow input multiplexer to settle
		tick_delay(MILLIS_TO_TICKS(10));
		display_voltage(dvm_3, 0, 1);

		(void)analogRead(dvm_4);	//	Allow input multiplexer to settle
		tick_delay(MILLIS_TO_TICKS(10));
		display_voltage(dvm_4, 11, 1);

		tick_delay(MILLIS_TO_TICKS(450));
	}
}

static void dvm_init(void)
{
	analogReference(DEFAULT);
	pinMode(dvm_1, INPUT);
	pinMode(dvm_2, INPUT);
	pinMode(dvm_3, INPUT);
	pinMode(dvm_4, INPUT);
	lcd->setCursor(0, 1);
	fill_spaces(16);
	tick_delay(MILLIS_TO_TICKS(1000));
	lcd->setCursor(0, 0);
	fill_spaces(16);
}

static void display_voltage(uint8_t pin, uint8_t x, uint8_t y)
{
	double scale = 5.0/1024.0;

	double v = ((double)analogRead(pin)) * scale;
	

	lcd->setCursor(x, y);
	lcd->print(v, 2);
	lcd->print(F("v"));
}
