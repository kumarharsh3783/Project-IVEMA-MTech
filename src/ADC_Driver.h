/*
 * ADC_Driver.h
 *
 *  Created on: 11-Mar-2024
 *      Author: Kumar Harsh
 */

#ifndef ADC_DRIVER_H_
#define ADC_DRIVER_H_

/*********************************************************************
 * Includes
 *********************************************************************/
#include "stm32f10x.h"
#include "external.h"

/*********************************************************************
 * Macros Definitions
 ********************************************************************/
#define 	adc_mod1			1u
#define 	adc_mod2			2u
#define 	adc_mod3			3u

#define 	APB2ENR_ADC1EN		0x0200
#define 	APB2ENR_ADC2EN		0x0400
#define		APB2ENR_ADC3EN		0x8000

#define 	CFGR_ADCPRE_DIV2	(0u)
#define		CFGR_ADCPRE_DIV4	(1 << 14)
#define 	CFGR_ADCPRE_DIV6	(1 << 15)
#define 	CFGR_ADCPRE_DIV8	((1 << 15) | (1 << 14))

#define 	CR2_ADON			(1 << 0)
#define		CR2_CONT			(1 << 1)
#define 	CR2_CAL				(1 << 2)
#define 	CR2_RSTCAL			(1 << 3)
#define 	CR2_ALIGN			(1 << 11)
#define 	CR2_LEFT_ALIGN		(1 << 11)
#define 	CR2_RIGHT_ALIGN		~(1 << 11)

/* Temperature Conversion Macros */
#define 	V25					1.43
#define 	AVG_SLOPE			0.0043
#define 	VSENSE				(3.3/4095)

/*********************************************************************
 * Variable Declarations
 *********************************************************************/

/*********************************************************************
 * Function Declarations
 *********************************************************************/
float map(long x, long in_min, long in_max, long out_min, long out_max);

void adcInit(void);
void adcCalibration(unsigned char);
void adcDeInit(void);
void adcConverterDisable(unsigned char);
unsigned int adcReadDataReg(unsigned char);

void adc1Init(void);
void adc1ConverterEnable(void);
void adc1ConverterDisable(void);
void adc1StartConversion(void);
float get_Temperature(uint16_t);

#endif /* ADC_DRIVER_H_ */
