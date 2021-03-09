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
#include <LiquidCrystal.h>
#include "joat.h"
#include "timing.h"

// We cannot use a static constructor because the LiquidCrystal library uses the Arduino delay functions
// and the local replacement hasn't been initialised yet.
LiquidCrystal *lcd;

// This is where the individual functions store global variables.
joat_data_t joat_data;

//
static uint32_t btn_timer;
static uint32_t btn_lasttime;
static uint8_t btn_last;

static void joat_setup(void);
static void display_mode(uint8_t row, uint8_t m);

// TEMPORARY: dummy functions for initial testing
extern void avr_hvp(void) __attribute__((noreturn));

int main(void)
{
	init();
	joat_setup();

	// Initial mode
	uint8_t mode = m_start;

	for (;;)
	{
		uint8_t b = button();

		if ( b == btn_change )
		{
			mode++;
			if ( mode > m_max )
				mode = 0;
			display_mode(1, mode);
		}
		else if ( b == btn_ok && mode <= m_max)
		{
			display_mode(0, mode);
			wipe_row(1);
			
			switch ( mode )
			{
			case m_freq:
				frequency_meter();
				break;

			case m_cap:
				capacitance_meter();
				break;

			case m_ind:
				inductance_meter();
				break;

			case m_prog:
				avr_programmer();
				break;

			case m_hvp:
				avr_hvp();
				break;

			default:
				/* Not reached */
				break;
			}
		}
	}
}

static void joat_setup(void)
{
	// Initialise the timing system (timer1)
	init_timing();

	// Initialise the lcd driver
	lcd = new LiquidCrystal(lcd_rs, lcd_e, lcd_d4, lcd_d5, lcd_d6, lcd_d7);
	lcd->begin(16, 2);

	// Display a friendly greeting
	lcd->setCursor(0,0);
	lcd->print(F("The Joat"));
	lcd->setCursor(0,1);
	lcd->print(F("(c) dh   GPLv3"));

	// Initialise the a/d converter
	analogReference(DEFAULT);
}

void fill_spaces(uint8_t nsp)
{
	while ( nsp > 0 )
	{
		lcd->print(' ');
		nsp--;
	}
}

void wipe_row(uint8_t row)
{
	lcd->setCursor(0, row);
	fill_spaces(16);
	lcd->setCursor(0, row);
}

static void display_mode(uint8_t row, uint8_t m)
{
	wipe_row(row);
	switch (m)
	{
	case m_freq:
		lcd->print(F("Frequency"));
		break;

	case m_cap:
		lcd->print(F("Capacitance"));
		break;

	case m_ind:
		lcd->print(F("Inductance"));
		break;

	case m_prog:
		lcd->print(F("AVR programmer"));
		break;

	case m_hvp:
		lcd->print(F("AVR HVP"));
		break;

	default:
		/* Not reached */
		lcd->print(F("Help!"));
		break;
	}
}

uint8_t button(void)
{
	if ( btn_timer != 0 )
	{
		// Hold off sampling for a while after a button change
		uint32_t now = (uint32_t)read_ticks();
		uint32_t el = now - btn_lasttime;
		if ( btn_timer < el )
			btn_timer = 0;
		else
			btn_timer -= el;
		btn_lasttime = now;
	}
	else
	{
		uint8_t new_btn;
		int16_t av1;
		int16_t av2 = (int16_t)analogRead(btn_pin);
		do {
			av1 = av2;
			av2 = (int16_t)analogRead(btn_pin);
		} while ( abs(av2-av1) > 16);
		if ( av1 < 256 )
			new_btn = btn_ok;
		else if ( av1 < 768 )
			new_btn = btn_change;
		else
			new_btn = btn_none;

		if ( new_btn != btn_last )
		{
			btn_last = new_btn;
			btn_timer = (uint32_t)MILLIS_TO_TICKS(20);
			btn_lasttime = (uint32_t)read_ticks();
			return new_btn;
		}
	}
	return btn_none;
}

// TEMPORARY: dummy functions for initial testing
static void not_implemented(void) __attribute__((noreturn));
static void not_implemented(void)
{
	wipe_row(1);
	lcd->print(F("Not implemented"));
	for (;;) {}
}

void avr_hvp(void)
{
	not_implemented();
}
