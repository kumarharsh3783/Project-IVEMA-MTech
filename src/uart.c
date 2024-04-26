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
 * Brief:	Initialize USART1
 * Param:	none
 * Return:	none
 */
void uartInit()
{
	/* Clock to USART Peripherals */
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;			/*Clock Enabled in USART1 peripheral*/

	/* USART Initialization */
	USART_StructInit(USART_InitStruct);				/*Baud: 9600 bits per second*/

	/*Changing the Baud Rate from default 9600 to 57600*/
	changeBaud(BAUD_57600);

	/* Initializing USART 2 */
	USART_Init(USART1,USART_InitStruct);
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
 * Brief:	Transmit UART multiple data
 * Param:	(Pointer to a character) txBuffer - contains the string to be sent via UART
 * Return:	none
 */
void uartSendData(char* txBuffer)
{
	for(txIndex = 0; txBuffer[txIndex]!='\0'; txIndex++)
	{
		USART_SendData(USART1, (char)txBuffer[txIndex]);		//instruct to dump TDR value into shift register
		while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));	//wait till TDR is empty
	}
	while(!USART_GetFlagStatus(USART1, USART_FLAG_TC));			//wait till TC get set
	USART_ClearFlag(USART1, USART_FLAG_TC);						//clearing TC bit for next transmission frame
}

/**
 * Brief:	Receive UART data into receive buffer
 * Param:	(pointer to a char) Address where received response string is saved, (int) No. of lines to read
 * Return:	none
 */
void uartReceiveData(char* sim900Response,int lineCount)
{
	int responseIndex = 0;
	uint16_t rBuf;

	/* Reset sim900Response Buffer with NULL characters */
	resetBuffer(sim900Response,SIM900_RESPONSE_SIZE);

	sysTimeout = 0;												/* Default status */
	TIM_Cmd(TIM2, ENABLE);										/* Enable Timer 2 for timeout feature */

	while(lineCount != 0)
	{

		while(!USART_GetFlagStatus(USART1, USART_FLAG_RXNE))	/* wait till receive data buffer is empty */
		{
			if(sysTimeout == TIMEOUT_DETECTION)
			{
				TIM_Cmd(TIM2, DISABLE);							/* Disable Timer 2 for timeout feature */
				TIM2->CNT = 0;
				sysTimeout = 0;									/* Default status */
				return;
			}
		}
		rBuf = USART_ReceiveData(USART1);

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
			sim900Response[responseIndex] = rBuf;				/* save into sim900Response Buffer */
			responseIndex++;
		}
	}
	sim900Response[responseIndex] = '\0';						/* adding null char at the end of string */

	/* Disable Timer 2 for timeout feature */
	TIM_Cmd(TIM2, DISABLE);
	TIM2->CNT = 0;
	sysTimeout = 0;												/* Default status */
}

