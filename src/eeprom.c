/*
 * eeprom.c
 *
 *  Created on: 03-Apr-2021
 *      Author: Kumar Harsh
 */

#include "eeprom.h"

/**
 * Brief: 	Configure i2c for stm32f103c8t6
 *
 *  GPIO pins for i2c communication with EEPROM
 * 			---------------
 * 			I2C1_SCL : PB8
 * 			I2C1_SDA : PB9
 * 			--------------
 *
 * param: 	none
 * return: 	none
 */
void i2cInit()
{
	/* Enable i2c clock and GPIO clocks*/
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;/*Enable I2C GPIO port B*/
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; 					/*Enable clock to i2c*/
	AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;

	/*Reset the i2c*/
	I2C1->CR1 |= (1<<15);					/*software reset i2c*/
	I2C1->CR1 &= ~(1<<15);

	I2C1->CR2 |= I2C_MHZ_FREQ;				/*As RCC_APB1ENR is running @36MHz*/
	I2C1->CCR |= I2C_CCR_SM;				/*I2C1->CCR = 180 for SM mode @100KHz*/
	I2C1->TRISE = I2C_TRISE_SM;				/*For 100KHz Standard Mode*/

	/*Configure i2c pins for AF*/
	GPIOB->CRH |= GPIO_CRH_MODE8_0 | GPIO_CRH_CNF8; 		/*Configure I2C1_SCL PB8 in Alternate Output Open Drain @10Mhz*/
	GPIOB->CRH |= GPIO_CRH_MODE9_0 | GPIO_CRH_CNF9; 		/*Configure I2C1_SDA PB9 in Alternate Output Open Drain @10Mhz*/

}
void i2cDeInit()
{
	I2C1->CR1 &= I2CPeripheralDisable;		/*I2C Peripheral Disable*/
	RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
	RCC->APB2RSTR |= RCC_APB2RSTR_AFIORST;
}
void i2cEnable()
{
	I2C1->CR1 |= I2CPeripheralEnable;		/*I2C Peripheral Enable*/
}
void i2cDisable()
{
	I2C1->CR1 &= I2CPeripheralDisable;		/*I2C Peripheral Disable*/
}
/**
 * Brief: 	Method is used to generate a start condition and switch micro-controller into master mode.
 * param: 	none
 * return:	none
 */
void i2cStart()
{

	I2C1->CR1 |= I2C_Ack_Enable;			/*Enable Acknowledgment in CR1 Register*/
	I2C1->CR1 |= CR1_Start;					/*Enable start bit in CR1*/
	while(!(I2C1->SR1 & SR1_SB));
}

/**
 * Brief:	Method is used to send slave address to the SDA i2c bus
 * param:	address: unsigned int slave address
 * return:	none
 */
void i2cAddressSlave(uint16_t address)
{
	I2C1->DR = address;						/*send the address*/
	while(!(I2C1->SR1 & SR1_ADDR));			/*Wait till the end of address transmission*/
	temp = I2C1->SR1 | I2C1->SR2;			/*read sr1 and sr2 to clear ADDR bit*/
}

/**
 * Brief:	Method used to send data byte in the SDA line
 * param:	i2cData: uint16_t data variable
 * return:	none
 */
void i2cWrite(uint16_t i2cData)
{
	while(!(I2C1->SR1 & SR1_TXE));			/*wait for Data Register to be empty*/
	I2C1->DR = i2cData;						/*Data byte is loaded into the i2c data register*/
	while(!(I2C1->SR1 & SR1_BTF));			/*wait for byte transfer to be finished*/
}

/**
 * Brief:	Method to write multiple bytes
 */
void i2cWriteMulti(char* data, uint8_t size)
{
	while(!(I2C1->SR1 & SR1_TXE));			/*wait for Data Register to be empty*/
	while(size)
	{
		while(!(I2C1->SR1 & SR1_TXE));		/*wait for Data Register to be empty*/
		I2C1->DR = (uint32_t)(*data++); /*Data byte is loaded into the i2c data register*/
		size--;
	}
	while(!(I2C1->SR1 & SR1_BTF));			/*wait for byte transfer to be finished*/
}

/**
 * Brief: 	Method used to read received bytes
 * param:	none
 * return:	none
 */
void i2cRead(uint8_t slave_addr, uint8_t size, char* buffer)
{
	uint8_t remainingBytes = size;

	i2cAddressSlave(slave_addr+1);		/* sending i2c slave address with a read bit */

	while(remainingBytes > 2)
	{
		while(!(I2C1->SR1 & SR1_RXNE));		/* Wait for RXNE to be set */

		/**** Store read data into buffer ****/
		buffer[size-remainingBytes] = I2C1->DR;

		/**** set Ack bit to acknowledge the data received */
		I2C1->CR1 |= I2C_Ack_Enable;			/*Enable Acknowledgment in CR1 Register*/

		remainingBytes--;
	}

	/* Read Second LAST byte */
	while(!(I2C1->SR1 & SR1_RXNE));		/* Wait for RXNE to be set */
	buffer[size-remainingBytes] = I2C1->DR;

	I2C1->CR1 &= ~I2C_Ack_Enable;			/*Clear Ack bit in CR1 Register*/

	i2cStop();

	remainingBytes--;

	/* Read LAST byte */
	while(!(I2C1->SR1 & SR1_RXNE));		/* Wait for RXNE to be set */
	buffer[size-remainingBytes] = I2C1->DR;
}

/**
 * Brief: 	Method used to generate the stop condition
 * param:	none
 * return:	none
 */
void i2cStop()
{
	I2C1->CR1 |= CR1_STOP;					/*Stop condition is generated*/
}

/**
 * Brief:	Method to write EEPROM with data bytes.
 */
void eeprom_pageWrite(uint8_t slave_addr,uint8_t mem_address,char* data)
{
	const uint8_t pageLimit = 8;

	/* Sending a write i2c frame for EEPROM write */
	i2cStart();
	i2cAddressSlave(slave_addr|0);		/* sending i2c slave address with a write bit */
	i2cWrite(mem_address);				/* write memory address */
	/* Write data into EEPROM */
	i2cWriteMulti(data,pageLimit);
	i2cStop();
}

/**
 * Brief:	Method to read sequentially EEPROM multiple data
 */
void eeprom_seqRead(uint8_t slave_address,uint8_t mem_addr,char* buffer)
{
	i2cStart();
	i2cAddressSlave(slave_address|0);	/* sending i2c slave address with a write bit */
	i2cWrite(mem_addr);					/* write memory address to read from the start of EEPROM fresh page*/
	/**** Repeated Start ****/
	i2cStart();
	i2cRead(slave_address, 8, buffer);
	i2cStop();
}


