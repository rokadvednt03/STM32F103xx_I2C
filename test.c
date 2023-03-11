#include "stm32f103xx_gpio.h"
#include "stm32f103xx_i2c.h"
#include <string.h>
#define slave_addr ((0x68)&(0x7F))

int main()
{
	uint8_t receive_data[150] ;
	uint8_t rec;
	
		
	uint8_t data[] = {0x51,0x52};
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
	
	//I2C_Mem_Write(&i2ctest,0x68,0x75,2,data);
	I2C_Mem_Read(&i2ctest,0x68,data[0],1,&rec);
	I2C_Mem_Read(&i2ctest,0x68,data[1],rec,receive_data);
	
}
