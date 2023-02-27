#include "stm32f10x.h"
#include "stm32f103xx_i2c.h"

void I2C_PeriClockControl(I2C_TypeDef *pI2Cx , uint32_t EnorDi)
{
		if(EnorDi == ENABLE)
		{
			if(pI2Cx == I2C1)
				RCC->APB1ENR |= (1<<21);
			else
				RCC->APB1ENR |= (1<<22);
		}
		else
		{
			if(pI2Cx == I2C1)
				RCC->APB1ENR &= ~(1<<21);
			else
				RCC->APB1ENR &= ~(1<<22);
		}
}

void I2C_Control(I2C_TypeDef *pI2Cx , uint32_t EnorDi)
{
		if(EnorDi == ENABLE)
		{
			pI2Cx->CR1 |= (1<<0);
		}
		else
		{
			pI2Cx->CR1 &= ~(1<<0);
		}
}


void I2C_Init(I2C_Handle_t *pi2cHandler)
{
	if(pi2cHandler->i2cconfig.ACKControl == I2C_ACK_ENABLE)
	{
		pi2cHandler->pI2Cx->CR1 |= (1<<10);
	}
	else
	{
		pi2cHandler->pI2Cx->CR1 &= ~(1<<10);
	}
	
	
	pi2cHandler->pI2Cx->CR2 |= ((36) & 0x3F);
	
	pi2cHandler->pI2Cx->OAR1 &= ~(1 << 15);
	pi2cHandler->pI2Cx->OAR1 |= (1<< 14);
	pi2cHandler->pI2Cx->OAR1 |= (pi2cHandler->i2cconfig.DeviceADDR << 1);
	
	if(pi2cHandler->i2cconfig.SCLSpeed <= I2C_SCLSpeed_SM)
	{
		//Config for Standerd mode
		pi2cHandler->pI2Cx->CCR &= ~(I2C_CCR_FS);		
		pi2cHandler->pI2Cx->CCR |= ((((36000)/(pi2cHandler->i2cconfig.SCLSpeed*2))) & 0xFFF);
	}
	else
	{
		//Config for Fast mode
		pi2cHandler->pI2Cx->CCR |= (I2C_CCR_FS);
		if(pi2cHandler->i2cconfig.FM_DutyCycle == I2C_FM_DutyCycle_2)
		{
			pi2cHandler->pI2Cx->CCR &= ~(I2C_CCR_DUTY);
			pi2cHandler->pI2Cx->CCR |= ((((36000)/(pi2cHandler->i2cconfig.SCLSpeed*3))) & 0xFFF);
		}
		else
		{
			pi2cHandler->pI2Cx->CCR |= (I2C_CCR_DUTY);
			pi2cHandler->pI2Cx->CCR |= ((((36000)/(pi2cHandler->i2cconfig.SCLSpeed*25))) & 0xFFF);
		}
	}
	
}
