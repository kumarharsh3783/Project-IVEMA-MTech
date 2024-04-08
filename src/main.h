/*
 * main.h
 *
 *  Created on: 03-Mar-2021
 *      Author: Kumar Harsh
 */

#ifndef MAIN_H_
#define MAIN_H_

/*****************
 * Include files
 *****************/
#include "stm32f10x.h"
#include "lcd.h"
#include "ADC_Driver.h"
#include "external.h"
#include "timer.h"
#include "stdio.h"

/**************************
 * User-defined Variables
 *************************/

/* System Clock Frequencies */
typedef enum
{
	sysclk_72MHz,
	sysclk_56MHz
}sysclk_MHz;

/* Sensor Data to String Buffer */
char sensorDataStr[8];

/* DMA Receive Buffer */
uint16_t dmaRcvBuf[3];

/* ADC Buffer and ADC mean value buffer for the sensors */
uint16_t adcVal_mq135;
uint32_t avgAdcVal_mq135 = 0;

uint16_t adcVal_mq7;
uint32_t avgAdcVal_mq7 = 0;

float TemperatureValue;
float avgTemperatureValue = 0.0;

/***************************************
 * User-defined Function Declarations
 **************************************/
void systemClockInit(sysclk_MHz);
void gpioInit(void);
void dmaInit(uint32_t *, uint32_t *, unsigned int);
void Get_AverageAdcVal(void);

#endif /* MAIN_H_ */
