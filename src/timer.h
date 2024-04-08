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
#define DELAY_BASE		1000

/**
 * User-defined Function Declarations
 */
void timerInit(void);

void timer4ON(void);
void timer4OFF(void);
void timer2ON(void);
void timer2OFF(void);

void delay_in_sec(unsigned int);
void delay_in_ms(unsigned int);

#endif /* TIMER_H_ */
