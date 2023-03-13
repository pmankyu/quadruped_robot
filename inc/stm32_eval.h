/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_EVAL_H
#define __STM32_EVAL_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f10x.h"

typedef enum 
{
  COM1 = 0,
  COM2 = 1
} COM_TypeDef;   

/** @addtogroup STM3210E_EVAL_LOW_LEVEL_COM
  * @{
  */
#define COMn                             2

/**
 * @brief Definition for COM port1, connected to USART1
 */ 
#define EVAL_COM1                        USART1
#define EVAL_COM1_CLK                    RCC_APB2Periph_USART1
#define EVAL_COM1_TX_PIN                 GPIO_Pin_9
#define EVAL_COM1_TX_GPIO_PORT           GPIOA
#define EVAL_COM1_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM1_RX_PIN                 GPIO_Pin_10
#define EVAL_COM1_RX_GPIO_PORT           GPIOA
#define EVAL_COM1_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM1_IRQn                   USART1_IRQn

/**
 * @brief Definition for COM port2, connected to USART2
 */ 
#define EVAL_COM2                        USART2
#define EVAL_COM2_CLK                    RCC_APB1Periph_USART2
#define EVAL_COM2_TX_PIN                 GPIO_Pin_2
#define EVAL_COM2_TX_GPIO_PORT           GPIOA
#define EVAL_COM2_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM2_RX_PIN                 GPIO_Pin_3
#define EVAL_COM2_RX_GPIO_PORT           GPIOA
#define EVAL_COM2_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM2_IRQn                   USART2_IRQn

/**
  * @brief  MPU6050 Temperature Sensor I2C Interface pins
  */  
#define MPU6050_I2C                         I2C1
#define MPU6050_I2C_CLK                     RCC_APB1Periph_I2C1
#define MPU6050_I2C_SCL_PIN                 GPIO_Pin_6                  /* PB.06 */
#define MPU6050_I2C_SCL_GPIO_PORT           GPIOB                       /* GPIOB */
#define MPU6050_I2C_SCL_GPIO_CLK            RCC_APB2Periph_GPIOB
#define MPU6050_I2C_SDA_PIN                 GPIO_Pin_7                  /* PB.07 */
#define MPU6050_I2C_SDA_GPIO_PORT           GPIOB                       /* GPIOB */
#define MPU6050_I2C_SDA_GPIO_CLK            RCC_APB2Periph_GPIOB
#define MPU6050_I2C_SMBUSALERT_PIN          GPIO_Pin_5                  /* PB.05 */
#define MPU6050_I2C_SMBUSALERT_GPIO_PORT    GPIOB                       /* GPIOB */
#define MPU6050_I2C_SMBUSALERT_GPIO_CLK     RCC_APB2Periph_GPIOB
#define MPU6050_I2C_DR                      ((uint32_t)0x40005410)

#define MPU6050_DMA_CLK                     RCC_AHBPeriph_DMA1
#define MPU6050_DMA_TX_CHANNEL              DMA1_Channel6
#define MPU6050_DMA_RX_CHANNEL              DMA1_Channel7
#define MPU6050_DMA_TX_TCFLAG               DMA1_FLAG_TC6
#define MPU6050_DMA_RX_TCFLAG               DMA1_FLAG_TC7

void STM_EVAL_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct);
int32_t i2c_scanner();
#ifdef __cplusplus
}
#endif

#endif /* __STM32_EVAL_H */
