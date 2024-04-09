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
 * Brief:	Initialise USART2
 * Param:	none
 * Return:	none
 */
void uartInit()
{
	/* Clock to USART Peripherals */
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;			/*Clock Enabled in USART2 peripheral*/

	/* USART Initialisation */
	USART_StructInit(USART_InitStruct);				/*Baud: 9600 bits per second*/

	/*Changing the Baud Rate from default 9600 to 57600*/
	changeBaud(BAUD_57600);

	/* Initialising USART 2 */
	USART_Init(USART2,USART_InitStruct);
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

