#ifndef MPU6050_H
#define MPU6050_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

/** 
  * @brief  IOE DMA Direction  
  */ 
typedef enum
{
  MPU6050_DMA_TX = 0,
  MPU6050_DMA_RX = 1
}MPU6050_DMADirection_TypeDef;

/** 
  * @brief  TSENSOR Status  
  */ 
typedef enum
{
  MPU6050_OK = 0,
  MPU6050_FAIL
}MPU6050_Status_TypDef;


/**
 * @brief  Main MPU6050 structure
 */
typedef struct {
    /* Private */
    uint8_t Address;         /*!< I2C address of device. Only for private use */
    float Gyro_Mult;         /*!< Gyroscope corrector from raw data to "degrees/s". Only for private use */
    float Acce_Mult;         /*!< Accelerometer corrector from raw data to "g". Only for private use */
    /* Public */
    int16_t Accelerometer_X; /*!< Accelerometer value X axis */
    int16_t Accelerometer_Y; /*!< Accelerometer value Y axis */
    int16_t Accelerometer_Z; /*!< Accelerometer value Z axis */
    int16_t Gyroscope_X;     /*!< Gyroscope value X axis */
    int16_t Gyroscope_Y;     /*!< Gyroscope value Y axis */
    int16_t Gyroscope_Z;     /*!< Gyroscope value Z axis */
    float Temperature;       /*!< Temperature in degrees */
} MPU6050_TypeDef;

/* Uncomment the following line to use Timeout_User_Callback MPU6050_TimeoutUserCallback(). 
   If This Callback is enabled, it should be implemented by user in main function .
   MPU6050_TimeoutUserCallback() function is called whenever a timeout condition 
   occure during communication (waiting on an event that doesn't occur, bus 
   errors, busy devices ...). */   
/* #define USE_TIMEOUT_USER_CALLBACK */    
    
/* Maximum Timeout values for flags and events waiting loops. These timeouts are
   not based on accurate values, they just guarantee that the application will 
   not remain stuck if the I2C communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */   
#define MPU6050_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define MPU6050_LONG_TIMEOUT         ((uint32_t)(10 * MPU6050_FLAG_TIMEOUT))    
    

/**
  * @brief  Block Size
  */
#define MPU6050_REG_TEMP       0x00  /*!< Temperature Register of MPU6050 */
#define MPU6050_REG_CONF       0x01  /*!< Configuration Register of MPU6050 */
#define MPU6050_REG_THYS       0x02  /*!< Temperature Register of MPU6050 */
#define MPU6050_REG_TOS        0x03  /*!< Over-temp Shutdown threshold Register of MPU6050 */
#define I2C_TIMEOUT         ((uint32_t)0x3FFFF) /*!< I2C Time out */
#define MPU6050_ADDR           0x90   /*!< MPU6050 address */
#define MPU6050_I2C_SPEED      100000 /*!< I2C Speed */
  
 
void MPU6050_Init(void);
void MPU6050_LowLevel_Init(void);
ErrorStatus MPU6050_GetStatus(void);
uint8_t MPU6050_WriteConfReg(uint8_t RegValue);
uint8_t MPU6050_WriteReg(uint8_t RegName, uint16_t RegValue);
uint16_t MPU6050_ReadTemp(void);
uint8_t MPU6050_ReadConfReg(void);
static void MPU6050_DMA_Config(MPU6050_DMADirection_TypeDef Direction, uint8_t* buffer, uint8_t NumData);

/** 
  * @brief  Timeout user callback function. This function is called when a timeout
  *         condition occurs during communication with IO Expander. Only protoype
  *         of this function is decalred in IO Expander driver. Its implementation
  *         may be done into user application. This function may typically stop
  *         current operations and reset the I2C peripheral and IO Expander.
  *         To enable this function use uncomment the define USE_TIMEOUT_USER_CALLBACK
  *         at the top of this file.          
  */
#ifdef USE_TIMEOUT_USER_CALLBACK 
 uint8_t MPU6050_TIMEOUT_UserCallback(void);
#else
 #define MPU6050_TIMEOUT_UserCallback()  MPU6050_FAIL
#endif /* USE_TIMEOUT_USER_CALLBACK */
 
/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
