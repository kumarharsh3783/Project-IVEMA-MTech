/*
 * error.c
 *
 *  Created on: 04-Mar-2021
 *      Author: Kumar Harsh
 */

/*******************
 * Include Files
 *******************/
#include "error.h"

void errorMsg()
{
	clearLcd();
	lcdCursorSet(0,0);
	lcd_send_string("Network Failure ");
	lcdCursorSet(1,0);					/* Change cursor to line 1 */
	lcd_send_string("GPRS Error...   ");
	delay_in_ms(1500);
}

void errorMsg1()
{
	clearLcd();
	lcdCursorSet(0,0);
	lcd_send_string("Error connecting");
	lcdCursorSet(1,0);					/* Change cursor to line 1 */
	lcd_send_string("   GSM Modem !  ");
	delay_in_ms(1500);
}

