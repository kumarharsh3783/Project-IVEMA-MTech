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
uint16_t dmaRcvBuf[2];
uint16_t adcVal_mq135;
uint16_t adcVal_mq7;

/**
 * User-defined Function Declarations
 */
void systemClockInit(void);
void gpioInit(void);
void dmaInit(uint32_t *, uint32_t *, unsigned int);
void Delay(long int);
void msDelay(int);

#endif /* MAIN_H_ */
