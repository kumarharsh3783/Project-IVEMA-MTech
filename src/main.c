/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    03-March-2021
  * @brief   Default main function.
  ******************************************************************************
*/

/*<-- Includes -->*/
#include "main.h"

/**
 * Brief:	Hardware Connection Details
 * 			USART1[rfid] | USART2[sim808]
 * --------------------------------------
 * 	TX:		PA9			 |			PA2
 * 	RX:		PA10		 |			PA3
 * 	-------------------------------------
 */
int main(void)
{
	systemClockInit();					/* System Clock Frequency : 72 MHz */
	gpioInit();							/* All the GPIOs required for the project is initialized here */
/*
	uartInit();							 Initialise USART1: RFID and USART2: GPRS/GPS
	timerInit();						 Initialise TIM4 with Interrupt feature enabled
*/
	lcdInit();							/* Initialise 16*2 LCD */
	adcInit();							/* Initialise ADC1 Module */

	/* Initialise DMA for 2 ADC conversions transfer */
	dmaInit((uint32_t *)&ADC1->DR, (uint32_t *)dmaRcvBuf, 2u);
/*
	uart1InterruptRxEnable();			Enable Receive Interrupt Enable for UART1
	USART_Cmd(USART1, ENABLE); 			Enable USART1 - RFID
	USART_Cmd(USART2, ENABLE); 			Enable USART2 - SIM808
*/

	/* Display Welcome Message */
//	lcd_send_string("--------------------");
//	lcdCursorSet(1,0);					/* Change cursor to line 1 */
//	lcd_send_string("| Welcome to Savy  |");
//	lcdCursorSet(2,0);					/* Change cursor to line 2 */
//	lcd_send_string("| Electric Vehicle |");
//	lcdCursorSet(3,0);					/* Change cursor to line 3 */
//	lcd_send_string("--------------------");
	//lcd_msDelay(15000);					/* 15 Seconds Delay */

	/* Display Initial message to User */
//	clearLcd();
//	lcdCursorSet(1,2);					/* Change cursor to line 1 */
//	lcd_send_string("Swipe Smart Card");
//	lcdCursorSet(2,2);					/* Change cursor to line 2 */
//	lcd_send_string("to start trip...");

	/* Start the ADC Conversion */
	adc1StartConversion();

	while(1)							/* infinite loop */
	{
		/* LED LD2 Toggle */
		GPIOA->ODR ^= GPIO_ODR_ODR5;

		adcVal_mq135 = dmaRcvBuf[0];

		adcVal_mq7 = dmaRcvBuf[1];

		//lcd_msDelay(1);
	}
}

/**
 * Brief:	Configure the system clock to run on 72MHz (Max allowed Clock setting).
 * Param:	none
 * Return:	none
 */
void systemClockInit()
{
	RCC->CR |= RCC_CR_HSEON;			/* External HSE Clock is set On */
	while(!(RCC->CR & RCC_CR_HSERDY));	/* To check if HSE clock is stable or not */

	RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |  RCC_CFGR_PLLMULL);  /* Reset the CFGR register */

	RCC->CFGR &= ~(RCC_CFGR_PLLXTPRE);  /* PLLXTPRE bit set to 0 */
	RCC->CFGR |= RCC_CFGR_PLLSRC;   	/* PLL source */
	RCC->CFGR |= RCC_CFGR_PLLMULL9;  	/* PLL multiplier */
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;  	/* AHB pre-scaler */
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;  	/* APB1 pre-scaler */
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;  	/* APB2 pre-scaler */

	RCC->CR |= RCC_CR_PLLON;			/* Turn on PLL */
	while(!(RCC->CR & RCC_CR_PLLRDY)); 	/* wait till PLL is locked */

	RCC->CFGR |= RCC_CFGR_SW_PLL; 		/* PLL is selected as system clock */
	while(!(RCC->CFGR & RCC_CFGR_SWS_PLL));	/* Check if system clock is stable or not */

	SystemCoreClockUpdate();			/* Update the systick */
}

/**
 * Brief:	Configure the GPIOs for UART / LCD in their specified configurations.
 * Param:	none
 * Return:	none
 */
void gpioInit()
{
	/* Clock to GPIO Peripherals - Port A, B, C */
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;	/* Enable clock to GPIO Port A for UARTs and GPIO B LCD*/
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;							/* Enable clock to GPIO Port C for LED indicator */
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;							/* Alternate Function IO clock enabled */

	/*<--------------USART Clock/GPIO Configuration------------->*/
/*
	GPIOA->CRH |= GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9;				 TX pin PA9 configured as output @50MHz in alternate function push pull
	GPIOA->CRH &= ~(GPIO_CRH_CNF9_0);							 RX pin PA10 configured as input in floating input mode
	GPIOA->CRL |= GPIO_CRL_CNF2_1 | GPIO_CRL_MODE2;				 TX pin PA2 configured as output @50MHz in alternate function push pull
	GPIOA->CRL &= ~(GPIO_CRL_CNF2_0);							 RX pin PA3 configured as input in floating input mode
*/

	/*<------------------LCD GPIO Configuration----------------->*/
	/* [PB15,PB14,PB13,PB12] <=> [DB7,DB6,DB5,DB4] are configured */
	GPIOB->CRH |= GPIO_CRH_MODE12_0;							/* GPIO configured as Output mode @ 10 MHz */
	GPIOB->CRH &= ~(GPIO_CRH_CNF12_0);							/* and General Purpose PUSH-PULL : DB4 */
	GPIOB->CRH |= GPIO_CRH_MODE13_0;							/* GPIO configured as Output mode @ 10 MHz */
	GPIOB->CRH &= ~(GPIO_CRH_CNF13_0);							/* and General Purpose PUSH-PULL : DB5*/
	GPIOB->CRH |= GPIO_CRH_MODE14_0;							/* GPIO configured as Output mode @ 10 MHz */
	GPIOB->CRH &= ~(GPIO_CRH_CNF14_0);							/* and General Purpose PUSH-PULL : DB6 */
	GPIOB->CRH |= GPIO_CRH_MODE15_0;							/* GPIO configured as Output mode @ 10 MHz */
	GPIOB->CRH &= ~(GPIO_CRH_CNF15_0);							/* and General Purpose PUSH-PULL : DB7 */

	GPIOA->CRL |= GPIO_CRL_MODE0_0;								/* GPIO configured as Output mode @ 10 MHz */
	GPIOA->CRL &= ~(GPIO_CRL_CNF0);								/* and General Purpose PUSH-PULL : RS bit */
	GPIOA->CRL |= GPIO_CRL_MODE1_0;								/* GPIO configured as Output mode @ 10 MHz */
	GPIOA->CRL &= ~(GPIO_CRL_CNF1);								/* and General Purpose PUSH-PULL : EN bit */

	/*<------------------LED GPIO Configuration----------------->*/
	/* PA5 configured as Output push pull for LD2 */
	GPIOA->CRL |= GPIO_CRL_MODE5_0;								/* GPIOA Pin13 configured as output @ 10MHz */
	GPIOA->CRL &= ~(GPIO_CRL_CNF5);								/* GPIOA Pin13 configured as General Output Push Pull */

	/*<------------------ADC Pins Configuration----------------->*/
	/* [PC2,PC3] are configured as Analog input pins */
	GPIOC->CRL &= ~(GPIO_CRL_MODE2);							/* GPIOC Pin2 configured as Input mode */
	GPIOC->CRL &= ~(GPIO_CRL_CNF2);								/* and configured as Analog mode */
	GPIOC->CRL &= ~(GPIO_CRL_MODE3);							/* GPIOC Pin3 configured as Input mode */
	GPIOC->CRL &= ~(GPIO_CRL_CNF3);								/* and configured as Analog mode */
}

/**
 * Brief : DMA Initialisation API
 * Param : (uint32_t *) src : Source Address
 * 		(uint32_t *) dst : Destination Address
 * 		(unsigned int) len : Number of Data to transfer via DMA
 * RetVal : None.
 */
void dmaInit(uint32_t *src, uint32_t *dst, unsigned int len)
{
	/* Enable the peripheral clock for DMA1 */
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;

	/* Assigning Peripheral Address into DMA register */
	DMA1_Channel1->CPAR = (uint32_t)(src);

	/* Assigning Memory Address into DMA register */
	DMA1_Channel1->CMAR = (uint32_t)(dst);

	/* Defining 2 number of data to transfer via DMA */
	DMA1_Channel1->CNDTR = (DMA_CNDTR1_NDT & len);

	/* Memory & Peripheral Size set to 16 bit */
	DMA1_Channel1->CCR |= DMA_CCR1_MSIZE_0 | DMA_CCR1_PSIZE_0;

	/* Memory Increment Set to read Both ADC channels from Data Register */
	DMA1_Channel1->CCR |= DMA_CCR1_MINC;

	/* Circular mode enabled for ADC SCAN compatibility */
	DMA1_Channel1->CCR |= DMA_CCR1_CIRC;

	/* To enable DMA channel 1 as it contains ADC1 module mapped */
	DMA1_Channel1->CCR |= DMA_CCR1_EN;
}

/**
 * Brief:	Delay function
 * Param:	(long int) count - to count while delaying
 * Return:	none
 */
void Delay(long int count)
{
	while(count!=0)
		count--;
}

void msDelay(int count)
{
	int extCount;
	for(extCount=0; extCount < 5125; extCount++)
	{
		while(count!=0)
		{
			count--;
		}
	}
}
