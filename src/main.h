/*
 * main.h
 *
 *  Created on: 03-Mar-2021
 *      Author: Kumar Harsh
 */

#ifndef MAIN_H_
#define MAIN_H_

/**
 * Include files
 */
#include "stm32f10x.h"
#include "uart.h"
#include "timer.h"
#include "lcd.h"
#include "external.h"

/**
 * User-defined Variables
 */
char rfidBuffer[13] = "";				/* General use RFID buffer */
char driverRfid[13] = "";				/* Driver RFID Detail */
char riderRfid[3][13] = {"","",""};		/* Rider 01 RFID Detail */

int rfidIndex = 0;
uint8_t driverTripFlag = 0;		/* Indication for availability of driver inside the EV: 0 means No driver; 1 means driver inside */
uint8_t riderTripFlag = 0;		/* Indication for availability of rider inside the EV: 0 means No rider; 1 means rider inside */
int mSec = 0;
int timeout = 0;
uint8_t sysTimeout;
uint8_t clear = 0;
uint8_t eepromHasData = 0;

/**
 * User-defined Function Declarations
 */
void systemClockInit(void);
void gpioInit(void);
void Delay(long int);
void msDelay(int);

#endif /* MAIN_H_ */
