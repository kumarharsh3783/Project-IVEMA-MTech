/*
 * uart.h
 *
 *  Created on: 03-Mar-2021
 *      Author: Kumar Harsh
 */

#ifndef UART_H_
#define UART_H_

/**
 * Include Files
 */
#include "stm32f10x.h"
#include "external.h"
#include "serverApi.h"
#include "timer.h"
//#include "lcd.h"
#include "string.h"

/**
 * Macro and typedef Definitions
 */
/*<--- Different standard Baud-Rates --->*/
#define BAUD_9600				9600
#define BAUD_19200				19200
#define BAUD_38400				38400
#define BAUD_57600				57600
#define BAUD_115200				115200

/**
 * User-defined Variables
 */
USART_InitTypeDef usart;
int txIndex;


/**
 * User-defined Function Declarations
 */
void uartInit(void);
void changeBaud(uint32_t);
void uartSendData(char*);
void uartReceiveData(char*,int);

#endif	/* UART_H_ */
