/* avr-programmer.cpp - programmer for AVR microcontrollers using SPI
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

#include <Arduino.h>
#include <SPI.h>
#include "joat.h"
#include "timing.h"
#include "avr-programmer.h"

#define avrpdata	joat_data.avrp_data

static inline uint16_t beget16(uint8_t *addr)
{
	return addr[0]*256 + addr[1];
}

static void avrp_init(void);
static void reset_target(uint8_t reset);
static uint8_t getch(void);
static void fill(int n);
static void prog_lamp(int state);
static uint8_t spi_transaction(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
static void empty_reply(void);
static void breply(uint8_t b);
static void set_parameters(void);
static void get_version(uint8_t c);
static void start_pmode(void);
static void end_pmode(void);
static void universal(void);
static void flash(uint8_t hilo, unsigned int addr, uint8_t data);
static void commit(unsigned int addr);
static unsigned int current_page(void);
static void write_flash(int length);
static uint8_t write_flash_pages(int length);
static uint8_t write_eeprom(unsigned int length);
static uint8_t write_eeprom_chunk(unsigned int start, unsigned int length);
static void program_page(void);
static uint8_t flash_read(uint8_t hilo, unsigned int addr);
static char flash_read_page(int length);
static char eeprom_read_page(int length);
static void read_page(void);
static void read_signature(void);
static void avrisp(void);
static void vcc(uint8_t power);

static const char PROGMEM twiddle[4]	= { '-', 0x8c, '|', '/' };
static const char PROGMEM beat[4]		= { '.', 'o', 'O', 'o' };

void avr_programmer(void)
{
	avrp_init();

	for (;;)
	{
		uint8_t t0 = 4;
		unsigned err0 = 0;

		lcd->setCursor(0,1);
		lcd->print(F("Insert AVR  [OK]"));

		while ( button() != btn_ok )
		{	// Wait
		}

		// Turn on power to Vcc
		wipe_row(1);
		lcd->print(F("Vcc on"));
		vcc(1);
		tick_delay(MILLIS_TO_TICKS(500));

		while ( avrpdata.pmode < 2 )
		{
			uint8_t t = ((uint32_t)read_ticks() >> 22) & 0x3;

			if ( t != t0 )
			{
				t0 = t;
				lcd->setCursor(15, 1);
				lcd->print((char)pgm_read_byte(&beat[t]));
			}

			if ( avrpdata.error != err0 )
			{
				if ( err0 == 0 )
				{
					lcd->setCursor(7, 1);
					lcd->print(F("Error"));
				}
				lcd->setCursor(12, 1);
				lcd->print((char)((avrpdata.error & 0x01) == 0 ? '!' : ' '));
				err0 = avrpdata.error;
			}
			
			if (Serial.available())
			{
				avrisp();
			}
		}

		// Turn off power to Vcc
		vcc(0);
		tick_delay(MILLIS_TO_TICKS(500));

		avrpdata.pmode = 0;

		lcd->setCursor(0,1);
		lcd->print(F("Remove AVR  [OK]"));

		while ( button() != btn_ok )
		{	// Wait
		}
	}
}

static void vcc(uint8_t power)
{
	digitalWrite(PIN_VCC, power ? HIGH : LOW);
}

static void reset_target(uint8_t reset)
{
	uint8_t pval;
	if ( reset )
		pval = avrpdata.rst_active_high ? HIGH : LOW;
	else
		pval = avrpdata.rst_active_high ? LOW : HIGH;

	digitalWrite(PIN_RESET, pval);
}

static uint8_t getch(void)
{
	while ( !Serial.available() )
	{	// Wait
	}
	return Serial.read();
}

static void fill(int n)
{
	for (int x = 0; x < n; x++)
	{
		avrpdata.buff[x] = getch();
	}
}

static void prog_lamp(int state)
{
	if ( state )
	{
		lcd->setCursor(14, 1);
		lcd->print((char)pgm_read_byte(&twiddle[avrpdata.prog_lamp_count & 0x3]));
		avrpdata.prog_lamp_count++;
	}
}

static uint8_t spi_transaction(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
	SPI.transfer(a);
	SPI.transfer(b);
	SPI.transfer(c);
	return SPI.transfer(d);
}

static void empty_reply(void)
{
	if (CRC_EOP == getch())
	{
		Serial.print((char)STK_INSYNC);
		Serial.print((char)STK_OK);
	}
	else
	{
		avrpdata.error++;
		Serial.print((char)STK_NOSYNC);
	}
}

static void breply(uint8_t b)
{
	if ( getch() == CRC_EOP )
	{
		Serial.print((char)STK_INSYNC);
		Serial.print((char)b);
		Serial.print((char)STK_OK);
	}
	else
	{
		avrpdata.error++;
		Serial.print((char)STK_NOSYNC);
	}
}

static void get_version(uint8_t c)
{
	switch (c)
	{
	case 0x80:	breply(HWVER);	break;
	case 0x81:	breply(SWMAJ);	break;
	case 0x82:	breply(SWMIN);	break;
	case 0x93:	breply('S');	break;		// serial programmer
	default:	breply(0);
	}
}

static void set_parameters(void)
{
	// call this after reading parameter packet into buff[]
	avrpdata.param.devicecode = avrpdata.buff[0];
	avrpdata.param.revision   = avrpdata.buff[1];
	avrpdata.param.progtype   = avrpdata.buff[2];
	avrpdata.param.parmode    = avrpdata.buff[3];
	avrpdata.param.polling    = avrpdata.buff[4];
	avrpdata.param.selftimed  = avrpdata.buff[5];
	avrpdata.param.lockbytes  = avrpdata.buff[6];
	avrpdata.param.fusebytes  = avrpdata.buff[7];
	avrpdata.param.flashpoll  = avrpdata.buff[8];
	// ignore buff[9] (= buff[8])

	// following are 16 bits (big endian)
	avrpdata.param.eeprompoll = beget16(&avrpdata.buff[10]);
	avrpdata.param.pagesize   = beget16(&avrpdata.buff[12]);
	avrpdata.param.eepromsize = beget16(&avrpdata.buff[14]);

	// 32 bits flashsize (big endian)
	avrpdata.param.flashsize = avrpdata.buff[16] * 0x01000000
								+ avrpdata.buff[17] * 0x00010000
								+ avrpdata.buff[18] * 0x00000100
								+ avrpdata.buff[19];

	// AVR devices have active low reset, AT89Sx are active high
	avrpdata.rst_active_high = (avrpdata.param.devicecode >= 0xe0);
}

static void start_pmode(void)
{
	// Reset target before driving PIN_SCK or PIN_MOSI

	// SPI.begin() will configure SS as output, so SPI master mode is selected.
	// We have defined RESET as pin 10, which for many Arduinos is not the SS pin.
	// So we have to configure RESET as output here,
	// (reset_target() first sets the correct level)
	reset_target(1);
	pinMode(PIN_RESET, OUTPUT);
	SPI.begin();
	SPI.beginTransaction(SPISettings(SPI_CLOCK, MSBFIRST, SPI_MODE0));

	// See AVR datasheets, chapter "SERIAL_PRG Programming Algorithm":

	// Pulse RESET after PIN_SCK is low:
	digitalWrite(PIN_SCK, LOW);
	tick_delay(MILLIS_TO_TICKS(20));	// discharge PIN_SCK, value arbitrarily chosen
	reset_target(0);
	// Pulse must be minimum 2 target CPU clock cycles so 100 usec is ok for CPU
	// speeds above 20 KHz
	tick_delay(MICROS_TO_TICKS(100));
	reset_target(1);

	// Send the enable programming command:
	tick_delay(MILLIS_TO_TICKS(50));	// datasheet: must be > 20 msec
	spi_transaction(0xAC, 0x53, 0x00, 0x00);
	avrpdata.pmode = 1;
}

static void end_pmode(void)
{
	SPI.end();

	// We're about to take the target out of reset so configure SPI pins as input
	pinMode(PIN_MOSI, INPUT);
	pinMode(PIN_SCK, INPUT);
	reset_target(0);
	pinMode(PIN_RESET, INPUT);
	avrpdata.pmode = 2;
}

static void universal(void)
{
	fill(4);
	breply(spi_transaction(avrpdata.buff[0], avrpdata.buff[1], avrpdata.buff[2], avrpdata.buff[3]));
}

static void flash(uint8_t hilo, unsigned int addr, uint8_t data)
{
	spi_transaction(0x40 + 8 * hilo,
					addr >> 8 & 0xFF,
					addr & 0xFF,
					data);
}

static void commit(unsigned int addr)
{
	prog_lamp(0);

	spi_transaction(0x4C, (addr >> 8) & 0xFF, addr & 0xFF, 0);

	tick_delay(MILLIS_TO_TICKS(20));
	prog_lamp(1);
}

static unsigned int current_page(void)
{
	if ( avrpdata.param.pagesize == 32 )
		return avrpdata.here & 0xFFFFFFF0;

	if ( avrpdata.param.pagesize == 64 )
		return avrpdata.here & 0xFFFFFFE0;

	if ( avrpdata.param.pagesize == 128 )
		return avrpdata.here & 0xFFFFFFC0;

	if ( avrpdata.param.pagesize == 256 )
		return avrpdata.here & 0xFFFFFF80;

	return  avrpdata.here;
}


static void write_flash(int length)
{
	fill(length);
	if ( getch() == CRC_EOP )
	{
		Serial.print((char) STK_INSYNC);
		Serial.print((char) write_flash_pages(length));
	}
	else
	{
		avrpdata.error++;
		Serial.print((char) STK_NOSYNC);
	}
}

static uint8_t write_flash_pages(int length)
{
	int x = 0;
	unsigned int page = current_page();

	while ( x < length )
	{
		if ( page != current_page() )
		{
			commit(page);
			page = current_page();
		}
		flash(LOW, avrpdata.here, avrpdata.buff[x++]);
		flash(HIGH, avrpdata.here, avrpdata.buff[x++]);
		avrpdata.here++;
	}

	commit(page);

	return STK_OK;
}

static uint8_t write_eeprom(unsigned int length)
{
	// here is a word address, get the byte address
	unsigned int start = avrpdata.here * 2;
	unsigned int remaining = length;

	if (length > avrpdata.param.eepromsize)
	{
		avrpdata.error++;
		return STK_FAILED;
	}

	while (remaining > EECHUNK)
	{
		write_eeprom_chunk(start, EECHUNK);
		start += EECHUNK;
		remaining -= EECHUNK;
	}

	write_eeprom_chunk(start, remaining);

	return STK_OK;
}


// write (length) bytes, (start) is a byte address
static uint8_t write_eeprom_chunk(unsigned int start, unsigned int length)
{
	// this writes byte-by-byte, page writing may be faster (4 bytes at a time)
	fill(length);
	prog_lamp(0);

	for ( unsigned int x = 0; x < length; x++)
	{
		unsigned int addr = start + x;
		spi_transaction(0xC0, (addr >> 8) & 0xFF, addr & 0xFF, avrpdata.buff[x]);
		tick_delay(MILLIS_TO_TICKS(45));
	}
	prog_lamp(1);
	return STK_OK;
}

static void program_page(void)
{
	char result = (char) STK_FAILED;
	unsigned int length = 256 * getch();
	length += getch();
	char memtype = getch();

	// flash memory @here, (length) bytes
	if ( memtype == 'F' )
	{
		write_flash(length);
	}
	else if ( memtype == 'E' )
	{
		result = (char)write_eeprom(length);
		if ( getch() == CRC_EOP)
		{
			Serial.print((char) STK_INSYNC);
			Serial.print(result);
		}
		else
		{
			avrpdata.error++;
			Serial.print((char) STK_NOSYNC);
		}
	}
	else
	{
		Serial.print((char)STK_FAILED);
	}
}

static uint8_t flash_read(uint8_t hilo, unsigned int addr)
{
	return spi_transaction(0x20 + hilo * 8, (addr >> 8) & 0xFF, addr & 0xFF, 0);
}

static char flash_read_page(int length)
{
	for ( int x = 0; x < length; x += 2 )
	{
		uint8_t low = flash_read(LOW, avrpdata.here);
		Serial.print((char) low);

		uint8_t high = flash_read(HIGH, avrpdata.here);
		Serial.print((char) high);

		avrpdata.here++;
	}
	return STK_OK;
}

static char eeprom_read_page(int length)
{
	// here again we have a word address
	int start = avrpdata.here * 2;

	for ( int x = 0; x < length; x++ )
	{
		int addr = start + x;
		uint8_t ee = spi_transaction(0xA0, (addr >> 8) & 0xFF, addr & 0xFF, 0xFF);
		Serial.print((char) ee);
	}
	return STK_OK;
}

static void read_page(void)
{
	char result = (char)STK_FAILED;
	int length = 256 * getch();
	length += getch();
	char memtype = getch();

	if ( getch() == CRC_EOP )
	{
		Serial.print((char) STK_INSYNC);
		if (memtype == 'F')
			result = flash_read_page(length);
		else if (memtype == 'E')
			result = eeprom_read_page(length);
		Serial.print(result);
	}
	else
	{
		avrpdata.error++;
		Serial.print((char) STK_NOSYNC);
		return;
	}
}

static void read_signature(void)
{
	if ( getch() == CRC_EOP )
	{
		Serial.print((char) STK_INSYNC);

		uint8_t high = spi_transaction(0x30, 0x00, 0x00, 0x00);
		Serial.print((char) high);

		uint8_t middle = spi_transaction(0x30, 0x00, 0x01, 0x00);
		Serial.print((char) middle);

		uint8_t low = spi_transaction(0x30, 0x00, 0x02, 0x00);
		Serial.print((char) low);

		Serial.print((char) STK_OK);
	}
	else
	{
		avrpdata.error++;
		Serial.print((char) STK_NOSYNC);
	}
}

static void avrisp(void)
{
	uint8_t ch = getch();

	switch (ch)
	{
	case '0':		// sign-on
		avrpdata.error = 0;
		empty_reply();
		break;

	case '1':
		if ( getch() == CRC_EOP )
		{
			Serial.print((char)STK_INSYNC);
			Serial.print("AVR ISP");
			Serial.print((char)STK_OK);
		}
		else
		{
			avrpdata.error++;
			Serial.print((char) STK_NOSYNC);
		}
		break;

	case 'A':
		get_version(getch());
		break;

	case 'B':
		fill(20);
		set_parameters();
		empty_reply();
		break;

	case 'E': // extended parameters - ignore for now
		fill(5);
		empty_reply();
		break;

	case 'P':
		if (!avrpdata.pmode)
			start_pmode();
		empty_reply();
		break;

	case 'U': // set address (word)
		avrpdata.here = getch();
		avrpdata.here += 256 * getch();
		empty_reply();
		break;

	case 0x60: //STK_PROG_FLASH
		getch(); // low addr
		getch(); // high addr
		empty_reply();
		break;

	case 0x61: //STK_PROG_DATA
		getch(); // data
		empty_reply();
		break;

	case 0x64: //STK_PROG_PAGE
		program_page();
		break;

	case 0x74: //STK_READ_PAGE 't'
		read_page();
		break;

	case 'V': //0x56
		universal();
		break;

	case 'Q': //0x51
		avrpdata.error = 0;
		end_pmode();
		empty_reply();
		break;

	case 0x75: //STK_READ_SIGN 'u'
		read_signature();
		break;

	// expecting a command, not CRC_EOP
	// this is how we can get back in sync
	case CRC_EOP:
		avrpdata.error++;
		Serial.print((char)STK_NOSYNC);
		break;

	// anything else we will return STK_UNKNOWN
	default:
		avrpdata.error++;
		if ( getch() == CRC_EOP )
			Serial.print((char)STK_UNKNOWN);
		else
			Serial.print((char)STK_NOSYNC);
		break;
	}
}

static void avrp_init(void)
{
	Serial.begin(BAUDRATE);
}
