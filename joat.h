/* joat.h
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
#ifndef JOAT_H
#define JOAT_H	1

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "frequency.h"
#include "capacitance.h"

// Operating modes
#define m_freq		0
#define m_cap		1
#define m_ind		2
#define m_prog		3
#define m_hvp		4
#define m_max		4
#define m_start		5	// Deliberately out of range

// LCD/VFD pins (4-bit mode)
#define lcd_rs		7
#define lcd_e		6
#define lcd_d4		5
#define lcd_d5		4
#define lcd_d6		3
#define lcd_d7		2

// Buttons
#define btn_pin		A6	// Buttons use a resistor network and an analogue pin.
#define btn_none	0
#define btn_ok		1
#define btn_change	2

// Data for all modes, packed into a union to save RAM
typedef union
{
	frequency_data_t freq_data;
} joat_data_t;

extern joat_data_t joat_data;
extern LiquidCrystal *lcd;

extern void init(void);
extern uint8_t button(void);
extern void fill_spaces(uint8_t nsp);

#endif
