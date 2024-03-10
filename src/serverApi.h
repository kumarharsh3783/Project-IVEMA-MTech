/*
 * serverApi.h
 *
 *  Created on: 04-Mar-2021
 *      Author: Kumar Harsh
 */

#ifndef SERVERAPI_H_
#define SERVERAPI_H_

/**
 * Include Files
 */
#include "stm32f10x.h"
#include "external.h"
#include "string.h"
#include "lcd.h"
#include "eeprom.h"

/**
 * Typedef and Macro Definitions
 */
#define 	SIM808_RESPONSE_SIZE		200
#define		SERVER_RESPONSE_SIZE		1700
#define		RESPONSECODE_SIZE			4
#define		JWT_TOKEN_SIZE				450

#define 	LAT_SIZE					11
#define 	LON_SIZE					12

#define 	DIST_SIZE					10

typedef enum{Fail,Success} sysResponse;

/**
 * User-defined Variables
 */
char sim808Response[SIM808_RESPONSE_SIZE];	/* To store the SIM808 module response */
char serverResponse[SERVER_RESPONSE_SIZE];	/* To store the server response */
char responseCode[RESPONSECODE_SIZE];		/* To store the server response code */
char jwtToken[JWT_TOKEN_SIZE];				/* To store the jwtToken received from server */

char latitude[LAT_SIZE];
char longitude[LON_SIZE];

char distanceCovered[DIST_SIZE];

/**
 * User-defined Function Declarations
 */
sysResponse systemResponseCheck(char*,char*);
void commandConcat(char*,char*,char*);
void searchStringFunc(char*,char*,int,char*);
void saveKey_Val(int*,char*,int,int*,char*,int);

/* GPS related functions Declarations */
void connectGps(void);
void extractGpsCoordinates(void);			/* Method to extract the GPS coordinates */
void disconnectGps(void);

/* GPRS related functions Declarations */
int startGprs(void);
void extractServerResponse(int,char);		/* Method to receive server response till '}' n number of times */
void resetBuffer(char*,int);				/* Method to reset buffer with NULL characters */
int sortriderArr(char(*)[13]);

/* APIs Declarations */
int driverAuthenticate(void);
int driverStart(void);
int driverUpdate(void);
int driverEnd(void);
int riderStart(void);
int riderEnd(void);

#endif /* SERVERAPI_H_ */
