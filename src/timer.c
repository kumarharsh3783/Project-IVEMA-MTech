/*
 * timer.c
 *
 *  Created on: 04-Mar-2021
 *      Author: Kumar Harsh
 */

/*<-- Includes -->*/
#include "timer.h"

/**
 * User-defined Variables
 */
int DELAY_mSECS;

/**
 * Brief:	Configure Timer 4 as general purpose timer.
 * Param:	none
 * Return:	none
 */
void timerInit()
{
	/* Clock to TIM Peripherals */
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;		/* clock enabled to TIM4 Peripheral */

	/* Configuration to timer registers */
	TIM4->ARR = 56000;
	TIM4->PSC = 0;							/* pre-scaler: (1-1) @ 1 KHz = 1ms */

}

/**
 * Brief:	Enable timer TIM4
 * Param:	none
 * Return:	none
 */
void timer4ON()
{
	TIM_Cmd(TIM4,ENABLE);					/* Calling Standard Library Function: stm32f10x_tim.c */
}

/**
 * Brief:	Disable timer TIM4
 * Param:	none
 * Return:	none
 */
void timer4OFF()
{
	TIM_Cmd(TIM4,DISABLE);					/* Calling Standard Library Function: stm32f10x_tim.c */
}

/** Brief : Delay in seconds API */
void delay_in_sec(unsigned int seconds)
{
	DELAY_mSECS = seconds * DELAY_BASE;

	/* Reset the Counter */
	TIM4->CNT = 0;

	/* Start the timer 4 */
	timer4ON();

	while(DELAY_mSECS)
	{
		/* wait until 1ms */
		while(!(TIM4->SR & TIM_SR_UIF));

		TIM4->SR &= ~(TIM_SR_UIF);			/* Clearing interrupt flag */
		--DELAY_mSECS;
	}

	/* End the timer 4 */
	timer4OFF();
}
