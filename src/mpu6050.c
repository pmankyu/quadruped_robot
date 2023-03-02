#include "mpu6050.h"
#include "stm32_eval.h"
 
/** @defgroup STM32_EVAL_I2C_TSENSOR_Private_Variables
  * @{
  */ 
  
__IO uint32_t  MPU6050_Timeout = MPU6050_LONG_TIMEOUT; 

/**
 * @brief  Initializes the MPU6050_I2C.
 * @param  None
 * @retval None
 */
void MPU6050_Init(void)
{
	I2C_InitTypeDef   I2C_InitStructure;

	MPU6050_LowLevel_Init();

	I2C_DeInit(MPU6050_I2C);

	/*!< MPU6050_I2C Init */
	I2C_InitStructure.I2C_Mode = I2C_Mode_SMBusHost;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = MPU6050_I2C_SPEED;
	I2C_Init(MPU6050_I2C, &I2C_InitStructure);

	/*!< Enable SMBus Alert interrupt */
	I2C_ITConfig(MPU6050_I2C, I2C_IT_ERR, ENABLE);

	/*!< MPU6050_I2C Init */

	I2C_Cmd(MPU6050_I2C, ENABLE);
}

/**
  * @brief  Initializes the MPU6050_I2C..
  * @param  None
  * @retval None
  */
void MPU6050_LowLevel_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /*!< MPU6050_I2C Periph clock enable */
  RCC_APB1PeriphClockCmd(MPU6050_I2C_CLK, ENABLE);
    
  /*!< MPU6050_I2C_SCL_GPIO_CLK, MPU6050_I2C_SDA_GPIO_CLK 
       and MPU6050_I2C_SMBUSALERT_GPIO_CLK Periph clock enable */
  RCC_APB2PeriphClockCmd(MPU6050_I2C_SCL_GPIO_CLK | MPU6050_I2C_SDA_GPIO_CLK |
                         MPU6050_I2C_SMBUSALERT_GPIO_CLK, ENABLE);
  
  /*!< Configure MPU6050_I2C pins: SCL */
  GPIO_InitStructure.GPIO_Pin = MPU6050_I2C_SCL_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(MPU6050_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure MPU6050_I2C pins: SDA */
  GPIO_InitStructure.GPIO_Pin = MPU6050_I2C_SDA_PIN;
  GPIO_Init(MPU6050_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure MPU6050_I2C pin: SMBUS ALERT */
  GPIO_InitStructure.GPIO_Pin = MPU6050_I2C_SMBUSALERT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(MPU6050_I2C_SMBUSALERT_GPIO_PORT, &GPIO_InitStructure); 
}

/**
  * @brief  Checks the MPU6050 status.
  * @param  None
  * @retval ErrorStatus: MPU6050 Status (ERROR or SUCCESS).
  */
ErrorStatus MPU6050_GetStatus(void)
{
  uint32_t I2C_TimeOut = I2C_TIMEOUT;

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
    return ERROR;
  }
  
  I2C_TimeOut = I2C_TIMEOUT;

  /*!< Send STMPU6050 slave address for write */
  I2C_Send7bitAddress(MPU6050_I2C, MPU6050_ADDR, I2C_Direction_Transmitter);

  while ((!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && I2C_TimeOut)/* EV6 */
  {
    I2C_TimeOut--;
  }

  if ((I2C_GetFlagStatus(MPU6050_I2C, I2C_FLAG_AF) != 0x00) || (I2C_TimeOut == 0))
  {
    return ERROR;
  }
  else
  {
    return SUCCESS;
  }
}

/**
  * @brief  Write to the configuration register of the MPU6050.
  * @param  RegValue: sepecifies the value to be written to MPU6050 configuration 
  *         register.
  * @retval None
  */
uint8_t MPU6050_WriteConfReg(uint8_t RegValue)
{   
  uint8_t MPU6050_BufferTX = 0;  
  MPU6050_BufferTX = (uint8_t)(RegValue);
  
  /* Test on BUSY Flag */
  MPU6050_Timeout = MPU6050_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(MPU6050_I2C,I2C_FLAG_BUSY)) 
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }
  
  /* Configure DMA Peripheral */
  MPU6050_DMA_Config(MPU6050_DMA_TX, (uint8_t*)(&MPU6050_BufferTX), 1);
  
  /* Enable the I2C peripheral */
  I2C_GenerateSTART(MPU6050_I2C, ENABLE);
  
  /* Test on SB Flag */
  MPU6050_Timeout = MPU6050_FLAG_TIMEOUT;
  while (I2C_GetFlagStatus(MPU6050_I2C,I2C_FLAG_SB) == RESET) 
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }
  
  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(MPU6050_I2C, MPU6050_ADDR, I2C_Direction_Transmitter);
  
  /* Test on ADDR Flag */
  MPU6050_Timeout = MPU6050_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }
  
  /* Transmit the first address for r/w operations */
  I2C_SendData(MPU6050_I2C, MPU6050_REG_CONF);
  
  /* Test on TXE FLag (data sent) */
  MPU6050_Timeout = MPU6050_FLAG_TIMEOUT;
  while ((!I2C_GetFlagStatus(MPU6050_I2C,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(MPU6050_I2C,I2C_FLAG_BTF)))  
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }
  
  /* Enable I2C DMA request */
  I2C_DMACmd(MPU6050_I2C,ENABLE);
  
  /* Enable DMA TX Channel */
  DMA_Cmd(MPU6050_DMA_TX_CHANNEL, ENABLE);
  
  /* Wait until DMA Transfer Complete */
  MPU6050_Timeout = MPU6050_LONG_TIMEOUT;
  while (!DMA_GetFlagStatus(MPU6050_DMA_TX_TCFLAG))
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }  
  
  /* Wait until BTF Flag is set before generating STOP */
  MPU6050_Timeout = MPU6050_LONG_TIMEOUT;
  while ((!I2C_GetFlagStatus(MPU6050_I2C,I2C_FLAG_BTF)))  
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }
  
  /* Send STOP Condition */
  I2C_GenerateSTOP(MPU6050_I2C, ENABLE);
  
  /* Disable DMA TX Channel */
  DMA_Cmd(MPU6050_DMA_TX_CHANNEL, DISABLE);
  
  /* Disable I2C DMA request */  
  I2C_DMACmd(MPU6050_I2C,DISABLE);
  
  /* Clear DMA TX Transfer Complete Flag */
  DMA_ClearFlag(MPU6050_DMA_TX_TCFLAG);  
  
  return MPU6050_OK;
}

/**
  * @brief  Write to the specified register of the MPU6050.
  * @param  RegName: specifies the MPU6050 register to be written.
  *              This member can be one of the following values:    
  *                  - MPU6050_REG_TOS: Over-limit temperature register
  *                  - MPU6050_REG_THYS: Hysteresis temperature register
  * @param  RegValue: value to be written to MPU6050 register.  
  * @retval None
  */
uint8_t MPU6050_WriteReg(uint8_t RegName, uint16_t RegValue)
{   
  uint8_t MPU6050_BufferTX[2] ={0,0};
  MPU6050_BufferTX[0] = (uint8_t)(RegValue >> 8);
  MPU6050_BufferTX[1] = (uint8_t)(RegValue);
  
  /* Test on BUSY Flag */
  MPU6050_Timeout = MPU6050_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(MPU6050_I2C,I2C_FLAG_BUSY)) 
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }
  
  /* Configure DMA Peripheral */
  MPU6050_DMA_Config(MPU6050_DMA_TX, (uint8_t*)MPU6050_BufferTX, 2);
  
  /* Enable the I2C peripheral */
  I2C_GenerateSTART(MPU6050_I2C, ENABLE);
  
  /* Test on SB Flag */
  MPU6050_Timeout = MPU6050_FLAG_TIMEOUT;
  while (I2C_GetFlagStatus(MPU6050_I2C,I2C_FLAG_SB) == RESET) 
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }
  
  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(MPU6050_I2C, MPU6050_ADDR, I2C_Direction_Transmitter);
  
  /* Test on ADDR Flag */
  MPU6050_Timeout = MPU6050_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }
  
  /* Transmit the first address for r/w operations */
  I2C_SendData(MPU6050_I2C, RegName);
  
  /* Test on TXE FLag (data sent) */
  MPU6050_Timeout = MPU6050_FLAG_TIMEOUT;
  while ((!I2C_GetFlagStatus(MPU6050_I2C,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(MPU6050_I2C,I2C_FLAG_BTF)))  
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }
  
  /* Enable I2C DMA request */
  I2C_DMACmd(MPU6050_I2C,ENABLE);
  
  /* Enable DMA TX Channel */
  DMA_Cmd(MPU6050_DMA_TX_CHANNEL, ENABLE);
  
  /* Wait until DMA Transfer Complete */
  MPU6050_Timeout = MPU6050_LONG_TIMEOUT;
  while (!DMA_GetFlagStatus(MPU6050_DMA_TX_TCFLAG))
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }  
  
  /* Wait until BTF Flag is set before generating STOP */
  MPU6050_Timeout = MPU6050_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(MPU6050_I2C,I2C_FLAG_BTF))  
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }
  
  /* Send STOP Condition */
  I2C_GenerateSTOP(MPU6050_I2C, ENABLE);
  
  /* Disable DMA TX Channel */
  DMA_Cmd(MPU6050_DMA_TX_CHANNEL, DISABLE);
  
  /* Disable I2C DMA request */  
  I2C_DMACmd(MPU6050_I2C,DISABLE);
  
  /* Clear DMA TX Transfer Complete Flag */
  DMA_ClearFlag(MPU6050_DMA_TX_TCFLAG);
  
  return MPU6050_OK;
}

/**
  * @brief  Read Temperature register of MPU6050: double temperature value.
  * @param  None
  * @retval MPU6050 measured temperature value.
  */
uint16_t MPU6050_ReadTemp(void)
{   
  uint8_t MPU6050_BufferRX[2] ={0,0};
  uint16_t tmp = 0;
  
  /* Test on BUSY Flag */
  MPU6050_Timeout = MPU6050_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(MPU6050_I2C,I2C_FLAG_BUSY)) 
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }
  
  /* Configure DMA Peripheral */
  MPU6050_DMA_Config(MPU6050_DMA_RX, (uint8_t*)MPU6050_BufferRX, 2);  
  
  /* Enable DMA NACK automatic generation */
  I2C_DMALastTransferCmd(MPU6050_I2C, ENABLE);
  
  /* Enable the I2C peripheral */
  I2C_GenerateSTART(MPU6050_I2C, ENABLE);
  
  /* Test on SB Flag */
  MPU6050_Timeout = MPU6050_FLAG_TIMEOUT;
  while (!I2C_GetFlagStatus(MPU6050_I2C,I2C_FLAG_SB)) 
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }
  
  /* Send device address for write */
  I2C_Send7bitAddress(MPU6050_I2C, MPU6050_ADDR, I2C_Direction_Transmitter);
  
  /* Test on ADDR Flag */
  MPU6050_Timeout = MPU6050_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) 
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }
  
  /* Send the device's internal address to write to */
  I2C_SendData(MPU6050_I2C, MPU6050_REG_TEMP);  
  
  /* Test on TXE FLag (data sent) */
  MPU6050_Timeout = MPU6050_FLAG_TIMEOUT;
  while ((!I2C_GetFlagStatus(MPU6050_I2C,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(MPU6050_I2C,I2C_FLAG_BTF)))  
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }
  
  /* Send START condition a second time */  
  I2C_GenerateSTART(MPU6050_I2C, ENABLE);
  
  /* Test on SB Flag */
  MPU6050_Timeout = MPU6050_FLAG_TIMEOUT;
  while (!I2C_GetFlagStatus(MPU6050_I2C,I2C_FLAG_SB)) 
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }
  
  /* Send MPU6050 address for read */
  I2C_Send7bitAddress(MPU6050_I2C, MPU6050_ADDR, I2C_Direction_Receiver);
  
  /* Test on ADDR Flag */
  MPU6050_Timeout = MPU6050_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))   
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }
  
  /* Enable I2C DMA request */
  I2C_DMACmd(MPU6050_I2C,ENABLE);
  
  /* Enable DMA RX Channel */
  DMA_Cmd(MPU6050_DMA_RX_CHANNEL, ENABLE);
  
  /* Wait until DMA Transfer Complete */
  MPU6050_Timeout = MPU6050_LONG_TIMEOUT;
  while (!DMA_GetFlagStatus(MPU6050_DMA_RX_TCFLAG))
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }        
  
  /* Send STOP Condition */
  I2C_GenerateSTOP(MPU6050_I2C, ENABLE);
  
  /* Disable DMA RX Channel */
  DMA_Cmd(MPU6050_DMA_RX_CHANNEL, DISABLE);
  
  /* Disable I2C DMA request */  
  I2C_DMACmd(MPU6050_I2C,DISABLE);
  
  /* Clear DMA RX Transfer Complete Flag */
  DMA_ClearFlag(MPU6050_DMA_RX_TCFLAG);
  
  /*!< Store MPU6050_I2C received data */
  tmp = (uint16_t)(MPU6050_BufferRX[0] << 8);
  tmp |= MPU6050_BufferRX[1];    
  
  /*!< Return Temperature value */
  return (uint16_t)(tmp >> 7);
}

/**
  * @brief  Read the configuration register from the MPU6050.
  * @param  None
  * @retval MPU6050 configuration register value.
  */
uint8_t MPU6050_ReadConfReg(void)
{   
  uint8_t MPU6050_BufferRX[2] ={0,0}; 
  
  /* Test on BUSY Flag */
  MPU6050_Timeout = MPU6050_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(MPU6050_I2C,I2C_FLAG_BUSY)) 
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }
  
  /* Configure DMA Peripheral */
  MPU6050_DMA_Config(MPU6050_DMA_RX, (uint8_t*)MPU6050_BufferRX, 2);  
  
  /* Enable DMA NACK automatic generation */
  I2C_DMALastTransferCmd(MPU6050_I2C, ENABLE);
  
  /* Enable the I2C peripheral */
  I2C_GenerateSTART(MPU6050_I2C, ENABLE);
  
  /* Test on SB Flag */
  MPU6050_Timeout = MPU6050_FLAG_TIMEOUT;
  while (!I2C_GetFlagStatus(MPU6050_I2C,I2C_FLAG_SB)) 
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }
  
  /* Send device address for write */
  I2C_Send7bitAddress(MPU6050_I2C, MPU6050_ADDR, I2C_Direction_Transmitter);
  
  /* Test on ADDR Flag */
  MPU6050_Timeout = MPU6050_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) 
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }
  
  /* Send the device's internal address to write to */
  I2C_SendData(MPU6050_I2C, MPU6050_REG_CONF);  
  
  /* Test on TXE FLag (data sent) */
  MPU6050_Timeout = MPU6050_FLAG_TIMEOUT;
  while ((!I2C_GetFlagStatus(MPU6050_I2C,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(MPU6050_I2C,I2C_FLAG_BTF)))  
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }
  
  /* Send START condition a second time */  
  I2C_GenerateSTART(MPU6050_I2C, ENABLE);
  
  /* Test on SB Flag */
  MPU6050_Timeout = MPU6050_FLAG_TIMEOUT;
  while (!I2C_GetFlagStatus(MPU6050_I2C,I2C_FLAG_SB)) 
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }
  
  /* Send MPU6050 address for read */
  I2C_Send7bitAddress(MPU6050_I2C, MPU6050_ADDR, I2C_Direction_Receiver);
  
  /* Test on ADDR Flag */
  MPU6050_Timeout = MPU6050_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))   
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }
  
  /* Enable I2C DMA request */
  I2C_DMACmd(MPU6050_I2C,ENABLE);
  
  /* Enable DMA RX Channel */
  DMA_Cmd(MPU6050_DMA_RX_CHANNEL, ENABLE);
  
  /* Wait until DMA Transfer Complete */
  MPU6050_Timeout = MPU6050_LONG_TIMEOUT;
  while (!DMA_GetFlagStatus(MPU6050_DMA_RX_TCFLAG))
  {
    if((MPU6050_Timeout--) == 0) return MPU6050_TIMEOUT_UserCallback();
  }        
  
  /* Send STOP Condition */
  I2C_GenerateSTOP(MPU6050_I2C, ENABLE);
  
  /* Disable DMA RX Channel */
  DMA_Cmd(MPU6050_DMA_RX_CHANNEL, DISABLE);
  
  /* Disable I2C DMA request */  
  I2C_DMACmd(MPU6050_I2C,DISABLE);
  
  /* Clear DMA RX Transfer Complete Flag */
  DMA_ClearFlag(MPU6050_DMA_RX_TCFLAG);
  
  /*!< Return Temperature value */
  return (uint8_t)MPU6050_BufferRX[0];
}

/**
  * @brief  Configure the DMA Peripheral used to handle communication via I2C.
  * @param  None
  * @retval None
  */
static void MPU6050_DMA_Config(MPU6050_DMADirection_TypeDef Direction, uint8_t* buffer, uint8_t NumData)
{
  DMA_InitTypeDef DMA_InitStructure;
  
  RCC_AHBPeriphClockCmd(MPU6050_DMA_CLK, ENABLE);
  
  /* Initialize the DMA_PeripheralBaseAddr member */
  DMA_InitStructure.DMA_PeripheralBaseAddr = MPU6050_I2C_DR;
  /* Initialize the DMA_MemoryBaseAddr member */
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buffer;
   /* Initialize the DMA_PeripheralInc member */
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  /* Initialize the DMA_MemoryInc member */
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  /* Initialize the DMA_PeripheralDataSize member */
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  /* Initialize the DMA_MemoryDataSize member */
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  /* Initialize the DMA_Mode member */
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  /* Initialize the DMA_Priority member */
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  /* Initialize the DMA_M2M member */
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  
  /* If using DMA for Reception */
  if (Direction == MPU6050_DMA_RX)
  {
    /* Initialize the DMA_DIR member */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    
    /* Initialize the DMA_BufferSize member */
    DMA_InitStructure.DMA_BufferSize = NumData;
    
    DMA_DeInit(MPU6050_DMA_RX_CHANNEL);
    
    DMA_Init(MPU6050_DMA_RX_CHANNEL, &DMA_InitStructure);
  }
   /* If using DMA for Transmission */
  else if (Direction == MPU6050_DMA_TX)
  { 
    /* Initialize the DMA_DIR member */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    
    /* Initialize the DMA_BufferSize member */
    DMA_InitStructure.DMA_BufferSize = NumData;
    
    DMA_DeInit(MPU6050_DMA_TX_CHANNEL);
    
    DMA_Init(MPU6050_DMA_TX_CHANNEL, &DMA_InitStructure);
  }
}
