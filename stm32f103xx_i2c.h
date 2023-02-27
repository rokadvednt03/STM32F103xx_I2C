#include "stm32f10x.h"

#define I2C_ACK_ENABLE 1
#define I2C_ACK_DISABLE 0

#define I2C_SCLSpeed_SM 100
#define I2C_SCLSpeed_FM 400

#define I2C_FM_DutyCycle_2 0
#define I2C_FM_DutyCycle_16_9 1

typedef struct
{
	uint32_t ACKControl;
	uint32_t SCLSpeed;
	uint32_t DeviceADDR;
	uint32_t FM_DutyCycle;
	
}I2C_Config_t;


typedef struct
{
	I2C_Config_t i2cconfig;
	I2C_TypeDef *pI2Cx;
}I2C_Handle_t;

void I2C_PeriClockControl(I2C_TypeDef *pI2Cx , uint32_t EnorDi);
void I2C_Init(I2C_Handle_t *pi2cHandler);
void I2C_Control(I2C_TypeDef *pI2Cx , uint32_t EnorDi);
