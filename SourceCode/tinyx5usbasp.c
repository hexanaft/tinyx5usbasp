/*
 * USBasp - USB in-circuit programmer for Atmel AVR controllers
 *
 * Thomas Fischl <tfischl@gmx.de>
 *
 * License........: GNU GPL v2 (see Readme.txt)
 * Target.........: ATMega8 at 12 MHz
 * Creation Date..: 2005-02-20
 * Last change....: 2009-02-28
 *
 * PC2 SCK speed option.
 * GND  -> slow (8khz SCK),
 * open -> software set speed (default is 375kHz SCK)
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <compat/deprecated.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#include "usbasp.h"
#include "usbdrv.h"
#include "isp.h"
#include "osccal.h"

//#define SLOW_SCK		PD7
//#define DDR_SLOW_SCK	DDRD
//#define PORT_SLOW_SCK	PORTD
//#define PIN_SLOW_SCK	PIND

static uint8_t replyBuffer[8];

static uint8_t prog_state = PROG_STATE_IDLE;
static uint8_t prog_sck = USBASP_ISP_SCK_AUTO;

static uint8_t prog_address_newmode = 0;
static uint32_t prog_address;
static uint16_t prog_nbytes = 0;
static uint16_t prog_pagesize;
static uint8_t prog_blockflags;
static uint8_t prog_pagecounter;


uint8_t usbFunctionSetup( uint8_t data[8] )
{
	uint8_t len = 0;
	
	if( data[1] == USBASP_FUNC_CONNECT )
	{
		/* set SCK speed */
/*		if(( PIN_SLOW_SCK & ( 1 << SLOW_SCK ) ) == 0 )
		{
			ispSetSCKOption( USBASP_ISP_SCK_8 );
		}
		else
		{
			ispSetSCKOption( prog_sck );
		}
		 */
		ispSetSCKOption( USBASP_ISP_SCK_32 );
		 
		/* set compatibility mode of address delivering */
		prog_address_newmode = 0;
//		ledRedOn();
		ispConnect();
	}
	else if( data[1] == USBASP_FUNC_DISCONNECT )
	{
		ispDisconnect();
//		ledRedOff();
	}
	else if( data[1] == USBASP_FUNC_TRANSMIT )
	{
		replyBuffer[0] = ispTransmit( data[2] );
		replyBuffer[1] = ispTransmit( data[3] );
		replyBuffer[2] = ispTransmit( data[4] );
		replyBuffer[3] = ispTransmit( data[5] );
		len = 4;
	}
	else if( data[1] == USBASP_FUNC_READFLASH )
	{
		if( !prog_address_newmode )
			prog_address = ( data[3] << 8 ) | data[2];
			
		prog_nbytes = ( data[7] << 8 ) | data[6];
		prog_state = PROG_STATE_READFLASH;
		len = 0xff; /* multiple in */
	}
	else if( data[1] == USBASP_FUNC_READEEPROM )
	{
		if( !prog_address_newmode )
			prog_address = ( data[3] << 8 ) | data[2];
			
		prog_nbytes = ( data[7] << 8 ) | data[6];
		prog_state = PROG_STATE_READEEPROM;
		len = 0xff; /* multiple in */
	}
	else if( data[1] == USBASP_FUNC_ENABLEPROG )
	{
		replyBuffer[0] = ispEnterProgrammingMode();
		len = 1;
	}
	else if( data[1] == USBASP_FUNC_WRITEFLASH )
	{
		if( !prog_address_newmode )
			prog_address = ( data[3] << 8 ) | data[2];
			
		prog_pagesize = data[4];
		prog_blockflags = data[5] & 0x0F;
		prog_pagesize += ((( uint16_t ) data[5] & 0xF0 ) << 4 );
		if( prog_blockflags & PROG_BLOCKFLAG_FIRST )
		{
			prog_pagecounter = prog_pagesize;
		}
		
		prog_nbytes = ( data[7] << 8 ) | data[6];
		prog_state = PROG_STATE_WRITEFLASH;
		len = 0xff; /* multiple out */
	}
	else if( data[1] == USBASP_FUNC_WRITEEEPROM )
	{
		if( !prog_address_newmode )
			prog_address = ( data[3] << 8 ) | data[2];
			
		prog_pagesize = 0;
		prog_blockflags = 0;
		prog_nbytes = ( data[7] << 8 ) | data[6];
		prog_state = PROG_STATE_WRITEEEPROM;
		len = 0xff; /* multiple out */
	}
	else if( data[1] == USBASP_FUNC_SETLONGADDRESS )
	{
		/* set new mode of address delivering (ignore address delivered in commands) */
		prog_address_newmode = 1;
		/* set new address */
		prog_address = *(( uint32_t* ) &data[2] );
	}
	else if( data[1] == USBASP_FUNC_SETISPSCK )
	{
		/* set sck option */
		prog_sck = data[2];
		replyBuffer[0] = 0;
		len = 1;
	}
	
	usbMsgPtr = replyBuffer;
	
	return len;
}

uint8_t usbFunctionRead( uint8_t *data, uint8_t len )
{
	uint8_t i;
	
	/* check if programmer is in correct read state */
	if(( prog_state != PROG_STATE_READFLASH ) &&
	        ( prog_state != PROG_STATE_READEEPROM ) )
	{
		return 0xff;
	}
	
	/* fill packet */
	for( i = 0; i < len; i++ )
	{
		if( prog_state == PROG_STATE_READFLASH )
		{
			data[i] = ispReadFlash( prog_address );
		}
		else
		{
			data[i] = ispReadEEPROM( prog_address );
		}
		prog_address++;
	}
	
	/* last packet? */
	if( len < 8 )
	{
		prog_state = PROG_STATE_IDLE;
	}
	
	return len;
}

uint8_t usbFunctionWrite( uint8_t *data, uint8_t len )
{

	uint8_t retVal = 0;
	uint8_t i;
	
	/* check if programmer is in correct write state */
	if(( prog_state != PROG_STATE_WRITEFLASH ) &&
	        ( prog_state != PROG_STATE_WRITEEEPROM ) )
	{
		return 0xff;
	}
	
	for( i = 0; i < len; i++ )
	{
		if( prog_state == PROG_STATE_WRITEFLASH )
		{
			/* Flash */
			if( prog_pagesize == 0 )
			{
				/* not paged */
				ispWriteFlash( prog_address, data[i], 1 );
			}
			else
			{
				/* paged */
				ispWriteFlash( prog_address, data[i], 0 );
				prog_pagecounter--;
				if( prog_pagecounter == 0 )
				{
					ispFlushPage( prog_address, data[i] );
					prog_pagecounter = prog_pagesize;
				}
			}
			
		}
		else
		{
			/* EEPROM */
			ispWriteEEPROM( prog_address, data[i] );
		}
		
		prog_nbytes--;
		
		if( prog_nbytes == 0 )
		{
			prog_state = PROG_STATE_IDLE;
			if(( prog_blockflags & PROG_BLOCKFLAG_LAST ) &&
			        ( prog_pagecounter != prog_pagesize ) )
			{
				/* last block and page flush pending, so flush it now */
				ispFlushPage( prog_address, data[i] );
			}
			
			retVal = 1; // Need to return 1 when no more data is to be received
		}
		
		prog_address++;
	}
	
	return retVal;
}


int main( void )
{
	/* set all ISP pins inputs */
	ISP_DDR &= ~(( 1 << ISP_RST ) | ( 1 << ISP_SCK ) | ( 1 << ISP_MOSI ) | ( 1 << ISP_MISO ) );
	/* switch pullups off */
	ISP_OUT &= ~(( 1 << ISP_RST ) | ( 1 << ISP_SCK ) | ( 1 << ISP_MOSI ) | ( 1 << ISP_MISO ) );
	
	//===========================================
	uint8_t calibrationValue;
    calibrationValue = eeprom_read_byte(0); /* calibration value from last time */
    if(calibrationValue != 0xff)
    {
        OSCCAL = calibrationValue;
    }
	//===========================================
	
	usbInit();
	//===========================================
	usbDeviceDisconnect();
	_delay_ms( 300 );			/* 300 ms disconnect */
	usbDeviceConnect();
	//===========================================
	sei();
	/* main event loop */
	for( ;; )
	{
		usbPoll();
	}
//	return 0;
}
