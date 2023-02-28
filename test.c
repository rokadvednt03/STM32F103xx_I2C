#include "stm32f103xx_gpio.h"
#include "stm32f103xx_i2c.h"
#include <string.h>

int main()
{
	uint8_t some_data[] = "HeLLO";
	GPIO_Handle_t gpiotest;
	I2C_Handle_t i2ctest;
	
	gpiotest.pGPIOx = GPIOB;
	gpiotest.GPIO_PinConfig.GPIO_ModeInOut = GPIO_OUT_10MHZ;
	gpiotest.GPIO_PinConfig.GPIO_ModeInOut = GPIO_MODE_OUT_OD;
	
	i2ctest.pI2Cx = I2C1;
	i2ctest.i2cconfig.ACKControl = I2C_ACK_ENABLE;
	i2ctest.i2cconfig.DeviceADDR = 0x61;
	i2ctest.i2cconfig.SCLSpeed = I2C_SCLSpeed_SM;
	
	
	GPIO_PeriClockControl(GPIOB,ENABLE);
	gpiotest.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_Init(&gpiotest);
	gpiotest.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GPIO_Init(&gpiotest);
	
		
	I2C_PeriClockControl(I2C1,ENABLE);
	I2C_Init(&i2ctest);
	
	I2C1->CR1 |= (I2C_CR1_PE);
	
	I2C_MasterSendData(&i2ctest,some_data,strlen((char*)some_data),0x61);
	
}



