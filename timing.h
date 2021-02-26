/* timing.h - monotonically-increasing time; replaces arduino millis/micros/delay/etc.
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
#ifndef TIMING_H
#define TIMING_H

#define HZ	16000000

#define MICROS_TO_TICKS(u)	((((uint64_t)(u))*HZ)/1000000)
#define MILLIS_TO_TICKS(m)	MICROS_TO_TICKS(((uint64_t)(m))*1000)

extern uint64_t timing_time;
extern uint16_t timing_last_t1;

extern uint64_t read_ticks(void);
extern void tick_delay(uint64_t dly);
extern uint64_t micros_to_ticks(uint64_t micros);
extern uint64_t millis_to_ticks(uint32_t millis);
extern uint32_t ticks_to_micros(uint32_t ticks);
extern uint32_t ticks_to_millis(uint32_t ticks);
extern void init_timing(void);

#endif
