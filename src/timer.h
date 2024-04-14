/*
 * timer.h
 *
 *  Created on: 04-Mar-2021
 *      Author: Kumar Harsh
 */

#ifndef TIMER_H_
#define TIMER_H_

/**
 * Include Files
 */
#include "stm32f10x.h"
//#include "serverApi.h"
#include "external.h"
//#include "eeprom.h"

/**
 * Macro and typedef Definitions
 */
#define DELAY_BASE			1000
#define	TIMEOUT_DETECTION	100  	/* 100 * 100ms = 10,000ms = 10s */

/**
 * User_Defined Variable Declarations
 */

/**
 * User-defined Function Declarations
 */
void timerInit(void);
void timer2ON(void);
void timer2OFF(void);
void timer4ON(void);
void timer4OFF(void);

void delay_in_sec(unsigned int);
void delay_in_ms(unsigned int);

#endif /* TIMER_H_ */
