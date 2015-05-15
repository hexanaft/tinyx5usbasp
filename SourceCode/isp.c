/*
 * isp.c - part of USBasp
 *
 * Autor..........: Thomas Fischl <tfischl@gmx.de>
 * Description....: Provides functions for communication/programming
 *                  over ISP interface
 * Licence........: GNU GPL v2 (see Readme.txt)
 * Creation Date..: 2005-02-23
 * Last change....: 2009-02-28
 */

#include <avr/io.h>
#include <util/delay.h>
#include "isp.h"
#include "usbasp.h"

//#define spiHWdisable() SPCR = 0

uint8_t sck_sw_delay;
//uint8_t sck_spcr;
//uint8_t sck_spsr;

void clockWait( uint8_t time )
{
	uint8_t i;
	for( i = 0; i < time; i++ )
	{
		_delay_us( 320 );
	}
}

/*
void spiHWenable()
{
	SPCR = sck_spcr;
	SPSR = sck_spsr;
}
*/

void ispSetSCKOption( uint8_t option )
{

	if( option == USBASP_ISP_SCK_AUTO )
		option = USBASP_ISP_SCK_375;
		
	if( option >= USBASP_ISP_SCK_93_75 )
	{
		ispTransmit = ispTransmit_sw_max;
		//sck_sw_delay = 1;
/*		ispTransmit = ispTransmit_hw;
		sck_spsr = 0;
		switch( option )
		{
			case USBASP_ISP_SCK_1500:
			{
				// enable SPI, master, 1.5MHz, XTAL/8 // 2MHz if F_CPU=16MHz
				sck_spcr = ( 1 << SPE ) | ( 1 << MSTR ) | ( 1 << SPR0 );
				sck_spsr = ( 1 << SPI2X );
			} break;
			case USBASP_ISP_SCK_750:
			{
				// enable SPI, master, 750kHz, XTAL/16 // 1MHz if F_CPU=16MHz
				sck_spcr = ( 1 << SPE ) | ( 1 << MSTR ) | ( 1 << SPR0 );
			} break;
			case USBASP_ISP_SCK_375:
			default:
			{
				// enable SPI, master, 375kHz, XTAL/32 (default) // 500KHz if F_CPU=16MHz
				sck_spcr = ( 1 << SPE ) | ( 1 << MSTR ) | ( 1 << SPR1 );
				sck_spsr = ( 1 << SPI2X );
			} break;
			case USBASP_ISP_SCK_187_5:
			{
				// enable SPI, master, 187.5kHz XTAL/64 // 250KHz if F_CPU=16MHz
				sck_spcr = ( 1 << SPE ) | ( 1 << MSTR ) | ( 1 << SPR1 );
			} break;
			case USBASP_ISP_SCK_93_75:
			{
				// enable SPI, master, 93.75kHz XTAL/128 // 125KHz if F_CPU=16MHz
				sck_spcr = ( 1 << SPE ) | ( 1 << MSTR ) | ( 1 << SPR1 ) | ( 1 << SPR0 );
			} break;
		}
		 */
	}
	else
	{
		ispTransmit = ispTransmit_sw;
		switch( option )
		{
			case USBASP_ISP_SCK_32:
			{
				sck_sw_delay = 1;
			} break;
			case USBASP_ISP_SCK_16:
			{
				sck_sw_delay = 2;
			} break;
			case USBASP_ISP_SCK_8:
			{
				sck_sw_delay = 4;
			} break;
			case USBASP_ISP_SCK_4:
			{
				sck_sw_delay = 8;
			} break;
			case USBASP_ISP_SCK_2:
			{
				sck_sw_delay = 16;
			} break;
			case USBASP_ISP_SCK_1:
			{
				sck_sw_delay = 32;
			} break;
			case USBASP_ISP_SCK_0_5:
			{
				sck_sw_delay = 64;
			} break;
		}
	}
	/*
		if (option >= USBASP_ISP_SCK_93_75) {
			ispTransmit = ispTransmit_hw;
			sck_spsr = 0;
	
			switch (option) {
	
			case USBASP_ISP_SCK_1500:
				// enable SPI, master, 1.5MHz, XTAL/8 //
				sck_spcr = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
				sck_spsr = (1 << SPI2X);
				break;
			case USBASP_ISP_SCK_750:
				// enable SPI, master, 750kHz, XTAL/16 //
				sck_spcr = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
				break;
			case USBASP_ISP_SCK_375:
			default:
				// enable SPI, master, 375kHz, XTAL/32 (default) //
				sck_spcr = (1 << SPE) | (1 << MSTR) | (1 << SPR1);
				sck_spsr = (1 << SPI2X);
				break;
			case USBASP_ISP_SCK_187_5:
				// enable SPI, master, 187.5kHz XTAL/64 //
				sck_spcr = (1 << SPE) | (1 << MSTR) | (1 << SPR1);
				break;
			case USBASP_ISP_SCK_93_75:
				// enable SPI, master, 93.75kHz XTAL/128 //
				sck_spcr = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);
				break;
			}
	
		} else {
			ispTransmit = ispTransmit_sw;
			switch (option) {
	
			case USBASP_ISP_SCK_32:
				sck_sw_delay = 3;
				break;
			case USBASP_ISP_SCK_16:
				sck_sw_delay = 6;
				break;
			case USBASP_ISP_SCK_8:
				sck_sw_delay = 12;
				break;
			case USBASP_ISP_SCK_4:
				sck_sw_delay = 24;
				break;
			case USBASP_ISP_SCK_2:
				sck_sw_delay = 48;
				break;
			case USBASP_ISP_SCK_1:
				sck_sw_delay = 96;
				break;
			case USBASP_ISP_SCK_0_5:
				sck_sw_delay = 192;
				break;
			}
		}
	*/
}

void ispDelay()
{
	uint8_t i;
	for( i = 0; i < sck_sw_delay; ++i )_delay_us( 15.625 );
}

void ispConnect()
{

	/* all ISP pins are inputs before */
	/* now set output pins */
	ISP_DDR |= ( 1 << ISP_RST ) | ( 1 << ISP_SCK ) | ( 1 << ISP_MOSI );
	
	/* reset device */
	ISP_OUT &= ~( 1 << ISP_RST ); /* RST low */
	ISP_OUT &= ~( 1 << ISP_SCK ); /* SCK low */
	
	/* positive reset pulse > 2 SCK (target) */
	ispDelay();
	ISP_OUT |= ( 1 << ISP_RST ); /* RST high */
	ispDelay();
	ISP_OUT &= ~( 1 << ISP_RST ); /* RST low */
/*	
	if( ispTransmit == ispTransmit_hw )
	{
		spiHWenable();
	}
	 */
}

void ispDisconnect()
{
	/* set all ISP pins inputs */
	ISP_DDR &= ~(( 1 << ISP_RST ) | ( 1 << ISP_SCK ) | ( 1 << ISP_MOSI ) );
	/* switch pullups off */
	ISP_OUT &= ~(( 1 << ISP_RST ) | ( 1 << ISP_SCK ) | ( 1 << ISP_MOSI ) );
	
	/* disable hardware SPI */
//	spiHWdisable();
}
/*
uint8_t ispTransmit_sw( uint8_t send_byte )
{
	uint8_t rec_byte = 0;
	uint8_t i;
	for( i = 0; i < 8; i++ )
	{
	
		//set MSB to MOSI-pin
//		if(( send_byte & 0x80 ) != 0 )
//		{
//			ISP_OUT |= ( 1 << ISP_MOSI ); // MOSI high //
//		}
//		else
//		{
//			ISP_OUT &= ~( 1 << ISP_MOSI ); // MOSI low //
//		}

		if( send_byte & 0x80 )	sbi( ISP_OUT, ISP_MOSI );	// MOSI high //
		else 					cbi( ISP_OUT, ISP_MOSI );	// MOSI low //
		
		// shift to next bit //
		send_byte = send_byte << 1;
		
		// receive data //
		rec_byte = rec_byte << 1;
//		if(( ISP_IN & ( 1 << ISP_MISO ) ) != 0 )
//		{
//			rec_byte++;
//		}

		if( bit_is_set( ISP_IN, ISP_MISO ) )rec_byte++;
		
		// pulse SCK //
		//ISP_OUT |= ( 1 << ISP_SCK );	// SCK high //
		sbi( ISP_OUT, ISP_SCK );		// SCK high //
		ispDelay();
		//ISP_OUT &= ~( 1 << ISP_SCK ); // SCK low //
		cbi( ISP_OUT, ISP_SCK ); 		// SCK low //
		ispDelay();
	}
	
	return rec_byte;
}
*/
uint8_t ispTransmit_sw( uint8_t send_byte )
{
	uint8_t rec_byte = 0;
	uint8_t i;
	for( i = 0; i < 8; i++ )
	{
		if( send_byte & 0x80 )	sbi( ISP_OUT, ISP_MOSI );	// MOSI high //
		else 					cbi( ISP_OUT, ISP_MOSI );	// MOSI low //
		
		// shift to next bit //
		send_byte = send_byte << 1;
		
		// receive data //
		rec_byte = rec_byte << 1;
		if( bit_is_set( ISP_IN, ISP_MISO ) )rec_byte++;
		
		// pulse SCK //
		sbi( ISP_OUT, ISP_SCK );		// SCK high //
		ispDelay();
		cbi( ISP_OUT, ISP_SCK ); 		// SCK low //
		ispDelay();
	}
	
	return rec_byte;
}

// при 16.5 МГц скорость порядка 300кбайт/с
uint8_t ispTransmit_sw_max( uint8_t send_byte )
{
	uint8_t rec_byte = 0;
	uint8_t i;
	for( i = 0; i < 8; i++ )
	{
		if( send_byte & 0x80 )	sbi( ISP_OUT, ISP_MOSI );	// MOSI high //
		else 					cbi( ISP_OUT, ISP_MOSI );	// MOSI low //
		
		// shift to next bit //
		send_byte = send_byte << 1;
		
		// receive data //
		rec_byte = rec_byte << 1;
		if( bit_is_set( ISP_IN, ISP_MISO ) )rec_byte++;
		
		// pulse SCK //
		sbi( ISP_OUT, ISP_SCK );		// SCK high //
		_delay_us(1);
		cbi( ISP_OUT, ISP_SCK ); 		// SCK low //
	}
	
	return rec_byte;
}

/*
uint8_t ispTransmit_hw( uint8_t send_byte )
{
	SPDR = send_byte;
	
	while( !( SPSR & ( 1 << SPIF ) ) )
		;
	return SPDR;
}
*/

uint8_t ispEnterProgrammingMode()
{
	uint8_t check;
	uint8_t count = 32;
	
	while( count-- )
	{
		ispTransmit( 0xAC );
		ispTransmit( 0x53 );
		check = ispTransmit( 0 );
		ispTransmit( 0 );
		
		if( check == 0x53 )
		{
			return 0;
		}
		
//		spiHWdisable();
		
		/* pulse SCK */
		ISP_OUT |= ( 1 << ISP_SCK ); /* SCK high */
		ispDelay();
		ISP_OUT &= ~( 1 << ISP_SCK ); /* SCK low */
		ispDelay();
/*		
		if( ispTransmit == ispTransmit_hw )
		{
			spiHWenable();
		}
*/		
	}
	
	return 1; /* error: device dosn't answer */
}

uint8_t ispReadFlash( uint32_t address )
{
	ispTransmit( 0x20 | (( address & 1 ) << 3 ) );
	ispTransmit( address >> 9 );
	ispTransmit( address >> 1 );
	return ispTransmit( 0 );
}

uint8_t ispWriteFlash( uint32_t address, uint8_t data, uint8_t pollmode )
{

	/* 0xFF is value after chip erase, so skip programming
	 if (data == 0xFF) {
	 return 0;
	 }
	 */
	
	ispTransmit( 0x40 | (( address & 1 ) << 3 ) );
	ispTransmit( address >> 9 );
	ispTransmit( address >> 1 );
	ispTransmit( data );
	
	if( pollmode == 0 )
		return 0;
		
	if( data == 0x7F )
	{
		clockWait( 15 ); /* wait 4,8 ms */
		return 0;
	}
	else
	{
	
		/* polling flash */
		uint8_t retries = 30;
		//uint8_t starttime = TIMERVALUE;
		while( retries != 0 )
		{
			if( ispReadFlash( address ) != 0x7F )
			{
				return 0;
			}
			clockWait( 1 );
			/*
						if ((uint8_t) (TIMERVALUE - starttime) > CLOCK_T_320us) {
							starttime = TIMERVALUE;
							retries--;
						}
			*/
		}
		return 1; /* error */
	}
	
}

uint8_t ispFlushPage( uint32_t address, uint8_t pollvalue )
{
	ispTransmit( 0x4C );
	ispTransmit( address >> 9 );
	ispTransmit( address >> 1 );
	ispTransmit( 0 );
	
	if( pollvalue == 0xFF )
	{
		clockWait( 15 );
		return 0;
	}
	else
	{
		/* polling flash */
		uint8_t retries = 30;
		
		while( retries != 0 )
		{
			if( ispReadFlash( address ) != 0xFF )
			{
				return 0;
			}
			clockWait( 1 );
		}
		
		return 1; /* error */
	}
	
}

uint8_t ispReadEEPROM( uint16_t address )
{
	ispTransmit( 0xA0 );
	ispTransmit( address >> 8 );
	ispTransmit( address );
	return ispTransmit( 0 );
}

uint8_t ispWriteEEPROM( uint16_t address, uint8_t data )
{
	ispTransmit( 0xC0 );
	ispTransmit( address >> 8 );
	ispTransmit( address );
	ispTransmit( data );
	
	clockWait( 30 ); // wait 9,6 ms
	
	return 0;
}
