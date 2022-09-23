
#include "apm32f0xx_i2c.h"
#include "apm32f0xx_gpio.h"
#include "apm32f0xx_rcm.h"
#include "apm32f0xx_misc.h"
#include "apm32f0xx_eint.h"
#include "string.h"



#define sEE_OK                    0
#define sEE_FAIL                  1 

#define sEE_PAGESIZE           4

#define sEE_MAX_TRIALS_NUMBER     300


void I2CInit(void);
void sEE_WriteBuffer(I2C_T *i2c, uint8_t driver_Addr, uint16_t WriteAddr, uint8_t *Write_Buffer, uint16_t NumByteToWrite);

uint32_t sEE_ReadBuffer(I2C_T* i2c, uint8_t driver_Addr, uint16_t ReadAddr, uint8_t* pBuffer,  uint16_t* NumByteToRead);

uint32_t I2C_BufferWritePage(I2C_T* i2c,uint8_t driver_Addr,uint8_t WriteAddr,uint8_t *pBuffer, uint8_t* NumByteToWrite);
