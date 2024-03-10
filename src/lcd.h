/*
 * lcd.h
 *
 *  Created on: 03-Mar-2021
 *      Author: Kumar Harsh
 */

#ifndef LCD_H_
#define LCD_H_

/**
 * Include files
 */
#include "stm32f10x.h"

/**
 * User-defined Variables
 */
int lcdIndex;
uint16_t lcdData;

/**
 * User-defined Function Declarations
 */
void lcdInit(void);
void clearLcd(void);
void sendLcdCmd(uint8_t);
void sendLcdData(uint8_t);
void lcd_send_string(char*);
void lcdCmdWrite(uint16_t);
void lcdDataWrite(uint16_t);
void loadLcdReg(uint16_t);
void lcdCursorSet(int,int);
void lcdtimerInit(void);
void lcd_msDelay(int);
void lcd_usDelay(int);

#endif
