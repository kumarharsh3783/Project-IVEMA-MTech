/*
 * lcd.c
 *
 *  Created on: 03-Mar-2021
 *      Author: Kumar Harsh
 */

/*<-- Includes -->*/
#include "lcd.h"

/**
 * Brief:	Initialize 1602 LCD.
 * 				GPIO PINS				LCD PINS
 * 				---------				--------
 * 			[PB15,PB14,PB13,PB12] <=> [DB7,DB6,DB5,DB4]
 * 				[PA0,GND,PA1]	  <=> [RS,RW,EN]
 * Param:	none
 * Return:	none
 */
void lcdInit()
{
	lcdtimerInit();
	lcd_msDelay(150);						/* wait for LCD Power ON time > 40ms */
	uint8_t fourBitLcdCmd[6] = {0x02,0x28,0x0c,0x06,0x01,0x80};

	for(lcdIndex = 0;lcdIndex<6;lcdIndex++)
	{
		sendLcdCmd(fourBitLcdCmd[lcdIndex]);
		lcd_usDelay(500);
	}
}

/**
 * Brief:	Set the row and col for the 1602 LCD
 */
void lcdCursorSet(int row, int col)
{
	switch (row)
	{
		case 0:
			col |= 0x80;
			break;
		case 1:
			col |= 0xC0;
			break;
		case 2:
			col += 0x94;
			break;
		case 3:
			col += 0xD4;
			break;
	}
	sendLcdCmd(col);						/* Send command to set the cursor position */
	lcd_usDelay(500);
}

/**
 * Brief:	Clear display and set the cursor at the beginning.
 */
void clearLcd(void)
{
	sendLcdCmd(0x01);						/* Clear display */
	lcd_usDelay(500);
	sendLcdCmd(0x80);						/* set cursor at pos. (0,0) */
	lcd_usDelay(500);
}

/**
 * Brief:	send string to lcd
 */
void lcd_send_string (char *str)
{
	while(*str)
	{
		sendLcdData(*str++);
		lcd_usDelay(500);
	}
}

/**
 * Brief:	Transmit LCD command directly to LCD.
 * Param:	(uint8_t)command - 8 bit LCD command to be sent
 * Return:	none
 */
void sendLcdCmd(uint8_t command)
{
	lcdData = (command & 0xf0);			/* Separating 8 bit command into nibbles to send to lcd 4 bit */
	lcdData = lcdData>>4;
	lcdCmdWrite((uint16_t)lcdData);		/* sending higher nibble (MSBs) */
	//lcd_msDelay(20);					/* Delay provided for lcd processing */
	lcdData = (command & 0x0f);
	lcdCmdWrite((uint16_t)lcdData);		/* sending lower nibble (LSBs) */
//	lcd_msDelay(10);					/* Delay provided for lcd processing */
}

/**
 * Brief:	Transmit LCD data directly to LCD.
 * Param:	(uint8_t)dat - 8 bit LCD data to be sent
 * Return:	none
 */
void sendLcdData(uint8_t dat)
{
	lcdData = (dat & 0xf0);				/* Separating 8 bit data into nibbles to send to lcd 4 bit */
	lcdData = lcdData>>4;
	lcdDataWrite((uint16_t)lcdData);	/* sending higher nibble (MSBs) */
	//lcd_msDelay(20);						/* Delay provided for lcd processing */
	lcdData = (dat & 0x0f);
	lcdDataWrite((uint16_t)lcdData);	/* sending lower nibble (LSBs) */
//	lcd_msDelay(10);						/* Delay provided for lcd processing */
}

/**
 * Brief: 	To load LCD data lines with LCD command
 * Param: 	(uint16_t)cmd - contains command (in nibbles) to be sent into the 4 lines
 * Return:	none
 */
void lcdCmdWrite(uint16_t cmd)
{
	loadLcdReg(cmd);					/* Load into LCD Data Register */

	GPIOA->BSRR	|= GPIO_BSRR_BR0;		/* RS = 0 */
//	GPIOA->BSRR	|= GPIO_BSRR_BR4;		/* RW = 0 */
	GPIOA->BSRR	|= GPIO_BSRR_BS1;		/* EN = 1 */
	lcd_msDelay(1);
	GPIOA->BSRR |= GPIO_BSRR_BR1;		/* EN = 0 */
	lcd_msDelay(1);
}

/**
 * Brief: 	To load LCD data lines with LCD data
 * Param: 	(uint16_t)data - contains data (in nibbles) to be sent into the 4 lines
 * Return:	none
 */
void lcdDataWrite(uint16_t data)
{
	loadLcdReg(data);					/* Load into LCD Data Register */

	GPIOA->BSRR	|= GPIO_BSRR_BS0;		/* RS = 1 */
//	GPIOA->BSRR	|= GPIO_BSRR_BR4;		/* RW = 0 */
	GPIOA->BSRR	|= GPIO_BSRR_BS1;		/* EN = 1 */
	lcd_msDelay(1);
	GPIOA->BSRR |= GPIO_BSRR_BR1;		/* EN = 0 */
	lcd_msDelay(1);
}

/**
 * Brief:	This method will load the lcd data without changing other port lines
 * Param:	(uint26_t) lcd - will contain the nibble
 * Return:	none
 */
void loadLcdReg(uint16_t lcd)
{
	GPIOB->ODR &= 0x0fff;				/* Mask 12 LSB bits */
	lcd = (lcd<<12);					/* left shift data to MSB */
	GPIOB->ODR |= lcd;					/* Load the lcd data into LCD data lines */
}

/**
 * Brief:	Configure the lcd delay timer TIM3
 */
void lcdtimerInit()
{
	/* Clock to TIM3 Peripheral */
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	/* Configuring Clock for 1MHz i.e. 72MHz/72 = 1MHz; T = 1us*/
	TIM3->ARR = 65535;
	TIM3->PSC = 72-1;

	/* Enable the timer 3 and wait till the UIF flag */
	TIM3->CR1 |= TIM_CR1_CEN;
	while(!(TIM3->SR & TIM_SR_UIF));
}

/**
 * Brief:	LCD microseconds Delay function
 */
void lcd_usDelay(int uscount)
{
		/* Reset the CNT register to 0 */
		TIM3->CNT = 0;

		/* Wait till 1ms Counter */
		while(TIM3->CNT < uscount);
}

/**
 * Brief:	LCD milliseconds Delay function
 */
void lcd_msDelay(int mscount)
{
	while(mscount != 0)
	{
		/* delay of 1000 us = 1 ms */
		lcd_usDelay(1000);
		mscount--;
	}
}
