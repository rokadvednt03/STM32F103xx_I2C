#include "stm32f103xx_gpio.h"
#include "stm32f103xx_i2c.h"

int main()
{
	I2C_Handle_t i2ctest;
	i2ctest.pI2Cx = I2C1;
	i2ctest.i2cconfig.ACKControl = I2C_ACK_ENABLE;
	i2ctest.i2cconfig.SCLSpeed = 50;
	i2ctest.i2cconfig.FM_DutyCycle = I2C_FM_DutyCycle_16_9;
	i2ctest.i2cconfig.DeviceADDR = 0x7F;
	
	I2C_PeriClockControl(I2C1,ENABLE);
	I2C_PeriClockControl(I2C2,ENABLE);
	I2C_Init(&i2ctest);

	
	i2ctest.pI2Cx = I2C2;
	i2ctest.i2cconfig.ACKControl = I2C_ACK_DISABLE;
	i2ctest.i2cconfig.SCLSpeed = 500;
	i2ctest.i2cconfig.FM_DutyCycle = I2C_FM_DutyCycle_16_9;
	i2ctest.i2cconfig.DeviceADDR = 0x45;
	I2C_Init(&i2ctest);
	
}
