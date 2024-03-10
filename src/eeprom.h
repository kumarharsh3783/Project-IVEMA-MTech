/*
 * eeprom.h
 *
 *  Created on: 03-Apr-2021
 *      Author: Kumar Harsh
 */

#ifndef EEPROM_H_
#define EEPROM_H_

/**
 * Includes
 */
#include "stm32f10x.h"
#include "external.h"

/**
 * Macro and typedef Definitions
 */
#define I2C_MHZ_FREQ 			((uint16_t)36)
#define I2C_CCR_SM	  			((uint16_t)180)
#define I2C_TRISE_SM  			((uint16_t)37)
#define I2C_Ack_Enable          ((uint16_t)0x0400)
#define I2CPeripheralEnable 	((uint16_t)0x0001)
#define I2CPeripheralDisable	0xfffffffe
#define CR1_POS					((uint16_t)0x0800)
#define CR1_ACK					((uint16_t)0x0400)
#define CR1_STOP				((uint16_t)0x0200)
#define CR1_Start				((uint16_t)0x0100)
#define SR1_SB					0x00000001
#define SR1_ADDR				((uint16_t)0x0002)
#define SR1_BTF					((uint16_t)0x0004)
#define SR1_TXE					0x00000080
#define SR1_RXNE				0x00000040
typedef enum{false, true} boolean;

/**
 * Private Variable Declarations
 */
uint8_t temp;
uint8_t page_addr_write;
uint8_t page_addr_read;

/**
 * Private Function Declarations
 */
void i2cInit(void);
void i2cEnable(void);
void i2cDisable(void);
void i2cDeInit(void);
void i2cStart(void);
void i2cAddressSlave(uint16_t);
void i2cWrite(uint16_t);
void i2cWriteMulti(char*,uint8_t);
void i2cRead(uint8_t,uint8_t,char*);
void i2cStop(void);

void eeprom_pageWrite(uint8_t,uint8_t,char*);
void eeprom_seqRead(uint8_t,uint8_t,char*);

#endif /* EEPROM_H_ */
