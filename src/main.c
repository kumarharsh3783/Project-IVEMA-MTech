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
 * 			USART2[sim900a]
 * --------------------------------------
 * 	TX:		PA2
 * 	RX:		PA3
 * 	-------------------------------------
 */
int main(void)
{
	/** 1. System Initialisation **/

	/* System Clock Frequency : 56 MHz */
	systemClockInit(sysclk_56MHz);
	/* All the GPIOs required for the project is initialized here */
	gpioInit();
	/* Initialise TIM4 */
	timerInit();
	/* Initialise ADC1 Module */
	adcInit();
	/* Initialise DMA for 3 ADC conversions transfer */
	dmaInit((uint32_t *)&ADC1->DR, (uint32_t *)dmaRcvBuf, 3u);
	/* Initialise USART1: RFID and USART2: GPRS/GPS */
	uartInit();
	/* Enable USART2 - SIM900a */
	USART_Cmd(USART2, ENABLE);

	/** End of System Initialisation **/

	/** 2. Display Peripheral Init **/

	/* Initialise 16*2 LCD */
	lcdInit();

	/** End of Display Peripheral Init **/

	/** 3. Display Message on LCD "Gas Sensors Warming Up" **/

	lcdCursorSet(0,0);
	lcd_send_string("MQ Gas Sensors  ");
	lcdCursorSet(1,0);					/* Change cursor to line 1 */
	lcd_send_string("warming up! Wait");

	/** End of Display Message on LCD "Gas Sensors Warming Up" **/

	/** 4. Wait for 90 seconds for gas sensor warming up **/

	delay_in_sec(90);

	/** End of Wait for 90 seconds for gas sensor warming up **/

	/** 5. Read ADC values for the Sensors **/

	/* Start the ADC Conversion */
	adc1StartConversion();

	/* infinite loop */
	while(1)
	{
		/* LED LD2 Toggle */
		GPIOA->ODR ^= GPIO_ODR_ODR5;

		/** 6. Calculate Average value for last 10 samples for each sensor **/

		Get_AverageAdcVal();

		/** End of Calculate Average value for last 10 samples for each sensor **/

		/** 8. Display average PPM and temperature values on LCD **/

		clearLcd();

		sprintf(sensorDataStr, "%lu", avgAdcVal_mq135);
		lcdCursorSet(0,0);
		lcd_send_string("  MQ135 ");
		lcd_send_string(sensorDataStr);

		sprintf(sensorDataStr, "%lu", avgAdcVal_mq7);
		lcdCursorSet(1,0);
		lcd_send_string("MQ7 ");
		lcd_send_string(sensorDataStr);

		sprintf(sensorDataStr, "%.2f", avgTemperatureValue);
		lcdCursorSet(1,9);
		lcd_send_string("T ");
		lcd_send_string(sensorDataStr);

		/** End of Display average PPM and temperature values **/

		delay_in_sec(10);
	}
}

/**
 * Brief:	Configure the system clock to run on 72MHz (Max allowed Clock setting).
 * Param:	(sysclk_MHz) enumeration for selecting system clock frequency
 * Return:	none
 */
void systemClockInit(sysclk_MHz sysclk)
{
	/* Reset Registers to default state */
	RCC->CR |= RCC_CR_HSION;

	RCC->CFGR = (uint32_t)(0);
	RCC->CR &= (uint32_t)((uint32_t)~(RCC_CR_PLLON | RCC_CR_HSEON | RCC_CR_CSSON | RCC_CR_HSEBYP));

	/* External HSE Clock is set On - 8.0 MHz crystal can be seen on the board HW */
	RCC->CR |= RCC_CR_HSEON;

	/* To check if HSE clock is stable or not */
	while(!(RCC->CR & RCC_CR_HSERDY));

	/* Reset Flash Latency */
	FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);

	/* Enable Prefetch Buffer and Set Latency to Two wait states, if 48 MHz < SYSCLK <= 72 MHz */
	FLASH->ACR |= (uint32_t)(FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2);

	/* Reset the CFGR register */
	/* PLLXTPRE bit set to 0 - HSE not divided before PLL Entry */
	RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |  RCC_CFGR_PLLMULL);

	switch(sysclk)
	{
	case sysclk_72MHz:

		RCC->CFGR |= RCC_CFGR_HPRE_DIV1;  	/* AHB pre-scaler -> 72 MHz*/
		RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;  	/* APB1 pre-scaler -> 72/2 = 36 MHz */
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;  	/* APB2 pre-scaler -> 72 MHz */

		/* PLL multiplier & HSE Clock Selected */
		RCC->CFGR |= (RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9);

		break;

	case sysclk_56MHz:

		RCC->CFGR |= RCC_CFGR_HPRE_DIV1;  	/* AHB pre-scaler -> 56 MHz*/
		RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;  	/* APB1 pre-scaler -> 56/2 = 28 MHz & APB1 Timer Clocks -> 28x2 = 56 MHz */
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;  	/* APB2 pre-scaler -> 56/2 = 28 MHz */

		/* PLL multiplier & HSE Clock Selected */
		RCC->CFGR |= (RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL7);

		break;

	default:

		RCC->CFGR |= RCC_CFGR_HPRE_DIV1;  	/* AHB pre-scaler -> 72 MHz*/
		RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;  	/* APB1 pre-scaler -> 72/2 = 36 MHz */
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;  	/* APB2 pre-scaler -> 72 MHz */

		/* PLL multiplier & HSE Clock Selected */
		RCC->CFGR |= (RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9);

		break;
	}

	/* Turn on PLL */
	RCC->CR |= RCC_CR_PLLON;

	/* wait till PLL is locked */
	while(!(RCC->CR & RCC_CR_PLLRDY));

	/* PLL is selected as system clock */
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	/* Check if system clock is stable or not */
	while(!(RCC->CFGR & RCC_CFGR_SWS_PLL));

	/* Update the systick */
	SystemCoreClockUpdate();
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

	/* Defining number of data to transfer via DMA */
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
 * Brief :
 *
 */
void Get_AverageAdcVal()
{
	uint8_t readSamples;

	avgAdcVal_mq135 = 0;
	avgAdcVal_mq7 = 0;
	avgTemperatureValue = 0.0;

	for(readSamples = 0; readSamples < MAX_NO_OF_SAMPLES; readSamples++)
	{
		/* Reading mq135 sensor */
		adcVal_mq135 = dmaRcvBuf[0];
		/* Reading mq7 sensor */
		adcVal_mq7 = dmaRcvBuf[1];
		/* Reading Temperature sensor */
		TemperatureValue = get_Temperature(dmaRcvBuf[2]);

		/* Adding 10 samples for each parameter */
		avgAdcVal_mq135 += (uint32_t)adcVal_mq135;
		avgAdcVal_mq7 += (uint32_t)adcVal_mq7;
		avgTemperatureValue += TemperatureValue;
	}

	/* Calculating Average Values */
	avgAdcVal_mq135 /= MAX_NO_OF_SAMPLES;
	avgAdcVal_mq7 /= MAX_NO_OF_SAMPLES;
	avgTemperatureValue /= MAX_NO_OF_SAMPLES;
}
