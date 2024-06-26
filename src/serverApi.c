/*
 * serverApi.c
 *
 *  Created on: 04-Mar-2021
 *      Author: Kumar Harsh
 */

/*<-- Includes -->*/
#include "serverApi.h"

/*Private User-defined variables start*/

sysResponse status;

/* Default Domain Name of the server */
char domainName[30] = "api.thingspeak.com";
/* serverConnect is 60 bytes long to adjust concatenated command string assuming domain name is less than 30 chars */
char serverConnect[80] = "AT+CIPSTART=\"TCP\",\"";

/*Private User-defined variables end*/

/**
 * Brief:	Validate the received response from the sim808 module
 * Param:	(pointer to a char) sim808Response - received string to be compared
 *			(pointer to a char) expectedResponse - to be compared with. Correct response expected.
 * Return:	(User-defined enum variable) status - Fail/Success
 */
sysResponse systemResponseCheck(char* sim900Response,char* expectedResponse)
{
	/* String Comparison Code */
	if((strcmp(sim900Response,expectedResponse)) == 0)
	{
		status = Success;			/* If correct response received return 1 */
	}
	else
	{
		status = Fail;				/* If error response received return 0 */
	}
	return status;						/* return whether expected response from module is received or not */
}

/**
 * Brief:	Concatenate strings in between.
 * Param:	(pointer to char) str1 is first sequence of command
 * 			(pointer to char) str2 is to be inserted part of command
 * 			(pointer to char) str3 is remaining sequence of command from str1
 * Retval:	(character pointer) str1
 */
void commandConcat(char* str1,char* str2,char* str3)
{
	strcat(str1,str2);					/* Concatenate str1+str2 into str1 */
	strcat(str1,str3);					/* Concatenate str1+str3 into str1 */
}

/**
 * Brief:	Reset buffer with NULL characters
 * Param:	(pointer to char array) buffer: constant pointer giving array address
 * 			(integer) bufferSize: size of the buffer to be reset with NULL characters
 * Return:	none
 */
void resetBuffer(char* buffer, int bufferSize)
{
	int bufferIndex;
	for(bufferIndex = 0; bufferIndex < bufferSize; bufferIndex++)
	{
		buffer[bufferIndex] = 0;								/* Resetting Buffer */
	}
}

/**Brief:	 Search a specified Key and Extract its value
 * METHOD-1	(Recommended to be used for searching string)...
 * Param: 	(char pointer)string - string buffer
 * 		 	(char pointer)searchStr - Key string defined as char searchStr[]
 * 		 	nPrint: number of characters to be printed/saved
 * 		 	(pointer to a char) - contains address where result of the search is stored
 * Return: 	none
 */
void searchStringFunc(char* str,char* searchStr,int nPrint,char* parsedString)
{
    int stringIndex, searchStrIndex = 0, parsedStrIndex = 0;
    int compare = 0, printVal = 0, matchFlag = 0;
    for(stringIndex = 0;str[stringIndex] != '\0';stringIndex++)
    {
        if(searchStr[searchStrIndex] != '\0')
        {
            if(str[stringIndex] == searchStr[searchStrIndex])
            {
                searchStrIndex++;       /*As first char is matched, increased index of searchStr for subsequent matching*/
            }
            else
            {
                searchStrIndex = 0;     /*Reset the search string index to continue searching the Key*/
            }
        }
        else if(searchStr[searchStrIndex] == '\0' && compare == 0)
        {
            matchFlag = 1;              /*Raise matchFlag if searchStr found*/
            compare = 1;
            printVal = 1;               /*Enable saving the Key's Value*/
            saveKey_Val(&nPrint,str,stringIndex,&printVal,parsedString,parsedStrIndex);   /*to continue processing main buffer with the Key's Value printing along side*/
            parsedStrIndex++;
        }
        else if(printVal == 1)
        {
            saveKey_Val(&nPrint,str,stringIndex,&printVal,parsedString,parsedStrIndex);
            parsedStrIndex++;
        }
    }
    if(matchFlag == 0)
    {
        //printf("\nSearch String Not Found!!\n");
    	/*Generate an ErrorCode for Error Handler to display appropriate message in LCD*/
    }
}

/**Brief: 	Saving key's value
 * Param: 	(pointer to an integer)nPtr: Pointer to number of characters to be printed
 * 			(pointer to a char array)str:  contains the serverResponse buffer
 * 			(integer) stringIndex: contains the current executing serverResponse array index
 * 			(pointer to an integer) printVal: contains the status of save flag
 * 			(pointer to a char array) parsedStr: contains the char array address where extracted value is to be stored
 * 			(integer) parsedStrIndex: contains the incrementing index for parsedStr array
 * Return: 	none
 */
void saveKey_Val(int* nPtr,char* str,int stringIndex,int* printVal,char* parsedStr,int parsedStrIndex)
{
	if(*nPtr != 0 && *nPtr < 100)                       /* For Normal Searches of Keys */
	{
		//printf("%c",str[stringIndex]);
		parsedStr[parsedStrIndex] = str[stringIndex];	/* saving into parsedStr */
		(*nPtr)--;
	}
	else if(*nPtr >= 100)                               /* For jwtToken search and save */
	{
		if(str[stringIndex] != '"')
		{
		   parsedStr[parsedStrIndex] = str[stringIndex];/* saving into parsedStr */
		}
		else
		{
			*nPtr = 0;                                  /* To stop the save operation for jwtToken */
		}
	}
	else
	{
		*printVal = 2;               					/*Disable printing the Key's Value after printing*/
		parsedStr[parsedStrIndex] = '\0';				/*End the parsed String with a null character*/
	}
}

/**
 * Brief:	Configure the module to start GPRS connectivity with the server
 * Param:	none
 * Return:	(int) 0: unsuccessful start of GPRS
 * 				  1: successful start of GPRS
 */
int startGprs()
{
	uartSendData("AT+CREG?\r\n");		/* Network registration status */
	uartReceiveData(sim900Response,4);
	uartSendData("AT+CGATT=1\r\n");		/* Attached to GPRS service */
	uartReceiveData(sim900Response,2);
	/* waiting to get attached to GPRS */
	delay_in_ms(2000);
	uartSendData("AT+CIPSHUT\r\n");		/* Deactivate GPRS PDP Context */
	uartReceiveData(sim900Response,2);
	uartSendData("AT+CIPMUX=0\r\n");	/* Single TCP/IP connection mode */
	uartReceiveData(sim900Response,2);
	uartSendData("AT+CGDCONT=1,\"IP\",\"airtelgprs.com\"\r\n");
	uartReceiveData(sim900Response,2);
	uartSendData("AT+CSTT=\"airtelgprs.com\"\r\n"); /* Start task and set APN */
	uartReceiveData(sim900Response,2);
	uartSendData("AT+CIICR\r\n");		/* Bring up wireless connection with GPRS */
	uartReceiveData(sim900Response,2);
	uartSendData("AT+CIFSR\r\n");		/* Get local IP address */
	uartReceiveData(sim900Response,2);
	uartSendData("AT+CIPSTATUS\r\n");	/* Query current connection status */
	uartReceiveData(sim900Response,4);	/* Receive sim808 multiple line response at sim808Response string */
	if(systemResponseCheck(sim900Response,"OKSTATE: IP STATUS") == Fail)	/* Check if OK is received */
	{
		/*Generate an ErrorCode for Error Handler to display appropriate message in LCD*/
		return 0;
	}
	commandConcat(serverConnect,domainName,"\",\"80\"\r\n");	/* Concatenate the command to connect with server */
	uartSendData(serverConnect);		/* Connect to server */
	uartReceiveData(sim900Response,4);	/* Receive sim808 multiple line response at sim808Response string */
	if(systemResponseCheck(sim900Response,"OKCONNECT OK") == Fail)	/* Check if OK is received */
	{
		/*Generate an ErrorCode for Error Handler to display appropriate message in LCD*/
		return 0;
	}
	resetBuffer(serverConnect,80);					/* Reset with NULL chars */
	strcpy(serverConnect,"AT+CIPSTART=\"TCP\",\"");	/* Reset the serverConnect command */
	uartSendData("AT+CIPSEND\r\n");		/* To send the data to server using POST or GET method*/
	/* check for the editor start character */
	uartReceiveData(sim900Response,1);
	while(!USART_GetFlagStatus(USART1, USART_FLAG_RXNE));	/* wait till receive data buffer is empty */
	if(!(USART_ReceiveData(USART1)=='>'))
	{
		return 0;
	}
	return 1;
}

/**
 * Brief:	API to send data to server
 * Param:	none
 * Return:	0: Unsuccessful 1: Successful Event
 */
int sendData_toServer()
{
	/**** Activate the GPRS Module Once Just after Echo Off ****/

	uartSendData("ATE0\r\n");			/* Initiate echo OFF SIM900 */
	uartReceiveData(sim900Response,2);	/* Receive sim900 module response at sim900Response string */
	uartSendData("AT\r\n");				/* Initiate AT commands for SIM900 */
	uartReceiveData(sim900Response,2);	/* Receive sim900 module response at sim900Response string */
	uartSendData("AT\r\n");				/* Initiate AT commands for SIM900 */
	uartReceiveData(sim900Response,2);	/* Receive sim900 module response at sim900Response string */
	if(systemResponseCheck(sim900Response,"OK") == Fail)	/* Check if OK is received */
	{
		/*Generate an ErrorCode for Error Handler to display appropriate message in LCD*/
		errorMsg1();
		return 0;
	}

	/****************************************************************************/

	if(!startGprs())											/* Connect to the server */
	{
		errorMsg();
		return 0;												/* Try again */
	}
	/* API Header */
	uartSendData("GET https://api.thingspeak.com/update?api_key=BD3E8K91S4T4NIH7&field1=");

	/* API Body */
	uartSendData(tempDataStr);
	uartSendData("&field2=");
	uartSendData(mq135DataStr);
	uartSendData("&field3=");
	uartSendData(mq7DataStr);

	/* New Line and Carriage return */
	uartSendData("\r\n");

	/* sending CTRL+Z to send */
	USART_SendData(USART1, (uint16_t)26);
	while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));	/* wait till TDR is empty */

	/* with ATE1 i.e. echo on, serverResponse will also capture API body which already contains '}', so with ATE1, param: '}' in API body + '}' expected in server response */
	/* with ATE0 i.e. echo off, param:	'}' expected in server response alone */
	uartReceiveData(sim900Response,5);
	searchStringFunc(sim900Response,"CLOSE",1,responseCode); /* extract the server response code */
	if(systemResponseCheck(responseCode,"D") == Fail)
	{
		/*Generate an ErrorCode for Error Handler to display appropriate message in LCD*/
		errorMsg();
		return 0;
	}

	delay_in_ms(1000);

	/* Close the connection */
	uartSendData("AT+CIPSHUT\r\n");
	uartReceiveData(sim900Response,2);

	/* Reset sim900Response Buffer with NULL characters */
	resetBuffer(sim900Response,SIM900_RESPONSE_SIZE);
	/* Reset responseCode Buffer with NULL characters */
	resetBuffer(responseCode,RESPONSECODE_SIZE);

	return 1;
}
