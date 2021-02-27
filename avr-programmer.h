/* avr-programmer.h - programmer for AVR microcontrollers using SPI
 *
 * (c) David Haworth & Randall Bohn
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
 *	along with Joat  If not, see <http://www.gnu.org/licenses/>.
*/

// Copyright notice from the original source file (ArduinoISP.ino)
//
// ArduinoISP
// Copyright (c) 2008-2011 Randall Bohn
// If you require a license, see
// http://www.opensource.org/licenses/bsd-license.php

#ifndef AVR_PROGRAMMER_H
#define AVR_PROGRAMMER_H	1

#include <Arduino.h>
#include "joat.h"

// Configure SPI clock (in Hz).
// E.g. for an ATtiny @ 128 kHz: the datasheet states that both the high and low
// SPI clock pulse must be > 2 CPU cycles, so take 3 cycles i.e. divide target
// f_cpu by 6:
//     #define SPI_CLOCK            (128000/6)
//
// A clock slow enough for an ATtiny85 @ 1 MHz, is a reasonable default:
#define SPI_CLOCK 		(1000000/6)

// Configure which pins to use
#define PIN_VCC		9
#define PIN_RESET	10
#define PIN_MOSI	11
#define PIN_MISO	12
#define PIN_SCK		13

// Configure the baud rate:
#define BAUDRATE	19200

#define HWVER 2
#define SWMAJ 1
#define SWMIN 18

// STK Definitions
#define STK_OK      0x10
#define STK_FAILED  0x11
#define STK_UNKNOWN 0x12
#define STK_INSYNC  0x14
#define STK_NOSYNC  0x15
#define CRC_EOP     0x20	//ok it is a space...

#define SPI_MODE0	0x00
#define EECHUNK		32

typedef struct avrp_param_s
{
	uint8_t devicecode;
	uint8_t revision;
	uint8_t progtype;
	uint8_t parmode;
	uint8_t polling;
	uint8_t selftimed;
	uint8_t lockbytes;
	uint8_t fusebytes;
	uint8_t flashpoll;
	uint16_t eeprompoll;
	uint16_t pagesize;
	uint16_t eepromsize;
	uint32_t flashsize;
} avrp_param_t;

typedef struct avrp_data_s
{
	uint8_t buff[256];			// Data buffer
	avrp_param_t param;			// Parameter block sent by PC
	unsigned int here;			// Address for reading and writing, set by 'U' command
	unsigned int error;			// Error counter
	uint8_t pmode;				// 0 = waiting, 1 = programming, 2 = done
	uint8_t rst_active_high;	// Depends on device
	uint8_t prog_lamp_count;	// Counter for programming activity
} avrp_data_t;

extern void avr_programmer(void) __attribute__((noreturn));

#endif
