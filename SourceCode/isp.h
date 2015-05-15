/*
 * isp.h - part of USBasp
 *
 * Autor..........: Thomas Fischl <tfischl@gmx.de>
 * Description....: Provides functions for communication/programming
 *                  over ISP interface
 * Licence........: GNU GPL v2 (see Readme.txt)
 * Creation Date..: 2005-02-23
 * Last change....: 2009-02-28
 */

#ifndef __isp_h_included__
#define	__isp_h_included__

#define	ISP_OUT   PORTB
#define ISP_IN    PINB
#define ISP_DDR   DDRB
#define ISP_RST   PB5
#define ISP_MOSI  PB0
#define ISP_MISO  PB1
#define ISP_SCK   PB2

#ifndef		_BV
	#define _BV(bit)	(1 << (bit))
#endif

#ifndef		sbi
	#define sbi(data, bit)	(data) |= _BV(bit)
#endif

#ifndef		cbi
	#define cbi(data, bit)	(data) &= ~_BV(bit)
#endif

/* 1 circle 320us */
void clockWait(uint8_t time);

/* Prepare connection to target device */
void ispConnect();

/* Close connection to target device */
void ispDisconnect();

/* read an write a byte from isp using software (slow) */
uint8_t ispTransmit_sw(uint8_t send_byte);

/* read an write a byte from isp using software (fast) */
uint8_t ispTransmit_sw_max(uint8_t send_byte);

/* read an write a byte from isp using hardware (fast) */
//uint8_t ispTransmit_hw(uint8_t send_byte);

/* enter programming mode */
uint8_t ispEnterProgrammingMode();

/* read byte from eeprom at given address */
uint8_t ispReadEEPROM(uint16_t address);

/* write byte to flash at given address */
uint8_t ispWriteFlash(uint32_t address, uint8_t data, uint8_t pollmode);

uint8_t ispFlushPage(uint32_t address, uint8_t pollvalue);

/* read byte from flash at given address */
uint8_t ispReadFlash(uint32_t address);

/* write byte to eeprom at given address */
uint8_t ispWriteEEPROM(uint16_t address, uint8_t data);

/* pointer to sw or hw transmit function */
uint8_t (*ispTransmit)(uint8_t);

/* set SCK speed. call before ispConnect! */
void ispSetSCKOption(uint8_t sckoption);


#endif /* __isp_h_included__ */
