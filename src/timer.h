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
#include "serverApi.h"
#include "external.h"
#include "eeprom.h"

/**
 * Macro and typedef Definitions
 */
#define DELAY_BASE		1000
#define DELAY_Timeout	(45 * DELAY_BASE)		/* Timeout Period : 45 seconds */
#define DELAY_EepromEvent 45000					/* in milliseconds */

/**
 * User-defined Function Declarations
 */
void timerInit(void);
void timerON(void);
void timerOFF(void);

#endif /* TIMER_H_ */
