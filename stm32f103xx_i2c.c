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
#include <stdint.h>

static void I2C_Startbit_Gernation(I2C_TypeDef *pI2Cx);
static void I2C_ExcicuteADDR_PhaseWrite(I2C_TypeDef *pI2Cx ,uint8_t SlaveADDR);
static void I2C_ClearFlag(I2C_TypeDef *pI2Cx);
static void I2C_Stopbit_Generation(I2C_TypeDef *pI2Cx);
static void I2C_EcxicuteADDR_Read(I2C_TypeDef *pI2Cx ,uint8_t SlaveADDR);

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
		// ACKControl;
	if(pi2cHandler->i2cconfig.ACKControl == I2C_ACK_ENABLE)
			pi2cHandler->pI2Cx->CR1 |= I2C_CR1_ACK;
	else
			pi2cHandler->pI2Cx->CR1 &= ~(I2C_CR1_ACK);
	
	// SCLSpeed;
	pi2cHandler->pI2Cx->CR2 |= ((36) & (0x3F));
	
	// DeviceADDR;
	pi2cHandler->pI2Cx->OAR1 &= ~(I2C_OAR1_ADDMODE);
	pi2cHandler->pI2Cx->OAR1 |= (1<<14);
	pi2cHandler->pI2Cx->OAR1 |= (((pi2cHandler->i2cconfig.DeviceADDR)&(0x7F)) << 1);
	// FM_DutyCycle;
	if(pi2cHandler->i2cconfig.SCLSpeed <= I2C_SCLSpeed_SM)
	{
		pi2cHandler->pI2Cx->CCR &= ~(I2C_CCR_FS);
		pi2cHandler->pI2Cx->CCR |= (36000000U/(2*(pi2cHandler->i2cconfig.SCLSpeed*1000U)));
	}
	else
	{
		pi2cHandler->pI2Cx->CCR |= (I2C_CCR_FS);
		pi2cHandler->pI2Cx->CCR |= (I2C_CCR_DUTY);
		if(pi2cHandler->i2cconfig.FM_DutyCycle == I2C_FM_DutyCycle_2)
			pi2cHandler->pI2Cx->CCR |= (36000000U/(3*(pi2cHandler->i2cconfig.SCLSpeed*1000U)));
		else
			pi2cHandler->pI2Cx->CCR |= (36000000U/(25*(pi2cHandler->i2cconfig.SCLSpeed*1000U)));
	}
	
	//TRISE
	if(pi2cHandler->i2cconfig.SCLSpeed <= I2C_SCLSpeed_FM)
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
	uint32_t reg_state;
	//1.START_BIT_GENERATE	
	pi2cHandler->pI2Cx->CR1 |= I2C_CR1_START;
	while(!(pi2cHandler->pI2Cx->SR1 & I2C_SR1_SB));

	reg_state = pi2cHandler->pI2Cx->SR1;
	pi2cHandler->pI2Cx->DR = (Slaveaddr<<1);
	
	while(!(pi2cHandler->pI2Cx->SR1 & I2C_SR1_ADDR));
	reg_state = pi2cHandler->pI2Cx->SR1;
	reg_state = pi2cHandler->pI2Cx->SR2;
	(void)reg_state;
		
	while(!(pi2cHandler->pI2Cx->SR1 & I2C_SR1_TXE));
	
	while(len--)
	{
		
		pi2cHandler->pI2Cx->DR = ((*pTxBuffer) & (0xFF)) ;
		while(!(pi2cHandler->pI2Cx->SR1 & I2C_SR1_TXE));
		pTxBuffer++;
		//I2C1->CR1 |= I2C_CR1_START;
		//while(!(pi2cHandler->pI2Cx->SR1 & I2C_SR1_SB));
		//reg_state = pi2cHandler->pI2Cx->SR1;
	}
	while(!(pi2cHandler->pI2Cx->SR1 & I2C_SR1_TXE));
	while(!(pi2cHandler->pI2Cx->SR1 & I2C_SR1_BTF));
	
	pi2cHandler->pI2Cx->CR1 |= (I2C_CR1_STOP);
}



/////////////////////////////////////////////////////////////////////////////////////////
//                                                                                     //
//																I2C_MasterReceiveData                                //
//																															  										 //
/////////////////////////////////////////////////////////////////////////////////////////
void I2C_MasterReceiveData(I2C_Handle_t *pi2cHandler , uint8_t *pRxBuffer , uint32_t len ,uint8_t Slaveaddr)
{
	uint32_t reg_state;
	Slaveaddr = (Slaveaddr << 1) ;
	Slaveaddr |= (1<<0);
	
	pi2cHandler->pI2Cx->CR1 |= (I2C_CR1_ACK);
	pi2cHandler->pI2Cx->CR1 |= I2C_CR1_START;
	while(!(pi2cHandler->pI2Cx->SR1 & I2C_SR1_SB));
	
	reg_state = pi2cHandler->pI2Cx->SR1;
	
	pi2cHandler->pI2Cx->DR = Slaveaddr;
	while(!(pi2cHandler->pI2Cx->SR1 & I2C_SR1_ADDR));
	
	if(len == 1)
	{
		pi2cHandler->pI2Cx->CR1 &= ~(I2C_CR1_ACK);
		reg_state = pi2cHandler->pI2Cx->SR1;
		reg_state = pi2cHandler->pI2Cx->SR2;
		pi2cHandler->pI2Cx->CR1 |= (I2C_CR1_STOP);
		while(!(pi2cHandler->pI2Cx->SR1 & I2C_SR1_RXNE));
		*pRxBuffer = pi2cHandler->pI2Cx->DR;
	}
	
	else
	{
		if(len > 1)
		{
			uint32_t i ;
			reg_state = pi2cHandler->pI2Cx->SR1;
			reg_state = pi2cHandler->pI2Cx->SR2;
			
			for(i = len ; i > 0 ; i--)
			{
				while(!(pi2cHandler->pI2Cx->SR1 & I2C_SR1_RXNE));
				if(i == 2)
				{
					pi2cHandler->pI2Cx->CR1 &= ~(I2C_CR1_ACK);
					
						pi2cHandler->pI2Cx->CR1 |= (I2C_CR1_STOP);
				}
				*pRxBuffer = pi2cHandler->pI2Cx->DR;
				pRxBuffer++;
				
			}
		}
	}
	
	(void)reg_state;
	if(pi2cHandler->i2cconfig.ACKControl == I2C_ACK_ENABLE)
	pi2cHandler->pI2Cx->CR1 |= I2C_CR1_ACK;
}



/////////////////////////////////////////////////////////////////////////////////////////
//                                                                                     //
//																I2C_MemoryWrite                                      //
//																															  										 //
/////////////////////////////////////////////////////////////////////////////////////////
void I2C_Mem_Write(I2C_Handle_t *pi2cHandler , uint8_t Slaveaddr , uint8_t MemAddr ,uint8_t MemAddrSize ,uint8_t *pTxBuffer)
{
	I2C_Startbit_Gernation(pi2cHandler->pI2Cx);
	while(!(I2C_GetFlagStatus(pi2cHandler->pI2Cx,I2C_SR1_SB)));
	
	I2C_ClearFlag(pi2cHandler->pI2Cx);
	
	I2C_ExcicuteADDR_PhaseWrite(pi2cHandler->pI2Cx,Slaveaddr);
	
	while(!(I2C_GetFlagStatus(pi2cHandler->pI2Cx,I2C_SR1_ADDR)));
	
	I2C_ClearFlag(pi2cHandler->pI2Cx);
	
	while(!(I2C_GetFlagStatus(pi2cHandler->pI2Cx,I2C_SR1_TXE)));
	
	pi2cHandler->pI2Cx->DR = MemAddr & (0xFF);
	
	while(!(I2C_GetFlagStatus(pi2cHandler->pI2Cx,I2C_SR1_TXE)));
	
	while(MemAddrSize--)
	{
		pi2cHandler->pI2Cx->DR = *pTxBuffer & (0xFF);
		while(!(I2C_GetFlagStatus(pi2cHandler->pI2Cx,I2C_SR1_TXE)));
		pTxBuffer++;
	}
	
	while(!(I2C_GetFlagStatus(pi2cHandler->pI2Cx,I2C_SR1_TXE)));
	while(!(I2C_GetFlagStatus(pi2cHandler->pI2Cx,I2C_SR1_BTF)));
	
	I2C_Stopbit_Generation(pi2cHandler->pI2Cx);
	
}


/////////////////////////////////////////////////////////////////////////////////////////
//                                                                                     //
//																I2C_MemoryWrite                                      //
//																															  										 //
/////////////////////////////////////////////////////////////////////////////////////////

void I2C_Mem_Read(I2C_Handle_t *pi2cHandler , uint8_t Slaveaddr , uint8_t MemAddr ,uint8_t MemAddrSize ,uint8_t *pRxBuffer)
{

	I2C_Startbit_Gernation(pi2cHandler->pI2Cx);
	while(!(I2C_GetFlagStatus(pi2cHandler->pI2Cx,I2C_SR1_SB)));
	
	I2C_ClearFlag(pi2cHandler->pI2Cx);
	
	I2C_ExcicuteADDR_PhaseWrite(pi2cHandler->pI2Cx,Slaveaddr);
	
	while(!(I2C_GetFlagStatus(pi2cHandler->pI2Cx,I2C_SR1_ADDR)));
	
	I2C_ClearFlag(pi2cHandler->pI2Cx);
	
	while(!(I2C_GetFlagStatus(pi2cHandler->pI2Cx,I2C_SR1_TXE)));
	
	pi2cHandler->pI2Cx->DR = MemAddr & (0xFF);
	
	while(!(I2C_GetFlagStatus(pi2cHandler->pI2Cx,I2C_SR1_TXE)));
	I2C_MasterReceiveData(pi2cHandler,pRxBuffer,MemAddrSize,Slaveaddr);

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


static void I2C_ExcicuteADDR_PhaseWrite(I2C_TypeDef *pI2Cx ,uint8_t SlaveADDR)
{
	SlaveADDR = SlaveADDR << 1 ;
	SlaveADDR &= ~(1 << 0) ;
	pI2Cx->DR = SlaveADDR ;
}

static void I2C_ClearFlag(I2C_TypeDef *pI2Cx)
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

static void I2C_EcxicuteADDR_Read(I2C_TypeDef *pI2Cx ,uint8_t SlaveADDR)
{
	SlaveADDR = SlaveADDR << 1 ;
	SlaveADDR |= (1);
	pI2Cx->DR = SlaveADDR;
}
