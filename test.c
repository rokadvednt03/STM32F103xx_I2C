#include "stm32f103xx_gpio.h"
#include "stm32f103xx_i2c.h"
#include <string.h>
#define slave_addr ((0x68)&(0x7F))

int main()
{
	char a[] = "hello world";
	uint8_t rcv[250] ;
	uint8_t mem_addr[] = {0x51,0x52};
	uint8_t len ;
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
	//I2C master and arduino slave
//	I2C_MasterSendData(&i2ctest,(uint8_t*)a,sizeof(a),0x68);

	//I2C slave and arduino master
	I2C_Mem_Read(&i2ctest,0x68,mem_addr[0],1,&len);
	I2C_Mem_Read(&i2ctest,0x68,mem_addr[1],len,rcv);
	
}
