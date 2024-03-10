/*
 * serverApi.c
 *
 *  Created on: 04-Mar-2021
 *      Author: Kumar Harsh
 */

/*<-- Includes -->*/
#include "serverApi.h"

/*Private User-defined variables start*/
sysResponse status, gpsFixStatus;
char domainName[30] = "savy-snp.herokuapp.com"; 	/* Default Domain Name of the server */
char serverConnect[80] = "AT+CIPSTART=\"TCP\",";	/* serverConnect is 60 bytes long to adjust concatenated command string assuming domain name is less than 30 chars */
/*Private User-defined variables end*/

/**
 * Brief:	Validate the received response from the sim808 module
 * Param:	(pointer to a char) sim808Response - received string to be compared
 *			(pointer to a char) expectedResponse - to be compared with. Correct response expected.
 * Return:	(User-defined enum variable) status - Fail/Success
 */
sysResponse systemResponseCheck(char* sim808Response,char* expectedResponse)
{
	/* String Comparison Code */
	if((strcmp(sim808Response,expectedResponse)) == 0)
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
 * Brief:	Method to sort riderRfid array on successful exit of rider and returns present value of existing riders
 * Param:	(pointer to a 2-D char array) (*strarr)[13] contains the address of riderRfid Array
 * Return:	onedarrayIndex returns the existing available slot for saving new rider
 * 			If returns 3, no slot available. Range: (onedarrayIndex < 3)
 */
int sortriderArr(char (*strarr)[13])
{
    int onedarrayIndex = 0;
    if(strarr[0][0] == 0)
    {
        /**** Sorting the array ****/
        strcpy(strarr[0],strarr[1]);
        strcpy(strarr[1],strarr[2]);
        resetBuffer(strarr[2],13);
    }
    else if(strarr[1][0] == 0)
    {
        /**** Sorting the array ****/
        strcpy(strarr[1],strarr[2]);
        resetBuffer(strarr[2],13);
    }
    /**** search for available slot and returns value of immediate slot for save ****/
    while(strarr[onedarrayIndex][0] != 0 && onedarrayIndex < 3)
    {
        onedarrayIndex++;
    }
    return onedarrayIndex;
}

/**
 * Brief:	Receive the server response and save into RAM for further use
 * Param:	(integer) endCharCount contains the number of '}' in reception of response till which response is read
 * 			(character) indicationChar contains the indicating character for knowing the wait time to read for server response
 * Retval:	none
 */
void extractServerResponse(int endCharCount, char indicationChar)
{
	uint16_t rBuf;
	int serverResponseIndex = 0;

	TIM_Cmd(TIM2, ENABLE);											/* Enable Timer 2 for timeout feature */

	/* Receive the response till n times of '}' is read */
	while(endCharCount != 0)
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
		/* Detecting closing braces (n times) for end of server response */
		if(rBuf == indicationChar)
		{
			endCharCount--;
		}
		serverResponse[serverResponseIndex] = rBuf;				/* save into server response buffer */
		serverResponseIndex++;
	}
	TIM_Cmd(TIM2, DISABLE);										/* Disable Timer 2 for timeout feature */
	TIM2->CNT = 0;
	timeout = 0;												/* Reset timeout */
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
 * Brief:	Extract GPS coordinates when valid GPS data is incoming
 * Param:	none
 * Return:	none
 */
void extractGpsCoordinates()
{
	uint8_t gpsIndex, countComma = 0, locIndex = 0;
	gpsFixStatus = Fail;				/* Default GPS fix status */

	for(gpsIndex = 0;sim808Response[gpsIndex] != '\0';gpsIndex++)
	{
		if(sim808Response[gpsIndex] == ',')
		{
			countComma++;				/* counting commas in RMC extraction string */
		}
		/* check for fix status */
		if((countComma == 1) && (sim808Response[gpsIndex] != ','))
		{
			if(sim808Response[gpsIndex] == '0')
			{
				gpsFixStatus = Fail;	/* Invalid status flag set */
			}
			else if(sim808Response[gpsIndex] == '1')
			{
				gpsFixStatus = Success; /* Valid status flag set */
			}
		}
		/* extract locations only when gps status is fixed */
		if(gpsFixStatus == Success)
		{
			/* wait till correct placement of latitude in RMC extraction string*/
			if((countComma == 3) && (sim808Response[gpsIndex] != ','))
			{
				/* saving latitude */
				latitude[locIndex] = sim808Response[gpsIndex];
				locIndex++;
			}
			else if((countComma == 4) && (sim808Response[gpsIndex] == ','))
			{
				latitude[locIndex] = '\0';	/* Adding NULL char will make latitude as a string */
				locIndex = 0;			/* Reset the location Index to save longitude next */
			}
			else if((countComma == 4) && (sim808Response[gpsIndex] != ','))
			{
				/* saving longitude */
				longitude[locIndex] = sim808Response[gpsIndex];
				locIndex++;
			}
			else if((countComma == 5) && (sim808Response[gpsIndex] == ','))
			{
				longitude[locIndex] = '\0';	/* Adding NULL char will make longitude as a string */
				locIndex = 0;			/* Reset the location Index */
			}
		}
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
	uartReceiveData(sim808Response,4);
	uartSendData("AT+CGATT=1\r\n");		/* Attached to GPRS service */
	uartReceiveData(sim808Response,2);
	uartSendData("AT+CIPSHUT\r\n");		/* Deactivate GPRS PDP Context */
	uartReceiveData(sim808Response,2);
	uartSendData("AT+CIPMUX=0\r\n");	/* Single TCP/IP connection mode */
	uartReceiveData(sim808Response,2);
	uartSendData("AT+CGDCONT=1,\"IP\",\"airtelgprs.com\"\r\n");
	uartReceiveData(sim808Response,2);
	uartSendData("AT+CSTT=\"airtelgprs.com\"\r\n"); /* Start task and set APN */
	uartReceiveData(sim808Response,2);
	uartSendData("AT+CIICR\r\n");		/* Bring up wireless connection with GPRS */
	uartReceiveData(sim808Response,2);
	uartSendData("AT+CIFSR\r\n");		/* Get local IP address */
	uartReceiveData(sim808Response,2);
	uartSendData("AT+CIPSTATUS\r\n");	/* Query current connection status */
	uartReceiveData(sim808Response,4);	/* Receive sim808 multiple line response at sim808Response string */
	if(systemResponseCheck(sim808Response,"OKSTATE: IP STATUS") == Fail)	/* Check if OK is received */
	{
		/*Generate an ErrorCode for Error Handler to display appropriate message in LCD*/
		return 0;
	}
	commandConcat(serverConnect,domainName,",\"80\"\r\n");	/* Concatenate the command to connect with server */
	uartSendData(serverConnect);		/* Connect to server */
	uartReceiveData(sim808Response,4);	/* Receive sim808 multiple line response at sim808Response string */
	if(systemResponseCheck(sim808Response,"OKCONNECT OK") == Fail)	/* Check if OK is received */
	{
		/*Generate an ErrorCode for Error Handler to display appropriate message in LCD*/
		return 0;
	}
	resetBuffer(serverConnect,80);					/* Reset with NULL chars */
	strcpy(serverConnect,"AT+CIPSTART=\"TCP\",");	/* Reset the serverConnect command */
	uartSendData("AT+CIPSEND\r\n");		/* To send the data to server using POST or GET method*/
	/* check for the editor start character */
	uartReceiveData(sim808Response,1);
	while(!USART_GetFlagStatus(USART2, USART_FLAG_RXNE));	/* wait till receive data buffer is empty */
	while(!(USART_ReceiveData(USART2)=='>'));
	msDelay(10000);
	return 1;
}

/**
 * Brief:	Configure the module to connect GPS for satellite triangulation on the system
 * Param:	none
 * Return:	none
 */
void connectGps()
{
	/* Display message for User*/
	clearLcd();
	lcdCursorSet(0,0);								/* Change cursor to line 0 */
	lcd_send_string("Connecting GPS..");

	uartSendData("AT+CGNSPWR=1\r\n");						/* Power On GPS */
	uartReceiveData(sim808Response,2);						/* Receive sim808 response */
	uartSendData("AT+CGNSSEQ=\"RMC\"\r\n");						/* Get and Set only for RMC */
	uartReceiveData(sim808Response,2);						/* Receive sim808 response */
	do
	{
		uartSendData("AT+CGPSSTATUS?\r\n");					/* Get GNSS FIX*/
		uartReceiveData(sim808Response,2);
		uartSendData("AT+CGNSINF\r\n");						/* Get GNSS Information */
		uartReceiveData(sim808Response,4);
		extractGpsCoordinates();							/* To save GPS locations once GPS FIX Status: 1 */
		msDelay(6000);
	}while(gpsFixStatus == Fail);							/* Try until valid GPS locations come */

	/* Display message for User*/
	clearLcd();
	lcdCursorSet(0,0);								/* Change cursor to line 0 */
	lcd_send_string("GPS Connected !");
	lcdCursorSet(1,0);								/* Change cursor to line 1 */
	lcd_send_string("Wait! Updating..");
}

/**
 * Brief:	Power Off GNSS in sim808 Module
 * Param:	none
 * Return:	none
 */
void disconnectGps()
{
	uartSendData("AT+CGPSPWR=0\r\n");						/* Power Off GPS */
	uartReceiveData(sim808Response,2);						/* Receive sim808 response */
	uartSendData("AT+CGNSPWR=0\r\n");						/* Power Off GNSS */
	uartReceiveData(sim808Response,2);						/* Receive sim808 response */
}

/**
 * Brief:	API to check for the authenticity of the driver with the Hardware ID
 * Param:	none
 * Return:	0: Unsuccessful 1: Successful Event
 */
int driverAuthenticate()
{
	/**** Activate the GPRS Module Once at the time of driver authentication ****/

	uartSendData("ATE0\r\n");			/* Initiate echo OFF SIM808 */
	uartReceiveData(sim808Response,2);	/* Receive sim808 module response at sim808Response string */
	uartSendData("AT\r\n");				/* Initiate AT commands for SIM808 */
	uartReceiveData(sim808Response,2);	/* Receive sim808 module response at sim808Response string */
	if(systemResponseCheck(sim808Response,"OK") == Fail)	/* Check if OK is received */
	{
		/*Generate an ErrorCode for Error Handler to display appropriate message in LCD*/
		return 0;
	}

	/****************************************************************************/

	if(!startGprs())											/* Connect to the server */
	{
		return 0;												/* Try again */
	}
	/* API Header */
	uartSendData("POST /authenticate HTTP/1.1\r\n");
	uartSendData("HOST: ");
	uartSendData(domainName);
	uartSendData("\r\nAccept: application/json\r\n");
	uartSendData("Content-Type: application/json\r\n");
	uartSendData("Content-Length: 83\r\n\r\n");
	/* API Body */
	uartSendData("{\"uniqueHardwareId\": \"3ea896e6-67e7-11eb-ae93-0242ac130002\",\"rfid\": \"");	/* To check with default set hardware ID */
	uartSendData(rfidBuffer);
	uartSendData("\"}\r\n"); //\n
	USART_SendData(USART2, (uint16_t)26);					/* sending CTRL+Z to send */
	while(!USART_GetFlagStatus(USART2, USART_FLAG_TXE));	/* wait till TDR is empty */

	/* with ATE1 i.e. echo on, serverResponse will also capture API body which already contains '}', so with ATE1, param: '}' in API body + '}' expected in server response */
	/* with ATE0 i.e. echo off, param:	'}' expected in server response alone */
	extractServerResponse(1,'}');								 /* receive server response into serverResponse string */
	searchStringFunc(serverResponse,"HTTP/1.1 ",3,responseCode); /* extract the server response code */
	if(systemResponseCheck(responseCode,"200") == Fail)
	{
		/*Generate an ErrorCode for Error Handler to display appropriate message in LCD*/
		return 0;
	}
	searchStringFunc(serverResponse,"\"jwtToken\":\"",100,jwtToken); /* extract the JSON web token */

	resetBuffer(serverResponse,SERVER_RESPONSE_SIZE);		/* Reset serverResponse Buffer with NULL characters */

	return 1;
}

/**
 * Brief:	API to start driver service with GPS location coordinates
 * Param:	none
 * Return:	0: Unsuccessful 1: Successful Event
 */
int driverStart()
{
	connectGps();											/* Connect to satellites for GPS coordinate triangulation */

	if(!startGprs())										/* Connect to the server */
	{
		return 0;											/* Try Again */
	}
	/* API Header */
	uartSendData("POST /secure/driver/start HTTP/1.1\r\n");
	uartSendData("HOST: ");
	uartSendData(domainName);
	uartSendData("\r\nAuthorization: Bearer ");
	uartSendData(jwtToken);									/* Sending JSON Token */
	uartSendData("\r\nAccept: application/json\r\n");
	uartSendData("Content-Type: application/json\r\n");
	uartSendData("Content-Length: 95\r\n\r\n");
	/* API Body */
	uartSendData("{ \"rfid\": \"");
	uartSendData(driverRfid);
	uartSendData("\", \"srcGeoLocation\": { \"latitude\": ");
	uartSendData(latitude);									/* Sending Latitude */
	uartSendData(", \"longitude\": ");
	uartSendData(longitude);								/* Sending Longitude */
	uartSendData(" } }\r\n");
	USART_SendData(USART2, (uint16_t)26);					/* sending CTRL+Z to send */
	while(!USART_GetFlagStatus(USART2, USART_FLAG_TXE));	/* wait till TDR is empty */

	extractServerResponse(1,'*');								 /* receive server response into serverResponse string */
	searchStringFunc(serverResponse,"HTTP/1.1 ",3,responseCode); /* extract the server response code */
	if(systemResponseCheck(responseCode,"200") == Fail)
	{
		if(systemResponseCheck(responseCode,"500") == Fail)		 /* To ignore server error showing driver has already started the trip */
		{
			/*Generate an ErrorCode for Error Handler to display appropriate message in LCD*/
			return 0;
		}
	}

	resetBuffer(serverResponse,SERVER_RESPONSE_SIZE);		/* Reset serverResponse Buffer with NULL characters */

	driverTripFlag = 1;										/* changing the flag status as driver has been authenticated and started*/

	return 1;
}

/**
 * Brief:	API to update system Location every fixed interval
 * Param:	none
 * Return:	0: Unsuccessful 1: Successful Event
 */
int driverUpdate()
{
	connectGps();											/* Connect to satellites for GPS coordinate triangulation */

	if(!startGprs())										/* Connect to the server */
	{
		return 0;											/* Try Again */
	}

	if(eepromHasData == 1)
	{
		DELAY_mSECS = 1;									/* Updation Time increased to 1 millisecond for sending EEPROM data */
		eeprom_seqRead(0xA0,page_addr_read,latitude); 		/* sequential read of EERPOM */
		latitude[8] = '\0';									/* adding Null to make it compatible to send via uart */
		page_addr_read += 8;
		lcd_msDelay(5);										/* 5ms delay to let the eeprom read from memory */
		eeprom_seqRead(0xA0,page_addr_read,longitude);		/* sequential read of EERPOM */
		longitude[8] = '\0';								/* adding Null to make it compatible to send via uart */
		page_addr_read += 8;
		lcd_msDelay(5);										/* 5ms delay to let the eeprom read from memory */

		/**** Check for EEPROM data has been sent already or not */
		if(page_addr_read != page_addr_write)
		{
			page_addr_read = page_addr_write = 0;					/* Reset the read/write page addresses */
			eepromHasData = 0;										/* Reset as all written data has been sent already */
			DELAY_mSECS = 10000;									/* Updation Time reset to default of 10 seconds for normal conditions */
		}
	}

	/* API Header */
	uartSendData("POST /secure/driver/update HTTP/1.1\r\n");
	uartSendData("HOST: ");
	uartSendData(domainName);
	uartSendData("\r\nAuthorization: Bearer ");
	uartSendData(jwtToken);									/* Sending JSON Token */
	uartSendData("\r\nAccept: application/json\r\n");
	uartSendData("Content-Type: application/json\r\n");
	uartSendData("Content-Length: 98\r\n\r\n");
	/* API Body */
	uartSendData("{ \"rfid\": \"");
	uartSendData(driverRfid);
	uartSendData("\", \"updateGeoLocation\": { \"latitude\": ");
	uartSendData(latitude);									/* Sending Latitude */
	uartSendData(", \"longitude\": ");
	uartSendData(longitude);								/* Sending Longitude */
	uartSendData(" } }\r\n");
	USART_SendData(USART2, (uint16_t)26);					/* sending CTRL+Z to send */
	while(!USART_GetFlagStatus(USART2, USART_FLAG_TXE));	/* wait till TDR is empty */

	extractServerResponse(1,'*');								 /* receive server response into serverResponse string */
	searchStringFunc(serverResponse,"HTTP/1.1 ",3,responseCode); /* extract the server response code */
	if(systemResponseCheck(responseCode,"200") == Fail)
	{
		/*Generate an ErrorCode for Error Handler to display appropriate message in LCD*/
		return 0;
	}

	resetBuffer(serverResponse,SERVER_RESPONSE_SIZE);		/* Reset serverResponse Buffer with NULL characters */

	return 1;
}

/**
 * Brief:	API to stop driver Service
 * Param:	none
 * Return:	0: Unsuccessful 1: Successful Event
 */
int driverEnd()
{
	connectGps();											/* Connect to satellites for GPS coordinate triangulation */
//	disconnectGps();										/* Disconnect GNSS */

	if(!startGprs())										/* Connect to the server */
	{
		return 0;											/* Try Again */
	}
	/* API Header */
	uartSendData("POST /secure/driver/end HTTP/1.1\r\n");
	uartSendData("HOST: ");
	uartSendData(domainName);
	uartSendData("\r\nAuthorization: Bearer ");
	uartSendData(jwtToken);									/* Sending JSON Token */
	uartSendData("\r\nAccept: application/json\r\n");
	uartSendData("Content-Type: application/json\r\n");
	uartSendData("Content-Length: 100\r\n\r\n");
	/* API Body */
	uartSendData("{ \"rfid\": \"");
	uartSendData(driverRfid);
	uartSendData("\", \"updateGeoLocation\": { \"latitude\": ");
	uartSendData(latitude);									/* Sending Latitude */
	uartSendData(", \"longitude\": ");
	uartSendData(longitude);								/* Sending Longitude */
	uartSendData(" } }\r\n");
	USART_SendData(USART2, (uint16_t)26);					/* sending CTRL+Z to send */
	while(!USART_GetFlagStatus(USART2, USART_FLAG_TXE));	/* wait till TDR is empty */

	/* with ATE1 i.e. echo on, serverResponse will also capture API body which already contains '}', so with ATE1, param: '}' in API body + '}' expected in server response */
	/* with ATE0 i.e. echo off, param:	'}' expected in server response alone */
	extractServerResponse(3,'}');								 /* receive server response into serverResponse string */
	searchStringFunc(serverResponse,"HTTP/1.1 ",3,responseCode); /* extract the server response code */
	if(systemResponseCheck(responseCode,"200") == Fail)
	{
		/*Generate an ErrorCode for Error Handler to display appropriate message in LCD*/
		return 0;
	}
	resetBuffer(distanceCovered,DIST_SIZE);					/* Reset before use */
	searchStringFunc(serverResponse,"\"disCovered\":",7,distanceCovered); /* extract distance covered from server response */

	resetBuffer(serverResponse,SERVER_RESPONSE_SIZE);		/* Reset serverResponse Buffer with NULL characters */
	resetBuffer(jwtToken,JWT_TOKEN_SIZE);					/* Reset jwtToken Buffer with NULL characters */
	resetBuffer(driverRfid,13);								/* Reset driverRfid with NULL characters */

	driverTripFlag = 0;										/* Reset driverTripFlag to indicate no driver inside */

	return 1;
}

/**
 * Brief:	API to start the trip for a rider
 * Param:	none
 * Return:	0: Unsuccessful 1: Successful Event
 */
int riderStart()
{
	if(!startGprs())										/* Connect to the server */
	{
		return 0;											/* Try Again */
	}
	/* API Header */
	uartSendData("POST /secure/rider/start HTTP/1.1\r\n");
	uartSendData("HOST: ");
	uartSendData(domainName);
	uartSendData("\r\nAuthorization: Bearer ");
	uartSendData(jwtToken);									/* Sending JSON Token */
	uartSendData("\r\nAccept: application/json\r\n");
	uartSendData("Content-Type: application/json\r\n");
	uartSendData("Content-Length: 63\r\n\r\n");
	/* API Body */
	uartSendData("{ \"riderRfid\": \"");
	uartSendData(riderRfid[riderIdPos]);
	uartSendData("\", \"driverRfid\": \"");
	uartSendData(driverRfid);
	uartSendData("\" }\r\n");
	USART_SendData(USART2, (uint16_t)26);					/* sending CTRL+Z to send */
	while(!USART_GetFlagStatus(USART2, USART_FLAG_TXE));	/* wait till TDR is empty */

	extractServerResponse(1,'*');								 /* receive server response into serverResponse string */
	searchStringFunc(serverResponse,"HTTP/1.1 ",3,responseCode); /* extract the server response code */
	if(systemResponseCheck(responseCode,"200") == Fail)
	{
		/*Generate an ErrorCode for Error Handler to display appropriate message in LCD*/
		return 0;
	}

	resetBuffer(serverResponse,SERVER_RESPONSE_SIZE);		/* Reset serverResponse Buffer with NULL characters */

	riderTripFlag += 1;										/* Flag is set to indicate rider inside */

	return 1;
}

/**
 * Brief:	API to end the trip for the existing rider
 * Param:	none
 * Return:	0: Unsuccessful 1: Successful Event
 */
int riderEnd()
{
	if(!startGprs())										/* Connect to the server */
	{
		return 0;											/* Try Again */
	}
	/* API Header */
	uartSendData("POST /secure/rider/end HTTP/1.1\r\n");
	uartSendData("HOST: ");
	uartSendData(domainName);
	uartSendData("\r\nAuthorization: Bearer ");
	uartSendData(jwtToken);									/* Sending JSON Token */
	uartSendData("\r\nAccept: application/json\r\n");
	uartSendData("Content-Type: application/json\r\n");
	uartSendData("Content-Length: 63\r\n\r\n");
	/* API Body */
	uartSendData("{ \"riderRfid\": \"");
	uartSendData(riderRfid[riderIdPos]);
	uartSendData("\", \"driverRfid\": \"");
	uartSendData(driverRfid);
	uartSendData("\" }\r\n");
	USART_SendData(USART2, (uint16_t)26);					/* sending CTRL+Z to send */
	while(!USART_GetFlagStatus(USART2, USART_FLAG_TXE));	/* wait till TDR is empty */

	/* with ATE1 i.e. echo on, serverResponse will also capture API body which already contains '}', so with ATE1, param: '}' in API body + '}' expected in server response */
	/* with ATE0 i.e. echo off, param:	'}' expected in server response alone */
	extractServerResponse(3,'}');								 /* receive server response into serverResponse string */
	searchStringFunc(serverResponse,"HTTP/1.1 ",3,responseCode); /* extract the server response code */
	if(systemResponseCheck(responseCode,"200") == Fail)
	{
		/*Generate an ErrorCode for Error Handler to display appropriate message in LCD*/
		return 0;
	}
	resetBuffer(distanceCovered,DIST_SIZE);					/* Reset before use */
	searchStringFunc(serverResponse,"\"disCovered\":",7,distanceCovered); /* extract distance covered from server response */

	resetBuffer(serverResponse,SERVER_RESPONSE_SIZE);		/* Reset serverResponse Buffer with NULL characters */

	resetBuffer(riderRfid[riderIdPos],13);					/* Reset riderRfid with NULL characters */
	sortriderArr(riderRfid);								/* Sort riderRfid array after exiting random existing rider */

	riderTripFlag -= 1;										/* Flag is reset to indicate no rider inside */

	return 1;
}


