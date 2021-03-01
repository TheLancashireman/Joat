/* frequency.h
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
#ifndef FREQUENCY_H
#define FREQUENCY_H	1

#include <Arduino.h>
#include "joat.h"

typedef struct frequency_data_s
{
	uint32_t update_interval;
	uint32_t total_time;
	uint16_t total_cap;
	uint16_t last_cap;
	uint16_t cap;
	uint8_t n_oflo;
	uint8_t n_cap;
} frequency_data_t;

extern void frequency_meter(void) __attribute__((noreturn));
extern void freq_init(void);

#endif
