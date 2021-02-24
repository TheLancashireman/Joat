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

// Operating modes
#define m_start		0
#define m_freq		1
#define m_cap		2
#define m_ind		3
#define m_prog		4
#define m_hvp		5
#define m_max		5

#define lcd_rs		7
#define lcd_e		6
#define lcd_d4		5
#define lcd_d5		4
#define lcd_d6		3
#define lcd_d7		2

extern void init(void);

#endif
