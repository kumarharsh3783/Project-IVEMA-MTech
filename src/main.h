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
#include "uart.h"
#include "timer.h"
#include "stdio.h"
#include "external.h"
#include "serverApi.h"

/**********************
 * MACRO DEFINITIONS
 *********************/

#define CFG_ENABLE_GPRS_DATA_TO_SERVER			1
#define CFG_ENABLE_CRITICAL_THRESHOLD_CHECKS 	1
#define CFG_PROCESSING_RAW_VALUES				1
#define CFG_ADDITIONAL_USER_DISPLAY_MSG			1

#if CFG_ENABLE_CRITICAL_THRESHOLD_CHECKS
#define mq135_critical_threshold_limit			(uint32_t)1000u
#define mq7_critical_threshold_limit			(uint32_t)15u
#endif

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
char tempDataStr[8];
char mq135DataStr[8];
char mq7DataStr[8];

/* DMA Receive Buffer */
uint16_t dmaRcvBuf[3];

/* ADC Buffer and ADC mean value buffer for the sensors */
uint16_t adcVal_mq135;
uint32_t avgAdcVal_mq135 = 0;

uint16_t adcVal_mq7;
uint32_t avgAdcVal_mq7 = 0;

#if CFG_PROCESSING_RAW_VALUES
uint32_t mq135_zero = 200, mq7_zero = 200;
float CO2ppm, COppm;
#endif

float TemperatureValue;
float avgTemperatureValue = 0.0;

/***************************************
 * User-defined Function Declarations
 **************************************/
void systemClockInit(sysclk_MHz);
void gpioInit(void);
void dmaInit(uint32_t *, uint32_t *, unsigned int);
void Get_AverageAdcVal(void);
void PowerOnLed(void);
void PowerOffLed(void);
void ToggleLed(void);
void ProcessRawSensorData(void);

#endif /* MAIN_H_ */
