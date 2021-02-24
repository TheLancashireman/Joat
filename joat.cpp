/* joat.cpp - Jack of all trades: frequency/capacitance/inductance meter, avr programmer, ...
 *
 * (c) David Haworth
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
#include <Arduino.h>
#include <joat.h>
#include <timing.h>
#include <LiquidCrystal.h>

/* We cannot use a static constructor because the LiquidCrystal library uses the Arduino delay functions
 * and the local replacement hasn't been initialised yet.
*/
LiquidCrystal *lcd;
uint8_t mode;

static void setup(void);

int main(void)
{
	init();
	setup();

	for (;;)
	{
	}
}

static void setup(void)
{
	// Initialise the timing system (timer1)
	init_timing();

	// Initialise the lcd driver
	lcd = new LiquidCrystal(lcd_rs, lcd_e, lcd_d4, lcd_d5, lcd_d6, lcd_d7);
	lcd->begin(16, 2);

	// Display a friendly greeting
	lcd->setCursor(0,0);
	lcd->print(F("Joat"));
	lcd->setCursor(0,1);
	lcd->print(F("(c) dh   GPLv3"));
}
