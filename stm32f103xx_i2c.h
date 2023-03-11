/**
  ******************************************************************************
  * @file    		stm32f103xx_i2c.h
  * @author  		Vedant A. Rokad
  * @processor 	ARM Cortex-M3
	* @controller STM32F103C8T8
  * @date    		26-Feb-2022
  * @brief   		Device_Driver Header file
  ******************************************************************************
 **/
 
 
#include "stm32f10x.h"



/////////////////////////////////////////////////////////////////////////////////////////
//                                                                                     //
//      																I2C_MACROS                                     //
//																															  										 //
/////////////////////////////////////////////////////////////////////////////////////////
#define I2C_ACK_ENABLE 1
#define I2C_ACK_DISABLE 0

#define I2C_SCLSpeed_SM 100
#define I2C_SCLSpeed_FM 400

#define I2C_FM_DutyCycle_2 0
#define I2C_FM_DutyCycle_16_9 1

#define FLAG_SET SET
#define FLAG_RESET RESET

#define I2C_FLAG_SB (1<<0)
#define I2C_FLAG_ADDR (1<<1)
#define I2C_FLAG_BTF (1<<2)
#define I2C_FLAG_ADD10 (1<<3)
#define I2C_FLAG_STOPF (1<<4)
#define I2C_FLAG_RXNE (1<<6)
#define I2C_FLAG_TXE (1<<7)
#define I2C_FLAG_BERR (1<<8)
#define I2C_FLAG_ARLO (1<<9)
#define I2C_FLAG_AF (1<<10)
#define I2C_FLAG_OVR (1<<11)
#define I2C_FLAG_PECERR (1<<12)
#define I2C_FLAG_TIMEOUT (1<<14)
#define I2C_FLAG_SMBALERT (1<<15)




/////////////////////////////////////////////////////////////////////////////////////////
//                                                                                     //
//																I2C_Configuration_Structure                          //
//																															  										 //
/////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	uint32_t ACKControl;
	uint32_t SCLSpeed;
	uint32_t DeviceADDR;
	uint32_t FM_DutyCycle;
	
}I2C_Config_t;




/////////////////////////////////////////////////////////////////////////////////////////
//                                                                                     //
//																I2C_Handling_Structure                               //
//																															  										 //
/////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	I2C_Config_t i2cconfig;
	I2C_TypeDef *pI2Cx;
}I2C_Handle_t;





/////////////////////////////////////////////////////////////////////////////////////////
//                                                                                     //
//												  	I2C_APIs_&_HELPING_FUNCTION                              //
//																															  										 //
/////////////////////////////////////////////////////////////////////////////////////////
void I2C_PeriClockControl(I2C_TypeDef *pI2Cx , uint32_t EnorDi);
void I2C_Init(I2C_Handle_t *pi2cHandler);
void I2C_Control(I2C_TypeDef *pI2Cx , uint32_t EnorDi);
void I2C_MasterSendData(I2C_Handle_t *pi2cHandler , uint8_t *pTxBuffer , uint32_t len ,uint8_t Slaveaddr);
uint8_t I2C_GetFlagStatus(I2C_TypeDef *pI2Cx , uint8_t Flagname);
void I2C_MasterReceiveData(I2C_Handle_t *pi2cHandler , uint8_t *pRxBuffer , uint32_t len ,uint8_t Slaveaddr);

void I2C_Mem_Write(I2C_Handle_t *pi2cHandler , uint8_t Slaveaddr , uint8_t MemAddr ,uint8_t MemAddrSize ,uint8_t *pTxBuffer);
void I2C_Mem_Read(I2C_Handle_t *pi2cHandler , uint8_t Slaveaddr , uint8_t MemAddr ,uint8_t MemAddrSize ,uint8_t *pRxBuffer);
