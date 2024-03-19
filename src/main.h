/*
 * main.h
 *
 *  Created on: 03-Mar-2021
 *      Author: Kumar Harsh
 */

#ifndef MAIN_H_
#define MAIN_H_

/**
 * Include files
 */
#include "stm32f10x.h"
#include "lcd.h"
#include "ADC_Driver.h"
#include "external.h"

/**
 * User-defined Variables
 */


/**
 * User-defined Function Declarations
 */
void systemClockInit(void);
void gpioInit(void);
void Delay(long int);
void msDelay(int);

#endif /* MAIN_H_ */
