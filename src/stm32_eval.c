#include "stm32_eval.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_sdio.h"
#include "stm32f10x_dma.h"
#include "mpu6050.h" 

USART_TypeDef* COM_USART[COMn] = {EVAL_COM1, EVAL_COM2}; 

GPIO_TypeDef* COM_TX_PORT[COMn] = {EVAL_COM1_TX_GPIO_PORT, EVAL_COM2_TX_GPIO_PORT};
 
GPIO_TypeDef* COM_RX_PORT[COMn] = {EVAL_COM1_RX_GPIO_PORT, EVAL_COM2_RX_GPIO_PORT};
 
const uint32_t COM_USART_CLK[COMn] = {EVAL_COM1_CLK, EVAL_COM2_CLK};

const uint32_t COM_TX_PORT_CLK[COMn] = {EVAL_COM1_TX_GPIO_CLK, EVAL_COM2_TX_GPIO_CLK};
 
const uint32_t COM_RX_PORT_CLK[COMn] = {EVAL_COM1_RX_GPIO_CLK, EVAL_COM2_RX_GPIO_CLK};

const uint16_t COM_TX_PIN[COMn] = {EVAL_COM1_TX_PIN, EVAL_COM2_TX_PIN};

const uint16_t COM_RX_PIN[COMn] = {EVAL_COM1_RX_PIN, EVAL_COM2_RX_PIN};

void STM_EVAL_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM] | RCC_APB2Periph_AFIO, ENABLE);

  /* Enable UART clock */
  if (COM == COM1)
  {
    RCC_APB2PeriphClockCmd(COM_USART_CLK[COM], ENABLE); 
  }
  else
  {
    RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
  }

  /* Configure USART Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[COM];
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(COM_TX_PORT[COM], &GPIO_InitStructure);

  /* Configure USART Rx as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[COM];
  GPIO_Init(COM_RX_PORT[COM], &GPIO_InitStructure);

  /* USART configuration */
  USART_Init(COM_USART[COM], USART_InitStruct);
    
  /* Enable USART */
  USART_Cmd(COM_USART[COM], ENABLE);
}

int32_t i2c_scanner()
{
	uint32_t I2C_TimeOut = I2C_TIMEOUT;
	int32_t i2c_addr = 0; 

  /*!< Clear the MPU6050_I2C AF flag */
  I2C_ClearFlag(MPU6050_I2C, I2C_FLAG_AF);

  /*!< Enable MPU6050_I2C acknowledgement if it is already disabled by other function */
  I2C_AcknowledgeConfig(MPU6050_I2C, ENABLE);

  /*---------------------------- Transmission Phase ---------------------------*/

  /*!< Send MPU6050_I2C START condition */
  I2C_GenerateSTART(MPU6050_I2C, ENABLE);

  /*!< Test on MPU6050_I2C EV5 and clear it */
  while ((!I2C_GetFlagStatus(MPU6050_I2C,I2C_FLAG_SB)) && I2C_TimeOut)  /*!< EV5 */
  {
    I2C_TimeOut--;
  }
  if (I2C_TimeOut == 0)
  {
    return -2;
  }

	I2C_TimeOut = I2C_TIMEOUT;

	/*!< Send STMPU6050 slave address for write */
	I2C_Send7bitAddress(MPU6050_I2C, 0x68, I2C_Direction_Transmitter);

	while ((!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && I2C_TimeOut)/* EV6 */
	{
		I2C_TimeOut--;
	}

	if ((I2C_GetFlagStatus(MPU6050_I2C, I2C_FLAG_AF) != 0x00) || (I2C_TimeOut == 0))
	{
		i2c_addr = -1;
	}
	else
	{
		i2c_addr = 0x68;
	}

	return i2c_addr; 
}
