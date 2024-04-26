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
uint8_t sysTimeout = 0;

/**
 * Brief:	Configure Timer 4 as general purpose timer.
 * Param:	none
 * Return:	none
 */
void timerInit()
{
	/* Clock to TIM Peripherals */
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;		/* clock enabled to TIM2 Peripheral */
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;		/* clock enabled to TIM4 Peripheral */

	/* Configuration to timer registers */

	/*************************************************************
	 * Timer delay = (ARR * (Prescaler + 1))/Timer Input Frequency
	 * 			= (56000 * 100)/56000000 = 100ms
	 ************************************************************/
	TIM2->ARR = 56000;
	TIM2->PSC = 99;
	/* Only counter overflow/underflow generates an update interrupt */
	TIM2->CR1 |= TIM_CR1_URS;
	/* Update interrupt enabled */
	TIM2->DIER |= TIM_DIER_UIE;
	/* Re-initialize the counter and generates an update of the registers */
	TIM2->EGR |= TIM_EGR_UG;
	/* Enable Timer Interrupt Request in NVIC */
	NVIC_EnableIRQ(TIM2_IRQn);

	/*************************************************************
	 * Timer delay = (ARR * (Prescaler + 1))/Timer Input Frequency
	 * 			= (56000 * 1)/56000000 = 1ms
	 ************************************************************/
	TIM4->ARR = 56000;
	TIM4->PSC = 0;							/* pre-scaler: (1-1) @ 1 KHz = 1ms */

}

/**
 * Brief:	Enable timer TIM2 for system Clocking in every 10ms - loop
 * Param:	none
 * Return:	none
 */
void timer2ON()
{
	TIM_Cmd(TIM2,ENABLE);					/* Calling Standard Library Function: stm32f10x_tim.c */
}

/**
 * Brief:	Disable timer TIM2
 * Param:	none
 * Return:	none
 */
void timer2OFF()
{
	TIM_Cmd(TIM4,DISABLE);					/* Calling Standard Library Function: stm32f10x_tim.c */
}

/**
 * Brief: TIM2 Interrupt Service Routine
 */
void TIM2_IRQHandler()
{
	/* Disable Timer */
	TIM_Cmd(TIM2,DISABLE);

	if(sysTimeout != TIMEOUT_DETECTION)
	{
		/* Timeout Detection is set till 10s */
		++sysTimeout;
	}

	/* Clearing interrupt flag */
	TIM2->SR &= ~(TIM_SR_UIF);
	/* Reset TIMER Counter for precise delay counts */
	TIM2->CNT = 0x00;
	/* Enable Timer */
	TIM_Cmd(TIM2,ENABLE);
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

/** Brief : Delay in milliseconds API */
void delay_in_ms(unsigned int ms)
{
	DELAY_mSECS = ms;

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
