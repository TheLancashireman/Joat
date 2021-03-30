/* dvm.h
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
#ifndef DVM_H
#define DVM_H	1

#include <Arduino.h>
#include "joat.h"

// Pin selection for inductance measurement.
#define dvm_1		A0
#define dvm_2		A1
#define dvm_3		A2
#define dvm_4		A3

// Note: there's no dvm_data_t; voltage measurement uses no global data

extern void dvm(void) __attribute__((noreturn));

#endif
