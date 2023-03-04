#include "stm32f103xx_gpio.h"
#include "stm32f103xx_i2c.h"
#include <string.h>
#define slave_addr ((0x68)&(0x7F))


int main()
{
	uint32_t reg_state;
	uint8_t data[] = "Hello_I2C-THIS IS VEDANT ROKAD";
	uint8_t *pdata = data;
	uint32_t len = strlen((char*)data);
	I2C_Handle_t i2ctest;
	
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN ;
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN ;
	
	GPIOB->CRL |= (GPIO_CRL_MODE6_0) | (GPIO_CRL_MODE6_1);
	GPIOB->CRL |= (GPIO_CRL_CNF6_0) | (GPIO_CRL_CNF6_1);
	
	
	GPIOB->CRL |= (GPIO_CRL_MODE7_0) | (GPIO_CRL_MODE7_1);
	GPIOB->CRL |= (GPIO_CRL_CNF7_0) | (GPIO_CRL_CNF7_1);
	
		
	i2ctest.pI2Cx = I2C1;
	i2ctest.i2cconfig.ACKControl = I2C_ACK_ENABLE;
	i2ctest.i2cconfig.DeviceADDR = 0x55;
	i2ctest.i2cconfig.SCLSpeed = 50;
	I2C_Init(&i2ctest);
	
	I2C1->CR1 |= (I2C_CR1_PE);
	
	I2C_MasterSendData(&i2ctest,data,len,0x34);
	
}




/*
int main()
{
	uint32_t reg_state;
	uint8_t data = 0x45;
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN ;
	RCC->APB1ENR |= RCC_APB1ENR_I2C2EN ;
	
	GPIOB->CRH |= (GPIO_CRH_MODE10) | (GPIO_CRH_CNF10);
	GPIOB->CRH |= (GPIO_CRH_MODE11) | (GPIO_CRH_CNF11);
	
		
	I2C2->CR2 |= (0x24 & (0x3F));
	I2C2->CCR |= (0x32 & (0xFFF));
	I2C2->TRISE = 0x25;
	
	I2C2->CR1 |= (I2C_CR1_PE);
	
	
	I2C2->CR1 |= I2C_CR1_START;
	
	while(!(I2C2->SR1 & I2C_SR1_SB));
	
	reg_state = I2C2->SR1;

	I2C2->DR = slave_addr;
	
	while(!(I2C2->SR1 & I2C_SR1_ADDR));
	
	reg_state = I2C2->SR1;
	reg_state = I2C2->SR2;
	(void)reg_state;
		
	while(!(I2C2->SR1 & I2C_SR1_TXE));
	
	I2C2->DR = ((data) & (0xFF)) ;
	while(!(I2C2->SR1 & I2C_SR1_TXE));
	
	while(!(I2C2->SR1 & I2C_SR1_BTF));
	I2C2->CR1 |= (I2C_CR1_STOP);
	
}

*/