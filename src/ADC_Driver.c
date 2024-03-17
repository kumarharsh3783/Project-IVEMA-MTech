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
 * Brief : Enable peripheral clock, self calibration and Power ON for the ADC module.
 * Param : (unsigned char) adcModule : ADC1, ADC2 or ADC3.
 * RetVal : none
 */
void adcPeripheralEnable(unsigned char adcModule)
{
	switch(adcModule)
	{
		case adc_mod1:

			RCC->APB2ENR |= APB2ENR_ADC1EN; /* ADC1 interface clock enable */
			ADC1->CR2 |= CR2_ADON;			/* ADC1 Power ON */

			/*	Before starting a calibration, the ADC must have been in power-on state (ADON bit = ‘1’) for
			at least two ADC clock cycles. */
			Delay(2);

			ADC1->CR2 |= CR2_CAL;			/* Calibration is started by setting the CAL bit in the ADC_CR2 register */

			/* Once calibration done, CAL bit is reset by hardware and normal conversion can be performed */
			while(ADC1->CR2 & CR2_CAL);

			break;

		case adc_mod2:

			RCC->APB2ENR |= APB2ENR_ADC2EN; /* ADC2 interface clock enable */
			ADC2->CR2 |= CR2_ADON;			/* ADC1 Power ON */

			/*	Before starting a calibration, the ADC must have been in power-on state (ADON bit = ‘1’) for
			at least two ADC clock cycles. */
			Delay(2);

			ADC2->CR2 |= CR2_CAL;			/* Calibration is started by setting the CAL bit in the ADC_CR2 register */

			/* Once calibration done, CAL bit is reset by hardware and normal conversion can be performed */
			while(ADC2->CR2 & CR2_CAL);

			break;

		case adc_mod3:

			RCC->APB2ENR |= APB2ENR_ADC3EN; /* ADC3 interface clock enable */
			ADC3->CR2 |= CR2_ADON;			/* ADC1 Power ON */

			/*	Before starting a calibration, the ADC must have been in power-on state (ADON bit = ‘1’) for
			at least two ADC clock cycles. */
			Delay(2);

			ADC3->CR2 |= CR2_CAL;			/* Calibration is started by setting the CAL bit in the ADC_CR2 register */

			/* Once calibration done, CAL bit is reset by hardware and normal conversion can be performed */
			while(ADC3->CR2 & CR2_CAL);

			break;
		default:
			/* Nothing */;
	}
}

/**
 * Brief : ADC Peripheral Clock Setting must not exceed 14MHz. Here ADC Prescaler is set to 6.
 * 		Hence, SysClockFrequency / 6 will be set as ADC Peripheral Frequency. After setting the clock frequency,
 * 		Requested ADC peripheral is powered on with proper calibration.
 * Param : (unsigned char) adcModule : ADC1, ADC2 or ADC3
 * RetVal : None.
 */
void adcInit(unsigned char adcModule)
{
	RCC->CFGR |= CFGR_ADCPRE_DIV6;			/* PCLK2 divided by 6 i.e. 72(max) / 6 = 12 MHz */

	adcPeripheralEnable(adcModule);			/* Enable peripheral clock for specific ADC module */
}

/**
 * Brief : Power down disable for the specific ADC module and reset the calibration registers.
 * Param : (unsigned char) adcModule : ADC1, ADC2 or ADC3.
 * RetVal : none
 */
void adcPeripheralDisable(unsigned char adcModule)
{
	switch(adcModule)
	{
		case adc_mod1:

			ADC1->CR2 &= ~(ADC_CR2_ADON);	/* Disable ADC1 Module */

			ADC1->CR2 |= (ADC_CR2_RSTCAL);	/* Reset Calibration */

			/* Wait till Calibration register initialised */
			while(ADC1->CR2 & ADC_CR2_RSTCAL);

			break;

		case adc_mod2:

			ADC2->CR2 &= ~(ADC_CR2_ADON);	/* Disable ADC2 Module */

			ADC2->CR2 |= (ADC_CR2_RSTCAL);	/* Reset Calibration */

			/* Wait till Calibration register initialised */
			while(ADC2->CR2 & ADC_CR2_RSTCAL);

			break;

		case adc_mod3:

			ADC3->CR2 &= ~(ADC_CR2_ADON);	/* Disable ADC3 Module */

			ADC3->CR2 |= (ADC_CR2_RSTCAL);	/* Reset Calibration */

			/* Wait till Calibration register initialised */
			while(ADC3->CR2 & ADC_CR2_RSTCAL);

			break;

		default:
			/* Nothing */;
	}
}

/**
 * Brief : De-initialise ADC module.
 * Param : (unsigned char) adcModule : ADC1, ADC2 or ADC3
 * RetVal : None.
 */
void adcDeInit(unsigned char adcModule)
{
	adcPeripheralDisable(adcModule);		/* Disable peripheral clock for specific ADC module */
}

/**
 * Brief: Start the ADC conversion and read the data register once End of Conversion.
 * Param: (unsigned char) adcModule : ADC1, ADC2 or ADC3
 * RetVal: (unsigned int) adcVal contains the ADC converted data.
 */

unsigned int adcStartConversion(unsigned char adcModule)
{
	unsigned int adcVal;
	switch(adcModule)
	{
		case adc_mod1:

			ADC1->CR2 |= ADC_CR2_SWSTART;		/* Start Conversion */

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
