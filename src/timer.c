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
int DELAY_mSECS	= (10 * DELAY_BASE);

/**
 * Brief:	Configure Timer 4 as general purpose timer.
 * Param:	none
 * Return:	none
 */
void timerInit()
{
	/* Clock to TIM Peripherals */
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;		/* clock enabled to TIM4 Peripheral */
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	/* Configuration to timer registers */
	TIM4->ARR = 7200;
	TIM4->PSC = 9;							/* pre-scaler: (10-1) @ 1 KHz = 1ms */
	TIM4->CR1 |= TIM_CR1_URS;				/* Only counter overflow/underflow generates an update interrupt */
	TIM4->DIER |= TIM_DIER_UIE;				/* Update Interrupt Enable */
	TIM4->EGR |= TIM_EGR_UG;				/* Re-initialize the counter and generates an update of the registers */

	TIM2->ARR = 7200;
	TIM2->PSC = 9;							/* pre-scaler: (10-1) @ 1 KHz = 1ms */
	TIM2->CR1 |= TIM_CR1_URS;				/* Only counter overflow/underflow generates an update interrupt */
	TIM2->DIER |= TIM_DIER_UIE;				/* Update Interrupt Enable */
	TIM2->EGR |= TIM_EGR_UG;				/* Re-initialize the counter and generates an update of the registers */

	NVIC_SetPriority(30,1);					/* TIM4 Priority has been lowered */

	NVIC_EnableIRQ(TIM4_IRQn);				/* Enable Timer Interrupt Request in NVIC */
	NVIC_EnableIRQ(TIM2_IRQn);				/* Enable Timer Interrupt Request in NVIC */
}

/**
 * Brief:	Enable timer TIM4
 * Param:	none
 * Return:	none
 */
void timerON()
{
	TIM_Cmd(TIM4,ENABLE);					/* Calling Standard Library Function: stm32f10x_tim.c */
}

/**
 * Brief:	Disable timer TIM4
 * Param:	none
 * Return:	none
 */
void timerOFF()
{
	TIM_Cmd(TIM4,DISABLE);					/* Calling Standard Library Function: stm32f10x_tim.c */
}

/**
 * Brief:	TIM4 Interrupt Service Routine
 * Param:	none
 * Return:	none
 */
void TIM4_IRQHandler()
{
	timerOFF();								/* Disable Timer */

	if(mSec != DELAY_mSECS)
	{
		mSec++;								/* Count for Number of milliseconds passed */
	}
	else
	{
		if(driverTripFlag == 1)				/* Ensure geoUpdate works only when driver inside */
		{
			GPIOC->BSRR |= GPIO_BSRR_BS13;	/* LED gets OFF showing system is busying communicating with server */

			/* Write Code for the task to be done every 10 seconds */
			/* Call to driverUpdate API */
			uart1InterruptRxDisable();		/* Disable USART1 Interrupt */
			USART_Cmd(USART1,DISABLE);		/* Disable USART1 */

			while(!driverUpdate())			/* update geo-location */
			{
				/**** Write to EEPROM ****/
				eepromHasData = 1;
				eeprom_pageWrite(0xA0,page_addr_write,latitude);
				page_addr_write += 8;		/* update the address of next page */
				lcd_msDelay(5);				/* 5ms delay to let the eeprom write into memory */
				eeprom_pageWrite(0xA0,page_addr_write,longitude);
				page_addr_write += 8;		/* update the address of next page */
				lcd_msDelay(DELAY_EepromEvent);	/* Delay of 45 secs for EEPROM write */
			}

			uart1InterruptRxEnable();		/* Enable Receive Interrupt Enable for UART1 */
			USART_Cmd(USART1,ENABLE);		/* Enable USART1 for further RFID read */
			USART_ReceiveData(USART1);		/* Dummy read to end the required interrupt */
			resetBuffer(rfidBuffer, 13);	/* Reset rfidBuffer */
			rfidIndex = 0;
			clear = 1;

			GPIOC->BSRR |= GPIO_BSRR_BR13;	/* LED gets ON showing system is idle for input */
		}
		mSec = 0;
	}
	TIM4->SR &= ~(TIM_SR_UIF);				/* Clearing interrupt flag */
	TIM4->CNT = 0x00;						/* Reset TIMER Counter for precise delay counts */

	timerON();								/* Enable Timer */
}

/**
 * Brief:	TIM2 Interrupt Service Routine for timeout functionality
 * Param:	none
 * Return:	none
 */
void TIM2_IRQHandler()
{
	TIM_Cmd(TIM2,DISABLE);					/* Disable Timer */

	if(timeout != DELAY_Timeout)
	{
		sysTimeout = 0;						/* Default case */
		timeout++;							/* Count for Number of milliseconds passed */
	}
	else
	{
		sysTimeout = 1;						/* To reset after timeout */
		timeout = 0;
	}
	TIM2->SR &= ~(TIM_SR_UIF);				/* Clearing interrupt flag */
	TIM2->CNT = 0x00;						/* Reset TIMER Counter for precise delay counts */

	TIM_Cmd(TIM2,ENABLE);					/* Enable Timer */
}

