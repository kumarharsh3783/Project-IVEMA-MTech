/*
 * ADC_Driver.c
 *
 *  Created on: 11-Mar-2024
 *      Author: Kumar Harsh
 */

/*********************************************************************
 * Includes
 *********************************************************************/
#include "ADC_Driver.h"

/*********************************************************************
 * Variables Declarations & Definitions
 *********************************************************************/

/*********************************************************************
 * Function Definitions
 *********************************************************************/

/*************************************************
 * Brief:	ADC Hardware Connection Details
 * 					ADC1 Module Used
 * -----------------------------------------------
 * 	MQ135 Gas Sensor  :	ADC1_Ch12 [PC2]
 * 	MQ7 Gas Sensor	  :	ADC1_Ch13 [PC3]
 * 	Temperature Sensor:	ADC1_Ch16 [Internally]
 * 	----------------------------------------------
 *************************************************/

/**
 * Brief : Re-maps a number from one range to another range.
 * Param : 	value: the number to map.
 * 		in_min : the lower bound of the value current range.
 * 		in_max : the upper bound of the value current range.
 * 		out_min : the lower bound of the value target range.
 * 		out_max : the upper bound of the value target range.
 * RetVal : (float) The mapped value.
 */
float map(long x, long in_min, long in_max, long out_min, long out_max)
{
	float mapped = 0.0, slope = 0.0;
	slope = (float)(out_max-out_min)/(in_max-in_min);
	mapped = (float)(x-in_min)*slope;
	mapped += out_min;
	return mapped;
}

/**
 * Brief : ADC Peripheral Clock Setting must not exceed 14MHz. Here ADC Prescaler is set to 6.
 * 		Hence, SysClockFrequency / 6 will be set as ADC Peripheral Frequency. After setting the clock frequency,
 * 		Requested ADC peripheral is powered on with proper calibration.
 * Param : None.
 * RetVal : None.
 */
void adcInit()
{
	adc1Init();			/* Initialization for specific ADC module */
}

/**
 * Brief : Self calibration for the ADC module.
 * Param : (unsigned char) adcModule : ADC1, ADC2 or ADC3.
 * RetVal : none
 */
void adcCalibration(unsigned char adcModule)
{
	switch(adcModule)
	{
		case adc_mod1:
			/*	Before starting a calibration, the ADC must have been in power-on state (ADON bit = ‘1’) for
			at least two ADC clock cycles. */
			delay_in_ms(2);

			ADC1->CR2 |= CR2_CAL;			/* Calibration is started by setting the CAL bit in the ADC_CR2 register */

			/* Once calibration done, CAL bit is reset by hardware and normal conversion can be performed */
			while(ADC1->CR2 & CR2_CAL);

			break;

		case adc_mod2:
			/*	Before starting a calibration, the ADC must have been in power-on state (ADON bit = ‘1’) for
			at least two ADC clock cycles. */
			delay_in_ms(2);

			ADC2->CR2 |= CR2_CAL;			/* Calibration is started by setting the CAL bit in the ADC_CR2 register */

			/* Once calibration done, CAL bit is reset by hardware and normal conversion can be performed */
			while(ADC2->CR2 & CR2_CAL);

			break;

		case adc_mod3:
			/*	Before starting a calibration, the ADC must have been in power-on state (ADON bit = ‘1’) for
			at least two ADC clock cycles. */
			delay_in_ms(2);

			ADC3->CR2 |= CR2_CAL;			/* Calibration is started by setting the CAL bit in the ADC_CR2 register */

			/* Once calibration done, CAL bit is reset by hardware and normal conversion can be performed */
			while(ADC3->CR2 & CR2_CAL);

			break;
		default:
			/* Nothing */;
	}
}

/**
 * Brief : Specific Initialization for ADC1 Module.
 */

void adc1Init()
{
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN ; 		/* ADC1 interface clock enable */
	RCC->CFGR |= RCC_CFGR_ADCPRE_DIV2;			/* PCLK2 divided by 2 i.e. 28 / 2 = 14 MHz */

	/* Regular Channel Sequence */
	ADC1->SQR1 |= ADC_SQR1_L_1;					/* 0010b: 3 Conversions - PC2, PC3, Temperature Sensor */

	/**
	 * Regular Conversion Sequence -
	 * Conversion 1 : PC2 ADCx_IN12 - MQ 135
	 * Conversion 2 : PC3 ADCx_IN13 - MQ 7
	 * Conversion 3 : ADCx_IN16 - Internal Temperature Sensor
	 */
	ADC1->SQR3 |= (ADC_SQR3_SQ1_2 | ADC_SQR3_SQ1_3);
	ADC1->SQR3 |= (ADC_SQR3_SQ2_0 | ADC_SQR3_SQ2_2 | ADC_SQR3_SQ2_3);
	ADC1->SQR3 |= ADC_SQR3_SQ3_4;

	/* Sampling time set to 1.17uS for ADC12_IN12 & ADC12_IN13 and 17.1uS for temperature sensor */
	ADC1->SMPR1 |= ADC_SMPR1_SMP16; 			/* 239.5 cycles set for ADC1_IN16 channel */

	/* ADC Conversion to be started with SWSTART in continuous mode */
	ADC1->CR2 |= ADC_CR2_EXTSEL | ADC_CR2_EXTTRIG | ADC_CR2_CONT;

	/* ADC Conversion to be started in SCAN mode with DMA enabled */
	ADC1->CR2 |= ADC_CR2_DMA;
	ADC1->CR1 |= ADC_CR1_SCAN;

	/* First ADON High for Powering ON the ADC */
	/* Wake up the temperature sensor from power down mode */
	ADC1->CR2 |= ADC_CR2_TSVREFE | ADC_CR2_ADON;

	/* Calibration for ADC1 */
	adcCalibration(adc_mod1);

}

/**
 * Brief : De-initialize ADC module.
 * Param : none
 * RetVal : None.
 */
void adcDeInit(void)
{
	adcConverterDisable(adc_mod1);		/* Disable peripheral clock for specific ADC module */
}

/**
 * Brief : Power down disable for the specific ADC module and reset the calibration registers.
 * Param : (unsigned char) adcModule : ADC1, ADC2 or ADC3.
 * RetVal : none
 */
void adcConverterDisable(unsigned char adcModule)
{
	switch(adcModule)
	{
		case adc_mod1:

			ADC1->CR2 &= ~ADC_CR2_ADON;		/* Disable ADC1 Conversion */

			break;

		case adc_mod2:

			ADC2->CR2 &= ~(ADC_CR2_ADON);	/* Disable ADC2 Module */

			break;

		case adc_mod3:

			ADC3->CR2 &= ~(ADC_CR2_ADON);	/* Disable ADC3 Module */

			break;

		default:
			/* Nothing */;
	}
}

/* Brief : Power ON ADC1 Module */
void adc1ConverterEnable(void)
{
	ADC1->CR2 |= ADC_CR2_ADON;			 	/* Enable ADC1 Conversion */
}

/* Brief : Power DOWN & disable calibration for ADC1 Module */
void adc1ConverterDisable(void)
{
	ADC1->CR2 &= ~ADC_CR2_ADON;				/* Disable ADC1 Conversion */
}

/* Brief : Start ADC1 Module conversions */
void adc1StartConversion()
{
	ADC1->CR2 |= ADC_CR2_SWSTART;			/* Start Conversion */
}

/**
 * Brief: Read the data register once End of Conversion.
 * Param: (unsigned char) adcModule : ADC1, ADC2 or ADC3
 * RetVal: (unsigned int) adcVal contains the ADC converted data.
 */
unsigned int adcReadDataReg(unsigned char adcModule)
{
	unsigned int adcVal;
	switch(adcModule)
	{
		case adc_mod1:

			adc1StartConversion();				/* Start Conversion */
			/* wait for the End Of Conversion */
			while(!(ADC1->SR & ADC_SR_EOC));

			adcVal = (unsigned int) ADC1->DR;
			break;
		case adc_mod2:

			ADC2->CR2 |= ADC_CR2_SWSTART;		/* Start Conversion */

			/* wait for the End Of Conversion */
			while(!(ADC2->SR & ADC_SR_EOC));

			adcVal = (unsigned int) ADC2->DR;
			break;
		case adc_mod3:

			ADC3->CR2 |= ADC_CR2_SWSTART;		/* Start Conversion */

			/* wait for the End Of Conversion */
			while(!(ADC3->SR & ADC_SR_EOC));

			adcVal = (unsigned int) ADC3->DR;
			break;

		default:
			/* Nothing */;
	}
	return adcVal;
}

/**
 * Brief : Temperature value conversion in degree Celsius.
 * Param : (uint16_t)ADC_VAL from DMA
 * RetVal : (float) temperature.
 */
float get_Temperature(uint16_t ADC_VAL)
{
	float temperature;
	temperature = ((V25 - VSENSE*ADC_VAL) / AVG_SLOPE) + 25;
	return temperature;
}

