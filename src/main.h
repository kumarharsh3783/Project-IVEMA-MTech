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
#include "timer.h"

/**
 * User-defined Variables
 */

typedef enum
{
	sysclk_72MHz,
	sysclk_56MHz
}sysclk_MHz;

uint16_t dmaRcvBuf[3];
uint16_t adcVal_mq135;
uint16_t adcVal_mq7;
float TemperatureValue;

/**
 * User-defined Function Declarations
 */
void systemClockInit(sysclk_MHz);
void gpioInit(void);
void dmaInit(uint32_t *, uint32_t *, unsigned int);
void Delay(long int);
void msDelay(int);

#endif /* MAIN_H_ */
