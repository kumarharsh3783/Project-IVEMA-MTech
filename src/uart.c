/*
 * uart.c
 *
 *  Created on: 03-Mar-2021
 *      Author: Kumar Harsh
 */


/*<-- Includes -->*/
#include "uart.h"

/*<-- Global Variables -->*/
USART_InitTypeDef* USART_InitStruct = &usart;

/**
 * Brief:	Initialize USART1 and USART2
 * Param:	none
 * Return:	none
 */
void uartInit()
{
	/* Clock to USART Peripherals */
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;			/*Clock Enabled in USART1 peripheral*/
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;			/*Clock Enabled in USART2 peripheral*/

	/* USART Initialization */
	USART_StructInit(USART_InitStruct);				/*Baud: 9600 bits per second*/

	USART_Init(USART1,USART_InitStruct);

	changeBaud(BAUD_57600);							/*Changing the Baud Rate from default 9600 to 57600*/

	USART_Init(USART2,USART_InitStruct);
	NVIC_SetPriority(37,1);							/* USART1 Priority has been lowered */
}

/**
 * Brief:	Configure the Baud Rate for USART
 * Param:	baudRate - (uint32_t) data-type
 * Return:	none
 */
void changeBaud(uint32_t baudRate)
{
	USART_InitStruct->USART_BaudRate = baudRate;
}

/**
 * Brief:	Enable UART1 interrupt for Receive only.
 * Param:	none
 * Return:	none
 */
void uart1InterruptRxEnable()
{
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);			/* Initialization of Serial Receive Interrupt */
	NVIC_EnableIRQ(USART1_IRQn);
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);			/* RXNE flag bit is cleared */
	NVIC_ClearPendingIRQ(USART1_IRQn);						/* Clear pending interrupt request before enabling interrupt */
}

/**
 * Brief:	Disable UART1 interrupt for Receive.
 * Param:	none
 * Return:	none
 */
void uart1InterruptRxDisable()
{
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	/* Initialization of Serial Receive Interrupt */
	NVIC_DisableIRQ(USART1_IRQn);
}

/**
 * Brief:	Transmit UART multiple data
 * Param:	(Pointer to a character) txBuffer - contains the string to be sent via UART
 * Return:	none
 */
void uartSendData(char* txBuffer)
{
	for(txIndex = 0; txBuffer[txIndex]!='\0'; txIndex++)
	{
		USART_SendData(USART2, (uint16_t)txBuffer[txIndex]);	//instruct to dump TDR value into shift register
		while(!USART_GetFlagStatus(USART2, USART_FLAG_TXE));	//wait till TDR is empty
	}
	while(!USART_GetFlagStatus(USART2, USART_FLAG_TC));			//wait till TC get set
	USART_ClearFlag(USART2, USART_FLAG_TC);						//clearing TC bit for next transmission frame
}

/**
 * Brief:	Receive UART data into receive buffer
 * Param:	(pointer to a char) Address where received response string is saved, (int) No. of lines to read
 * Return:	none
 */
void uartReceiveData(char* sim808Response,int lineCount)
{
	int responseIndex = 0;
	uint16_t rBuf;

	TIM_Cmd(TIM2, ENABLE);											/* Enable Timer 2 for timeout feature */

	while(lineCount != 0)
	{

		while(!USART_GetFlagStatus(USART2, USART_FLAG_RXNE))		/* wait till receive data buffer is empty */
		{
			if(sysTimeout == 1)
			{
				TIM_Cmd(TIM2, DISABLE);									/* Disable Timer 2 for timeout feature */
				TIM2->CNT = 0;
				sysTimeout = 0;											/* Default status */
				return;
			}
		}
		rBuf = USART_ReceiveData(USART2);

		if(rBuf == '\n')
		{
			lineCount--;										/* decrement line count to determine lines to be read */
		}
		else if(rBuf == '\r')
		{
			/* skipping carriage return character when received from Serial, Do nothing */
		}
		else
		{
			sim808Response[responseIndex] = rBuf;				/* save into sim808Response Buffer */
			responseIndex++;
		}
	}
	sim808Response[responseIndex] = '\0';						/* adding null char at the end of string */
	TIM_Cmd(TIM2, DISABLE);										/* Disable Timer 2 for timeout feature */
	TIM2->CNT = 0;
	timeout = 0;												/* Reset timeout */
}

int strcmpArr(char* searchstring, char (*refstring)[13])
{
	if(strcmp(refstring[0],searchstring) == 0)
	{
		riderIdPos = 0;											/* identify the position of riderId in riderRfid array */
		return 0;
	}
	else if(strcmp(refstring[1],searchstring) == 0)
	{
		riderIdPos = 1;											/* identify the position of riderId in riderRfid array */
		return 0;
	}
	else if(strcmp(refstring[2],searchstring) == 0)
	{
		riderIdPos = 2;											/* identify the position of riderId in riderRfid array */
		return 0;
	}
	else
	{
		/**** Return 1 if string not found in the string array ****/
		riderIdPos = 99;										/* arbitary value of pos to show invalid value */
		return 1;
	}
}

/**
 * Brief:	USART1 [RFID] Interrupt Service Routine.
 * Param:	none
 * Return:	none
 */
void USART1_IRQHandler()
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE))
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);			/* RXNE flag bit is cleared */
		rfidBuffer[rfidIndex] = USART_ReceiveData(USART1);		/* Reading RXNE buffer */

		/* Increase the RFID index after saving the incoming data byte */
		++rfidIndex;

		/* Check for the complete reception of RFID Unique ID */
		if(rfidIndex == 12)
		{
			GPIOC->BSRR |= GPIO_BSRR_BS13;						/* LED gets OFF showing system is busying communicating with server */

			rfidBuffer[rfidIndex] = '\0';						/* Making rfidBuffer as a string */
			rfidIndex = 0;										/* Reset rfidBufIndex */

			timerOFF();											/* Disable TIM4 */
			uart1InterruptRxDisable();							/* Disable USART1 Interrupt */
			USART_Cmd(USART1,DISABLE);							/* Disable USART1 */

			/* Display message to the user to wait*/
			clearLcd();
			lcdCursorSet(0,0);									/* Change cursor to line 0 */
			lcd_send_string("Please wait...");

			if(driverTripFlag == 0)
			{
				mSec = 0;										/* Reset the update time counter */
				if(!driverAuthenticate())						/* Calling driverAuthenticate API to validate the driver */
				{
					clear = 1;									/* to display GPRS connection failure */
					uart1InterruptRxEnable();					/* Enable Receive Interrupt Enable for UART1 */
					USART_Cmd(USART1,ENABLE);					/* Enable USART1 for further RFID read */
					USART_ReceiveData(USART1);					/* Dummy read to end the required interrupt */
					resetBuffer(rfidBuffer, 13);				/* Reset rfidBuffer */

					/* Display message to the driver on authentication failure */
					clearLcd();
					lcdCursorSet(1,3);							/* Change cursor to line 1 */
					lcd_send_string("Authentication");
					lcdCursorSet(2,6);							/* Change cursor to line 2 */
					lcd_send_string("Failed !");
					lcd_msDelay(3000);

					GPIOC->BSRR |= GPIO_BSRR_BR13;				/* LED gets ON showing system is idle for input */
					return;										/* Exiting ISR Immediately */
				}
				strcpy(driverRfid,rfidBuffer);					/* save rfidBuffer into driverRfid */
				/* Call to driverStart API */
				if(!driverStart())								/* starting Driver Service */
				{
					clear = 1;									/* to display GPRS connection failure */
					uart1InterruptRxEnable();					/* Enable Receive Interrupt Enable for UART1 */
					USART_Cmd(USART1,ENABLE);					/* Enable USART1 for further RFID read */
					USART_ReceiveData(USART1);					/* Dummy read to end the required interrupt */
					resetBuffer(rfidBuffer, 13);				/* Reset rfidBuffer */

					/* Display message to the driver on server error */
					clearLcd();
					lcdCursorSet(0,0);							/* Change cursor to line 0 */
					lcd_send_string("Error! Try Again");
					lcd_msDelay(2000);

					GPIOC->BSRR |= GPIO_BSRR_BR13;				/* LED gets ON showing system is idle for input */
					return;										/* Exiting ISR Immediately */
				}
				timerON();										/* Enable timer 4 only if successful driver start*/

				/* Display message to the driver on successful entry*/
				clearLcd();
				lcdCursorSet(1,2);								/* Change cursor to line 1 */
				lcd_send_string("Driver has been");
				lcdCursorSet(2,3);								/* Change cursor to line 2 */
				lcd_send_string("Authenticated");
				lcd_msDelay(3000);
				clear = 1;
			}
			else if(riderTripFlag == 0)
			{
				mSec = 0;										/* Reset the update time counter */
				if(strcmp(rfidBuffer,driverRfid) == 0)			/* For rfidBuffer == driverRfid */
				{
					/* Call to driverEnd API */
					if(!driverEnd())
					{
						clear = 1;								/* to display GPRS connection failure */
						timerOFF();								/* Disable timer 4 even if unsuccessful driver end*/
						uart1InterruptRxEnable();				/* Enable Receive Interrupt Enable for UART1 */
						USART_Cmd(USART1,ENABLE);				/* Enable USART1 for further RFID read */
						USART_ReceiveData(USART1);				/* Dummy read to end the required interrupt */
						resetBuffer(rfidBuffer, 13);			/* Reset rfidBuffer */

						/* Display message to the driver on server error */
						clearLcd();
						lcdCursorSet(0,0);							/* Change cursor to line 0 */
						lcd_send_string("Error! Try Again");
						lcd_msDelay(2000);

						GPIOC->BSRR |= GPIO_BSRR_BR13;			/* LED gets ON showing system is idle for input */
						return;									/* Exiting ISR Immediately */
					}
					timerOFF();									/* Disable timer 4 on successful driver end*/

					/* Display message to the driver on successful exit*/
					clearLcd();
					lcdCursorSet(1,5);							/* Change cursor to line 1 */
					lcd_send_string("Thank You!");
					lcdCursorSet(2,0);							/* Change cursor to line 2 */
					lcd_send_string("Distance(km) ");
					lcd_send_string(distanceCovered);			/* Display distance traveled */
					resetBuffer(distanceCovered, 10);			/* Reset distanceCovered Buffer */
					lcd_msDelay(5000);
					clear = 1;
				}
				else
				{
					strcpy(riderRfid[0],rfidBuffer);			/* save rfidBuffer into riderRfid[0] */
					riderIdPos = 0;								/* Initial value of rider array Id index */
					/* Call to riderStart API */
					if(!riderStart())
					{
						clear = 1;								/* to display GPRS connection failure */
						timerON();								/* Enable timer 4 even if unsuccessful rider start*/
						uart1InterruptRxEnable();				/* Enable Receive Interrupt Enable for UART1 */
						USART_Cmd(USART1,ENABLE);				/* Enable USART1 for further RFID read */
						USART_ReceiveData(USART1);				/* Dummy read to end the required interrupt */
						resetBuffer(rfidBuffer, 13);			/* Reset rfidBuffer */

						/* Display message to the driver on server error */
						clearLcd();
						lcdCursorSet(0,0);							/* Change cursor to line 0 */
						lcd_send_string("Error! Try Again");
						lcd_msDelay(2000);

						GPIOC->BSRR |= GPIO_BSRR_BR13;			/* LED gets ON showing system is idle for input */
						return;									/* Exiting ISR Immediately */
					}
					timerON();									/* Enable timer 4 on successful rider start*/

					/* Display message to the rider on successful entry*/
					clearLcd();
					lcdCursorSet(1,2);							/* Change cursor to line 1 */
					lcd_send_string("Enjoy your ride");
					lcdCursorSet(2,6);							/* Change cursor to line 2 */
					lcd_send_string("with us");
					lcd_msDelay(4000);
					clear = 1;
				}
			}
			else if(strcmpArr(rfidBuffer,riderRfid) == 0)		/* check If rfidBuffer is inside riderRfid array?  */
			{
				mSec = 0;										/* Reset the update time counter */
				/* Call to riderEnd API */
				if(!riderEnd())
				{
					clear = 1;									/* to display GPRS connection failure */
					timerON();									/* Enable timer 4 even if unsuccessful rider end*/
					uart1InterruptRxEnable();					/* Enable Receive Interrupt Enable for UART1 */
					USART_Cmd(USART1,ENABLE);					/* Enable USART1 for further RFID read */
					USART_ReceiveData(USART1);					/* Dummy read to end the required interrupt */
					resetBuffer(rfidBuffer, 13);				/* Reset rfidBuffer */

					/* Display message to the driver on server error */
					clearLcd();
					lcdCursorSet(0,0);							/* Change cursor to line 0 */
					lcd_send_string("Error! Try Again");
					lcd_msDelay(2000);

					GPIOC->BSRR |= GPIO_BSRR_BR13;				/* LED gets ON showing system is idle for input */
					return;										/* Exiting ISR Immediately */
				}
				timerON();										/* Enable timer 4 on successful rider end*/

				/* Display message to the rider on successful exit*/
				clearLcd();
				lcdCursorSet(1,2);								/* Change cursor to line 1 */
				lcd_send_string("Have a Good Day!");
				lcdCursorSet(2,0);								/* Change cursor to line 2 */
				lcd_send_string("Distance(km) ");
				lcd_send_string(distanceCovered);				/* Display distance traveled */
				resetBuffer(distanceCovered, 10);				/* Reset distanceCovered Buffer */
				lcd_msDelay(5000);
				clear = 1;
			}
			else if(riderTripFlag < 3)
			{
				mSec = 0;										/* Reset the update time counter */
				if(strcmp(rfidBuffer,driverRfid) == 0)			/* For rfidBuffer == driverRfid */
				{
					/*Generate an ErrorCode for Error Handler to display appropriate message in LCD*/
					timerON();										/* Enable timer 4 */

					/* Display warning message */
					clearLcd();
					lcdCursorSet(0,0);								/* Change cursor to line 0 */
					lcd_send_string("Please End");
					lcdCursorSet(1,0);								/* Change cursor to line 1 */
					lcd_send_string("On-going Ride");
					lcd_msDelay(3000);
					clear = 1;
				}
				else
				{
					riderIdPos = sortriderArr(riderRfid);			/* Find the position for saving new Rider Tag ID */
					if(riderIdPos >= 3)
					{
						/**** Error Handling for Debugging ****/
						/* Display message to the driver on server error */
						clearLcd();
						lcdCursorSet(0,0);							/* Change cursor to line 0 */
						lcd_send_string("Error!");
						lcd_msDelay(2000);
						return;
					}
					strcpy(riderRfid[riderIdPos],rfidBuffer);			/* save rfidBuffer into riderRfid[0] */
					/* Call to riderStart API */
					if(!riderStart())
					{
						clear = 1;								/* to display GPRS connection failure */
						timerON();								/* Enable timer 4 even if unsuccessful rider start*/
						uart1InterruptRxEnable();				/* Enable Receive Interrupt Enable for UART1 */
						USART_Cmd(USART1,ENABLE);				/* Enable USART1 for further RFID read */
						USART_ReceiveData(USART1);				/* Dummy read to end the required interrupt */
						resetBuffer(rfidBuffer, 13);			/* Reset rfidBuffer */

						/* Display message to the driver on server error */
						clearLcd();
						lcdCursorSet(0,0);							/* Change cursor to line 0 */
						lcd_send_string("Error! Try Again");
						lcd_msDelay(2000);

						GPIOC->BSRR |= GPIO_BSRR_BR13;			/* LED gets ON showing system is idle for input */
						return;									/* Exiting ISR Immediately */
					}
					timerON();									/* Enable timer 4 on successful rider start*/

					/* Display message to the rider on successful entry*/
					clearLcd();
					lcdCursorSet(1,2);							/* Change cursor to line 1 */
					lcd_send_string("Enjoy your ride");
					lcdCursorSet(2,6);							/* Change cursor to line 2 */
					lcd_send_string("with us");
					lcd_msDelay(4000);
					clear = 1;
				}
			}
			else
			{
				/*Generate an ErrorCode for Error Handler to display appropriate message in LCD*/
				timerON();										/* Enable timer 4 */

				/* Display warning message */
				clearLcd();
				lcdCursorSet(0,0);								/* Change cursor to line 0 */
				lcd_send_string("Please End");
				lcdCursorSet(1,0);								/* Change cursor to line 1 */
				lcd_send_string("On-going Ride");
				lcd_msDelay(3000);
				clear = 1;
			}

			if(USART_GetITStatus(USART1,USART_IT_RXNE))
			{
				USART_ReceiveData(USART1);						/* Dummy read to end the required interrupt */
			}
			uart1InterruptRxEnable();							/* Enable Receive Interrupt Enable for UART1 */
			USART_Cmd(USART1,ENABLE);							/* Enable USART1 for further RFID read */
			USART_ReceiveData(USART1);							/* Dummy read to end the required interrupt */
			resetBuffer(rfidBuffer, 13);						/* Reset rfidBuffer */

			rfidIndex = 0;										/* Reset rfidBufIndex */
			GPIOC->BSRR |= GPIO_BSRR_BR13;						/* LED gets ON showing system is idle for input */
		}
	}
}

