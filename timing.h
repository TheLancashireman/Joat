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

#define MICROS_TO_TICKS(u)	((((unsigned long long)(u))*HZ)/1000000)
#define MILLIS_TO_TICKS(m)	MICROS_TO_TICKS(((unsigned long long)(m))*1000)

extern unsigned long long timing_time;
extern unsigned timing_last_t1;

extern unsigned long long read_ticks(void);
extern void tick_delay(unsigned long long dly);
extern unsigned long long micros_to_ticks(unsigned long long micros);
extern unsigned long long millis_to_ticks(unsigned long millis);
extern void init_timing(void);

#endif
