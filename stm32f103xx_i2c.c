/**
  ******************************************************************************
  * @file    		stm32f103xx_i2c.c
  * @author  		Vedant A. Rokad
  * @processor 	ARM Cortex-M3
	* @controller STM32F103C8T8
  * @date    		26-Feb-2022
  * @brief   		Device_Driver Source file
  ******************************************************************************
 **/

#include "stm32f10x.h"
#include "stm32f103xx_i2c.h"


static void I2C_Startbit_Gernation(I2C_TypeDef *pI2Cx);
static void I2C_ExcicuteADDR_Phase(I2C_TypeDef *pI2Cx ,uint8_t SlaveADDR);
static void I2C_ClearADDRFlag(I2C_TypeDef *pI2Cx);
static void I2C_Stopbit_Generation(I2C_TypeDef *pI2Cx);

/////////////////////////////////////////////////////////////////////////////////////////
//                                                                                     //
//																I2C_PeriClockControl                                 //
//																															  										 //
/////////////////////////////////////////////////////////////////////////////////////////
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




/////////////////////////////////////////////////////////////////////////////////////////
//                                                                                     //
//														    		I2C_Control                                      //
//																															  										 //
/////////////////////////////////////////////////////////////////////////////////////////
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





/////////////////////////////////////////////////////////////////////////////////////////
//                                                                                     //
//							  				     					I2C_Init                                       //
//																															  										 //
/////////////////////////////////////////////////////////////////////////////////////////
void I2C_Init(I2C_Handle_t *pi2cHandler)
{
	uint32_t trise;
	//ACK
	if(pi2cHandler->i2cconfig.ACKControl == I2C_ACK_ENABLE)
	{
		pi2cHandler->pI2Cx->CR1 |= (I2C_CR1_ACK);
	}
	else
	{
		pi2cHandler->pI2Cx->CR1 &= ~(I2C_CR1_ACK);
	}
	
	//Set_periClockfreq
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
	
	//TRISE Config
	if(pi2cHandler->i2cconfig.SCLSpeed <= I2C_SCLSpeed_SM)
	{
		trise = ((36000000U)/(1000000U))+ 1 ;
		pi2cHandler->pI2Cx->TRISE = ((trise) & 0x3F);
	}
	else
	{
		trise = ((36000000U)*(300)/(1000000000U))+ 1 ;
		pi2cHandler->pI2Cx->TRISE = ((trise) & 0x3F);
	}
	
}





/////////////////////////////////////////////////////////////////////////////////////////
//                                                                                     //
//																I2C_MasterSendData                                   //
//																															  										 //
/////////////////////////////////////////////////////////////////////////////////////////
void I2C_MasterSendData(I2C_Handle_t *pi2cHandler , uint8_t *pTxBuffer , uint32_t len ,uint8_t Slaveaddr)
{
	//1.START_Condition Generation 
		I2C_Startbit_Gernation(pi2cHandler->pI2Cx);
	
	//2. confirm that start generation is completed by checking the SB flag in the SR1
	//   Note: Until SB is cleared SCL will be stretched (pulled to LOW)
	while(!(I2C_GetFlagStatus(pi2cHandler->pI2Cx,I2C_FLAG_SB) & FLAG_SET));
	
	//3. Send the address of the slave with r/nw bit set to R(1) (total 8 bits )
		I2C_ExcicuteADDR_Phase(pi2cHandler->pI2Cx,Slaveaddr);
	
	//4. wait until address phase is completed by checking the ADDR flag in teh SR1
	while(!(I2C_GetFlagStatus(pi2cHandler->pI2Cx,I2C_FLAG_ADDR) & FLAG_SET));
	
	//5. clear the ADDR flag according to its software sequence
	//   Note: Until ADDR is cleared SCL will be stretched (pulled to LOW)
	I2C_ClearADDRFlag(pi2cHandler->pI2Cx);
	
	//6. send the data until len becomes 0
	while(len--)
	{
		while(!(I2C_GetFlagStatus(pi2cHandler->pI2Cx,I2C_FLAG_TXE) & FLAG_SET));
		pi2cHandler->pI2Cx->DR = *(pTxBuffer);
		pTxBuffer--;
	}
	
	//7. when Len becomes zero wait for TXE=1 and BTF=1 before generating the STOP condition
	//   Note: TXE=1 , BTF=1 , means that both SR and DR are empty and next transmission should begin
	//   when BTF=1 SCL will be stretched (pulled to LOW)
	while(!(I2C_GetFlagStatus(pi2cHandler->pI2Cx,I2C_FLAG_TXE) & FLAG_SET));
	while(!(I2C_GetFlagStatus(pi2cHandler->pI2Cx,I2C_FLAG_BTF) & FLAG_SET));
	
	//8. Generate STOP condition and master need not to wait for the completion of stop condition.
	//   Note: generating STOP, automatically clears the BTF
	I2C_Stopbit_Generation(pi2cHandler->pI2Cx);

}






/////////////////////////////////////////////////////////////////////////////////////////
//                                                                                     //
//																I2C_Helping_Functions                                //
//																															  										 //
/////////////////////////////////////////////////////////////////////////////////////////
static void I2C_Startbit_Gernation(I2C_TypeDef *pI2Cx)
{
	pI2Cx->CR1 |= (I2C_CR1_START); 
}

uint8_t I2C_GetFlagStatus(I2C_TypeDef *pI2Cx , uint8_t Flagname)
{
	if (pI2Cx->SR1 & Flagname)
	{
		return FLAG_SET;
	}	
	return FLAG_RESET;
}


static void I2C_ExcicuteADDR_Phase(I2C_TypeDef *pI2Cx ,uint8_t SlaveADDR)
{
	SlaveADDR = SlaveADDR << 1 ;
	SlaveADDR &= ~(1 << 0) ;
	pI2Cx->DR = SlaveADDR ;
}

static void I2C_ClearADDRFlag(I2C_TypeDef *pI2Cx)
{
	uint32_t buffer ;
	buffer = pI2Cx->SR1 ;
	buffer = pI2Cx->SR2 ;
	(void)buffer;
}

static void I2C_Stopbit_Generation(I2C_TypeDef *pI2Cx)
{
	pI2Cx->CR1 |= (I2C_CR1_STOP);
}

