/*
 * external.h
 *
 *  Created on: 07-Mar-2021
 *      Author: Kumar Harsh
 */

#ifndef EXTERNAL_H_
#define EXTERNAL_H_

/**
 * External variables and functions
 */
extern char rfidBuffer[13];		/* General use RFID buffer */
extern char driverRfid[13];		/* Driver RFID Detail */
extern char riderRfid[3][13];	/* Rider 01 RFID Detail */
extern int rfidIndex;
extern uint8_t driverTripFlag;
extern uint8_t riderTripFlag;
extern char latitude[11];
extern char longitude[12];
extern int mSec;
extern int DELAY_mSECS;
extern int timeout;
extern uint8_t sysTimeout;
extern char distanceCovered[10];
extern uint8_t clear;
extern uint8_t eepromHasData;
extern uint8_t page_addr_write;
extern uint8_t page_addr_read;
extern int riderIdPos;


extern void Delay(long int);
extern void msDelay(int);
extern void lcd_usDelay(int);
extern void lcd_msDelay(int);
extern void timerON();
extern void timerOFF();
extern void uartSendData(char*);
extern void uartReceiveData(char*,int);
extern void uart1InterruptRxEnable(void);
extern void uart1InterruptRxDisable(void);
extern void resetBuffer(char*, int);
extern int sortriderArr(char(*)[13]);

#endif /* EXTERNAL_H_ */
