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

extern char tempDataStr[8];
extern char mq135DataStr[8];
extern char mq7DataStr[8];
extern uint8_t sysTimeout;
extern void delay_in_sec(unsigned int);
extern void delay_in_ms(unsigned int);

#endif /* EXTERNAL_H_ */
