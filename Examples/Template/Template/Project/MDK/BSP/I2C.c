#include "I2C.h"

/*Waiting TIMEOUT*/
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define sEE_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))
__IO uint32_t  sEETimeout = sEE_LONG_TIMEOUT;


__IO uint16_t  sEEDataNum;


void I2CInit(void)
 {
    GPIO_Config_T gpioConfigStruct;
    I2C_Config_T i2cConfigStruct;
     /** Enable I2C related Clock */
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOB);
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_I2C1);
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);


     /** Free I2C_SCL and I2C_SDA */
    gpioConfigStruct.mode = GPIO_MODE_OUT;
    gpioConfigStruct.speed = GPIO_SPEED_50MHz;
    gpioConfigStruct.outtype = GPIO_OUT_TYPE_PP;
    gpioConfigStruct.pupd = GPIO_PUPD_NO;
    gpioConfigStruct.pin = GPIO_PIN_6;
    GPIO_Config(GPIOB, &gpioConfigStruct);

    gpioConfigStruct.pin = GPIO_PIN_7;
    GPIO_Config(GPIOB, &gpioConfigStruct);

    GPIO_SetBit(GPIOB,GPIO_PIN_6);
    GPIO_SetBit(GPIOB,GPIO_PIN_7);

    /** Connect I2C to SCL */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_6, GPIO_AF_PIN1);
    /** Connect I2C to SDA */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_7, GPIO_AF_PIN1);
    /**  Config I2C1 GPIO */
    gpioConfigStruct.mode = GPIO_MODE_AF;
    gpioConfigStruct.speed = GPIO_SPEED_50MHz;
    gpioConfigStruct.outtype = GPIO_OUT_TYPE_OD;
    gpioConfigStruct.pupd = GPIO_PUPD_NO;
    gpioConfigStruct.pin = GPIO_PIN_6;
    GPIO_Config(GPIOB, &gpioConfigStruct);

    gpioConfigStruct.pin = GPIO_PIN_7;
    GPIO_Config(GPIOB, &gpioConfigStruct);

    /**  Config I2C1 */
    I2C_Reset(I2C1);
    RCM_ConfigI2CCLK(RCM_I2C1CLK_SYSCLK);
    i2cConfigStruct.ack = I2C_ACK_ENABLE;
    i2cConfigStruct.ackaddress = I2C_ACK_ADDRESS_7BIT;
    i2cConfigStruct.address1 = 0XC0;
    i2cConfigStruct.analogfilter = I2C_ANALOG_FILTER_ENABLE;
    i2cConfigStruct.digitalfilter = I2C_DIGITAL_FILTER_0;
    i2cConfigStruct.mode = I2C_MODE_I2C;
    i2cConfigStruct.timing = 0x1042F013;
    I2C_Config(I2C1,&i2cConfigStruct);

    /** Enable the I2C1 Interrupt */
 
    /** Enable I2Cx */
     I2C_Enable(I2C1);
 }
 

uint32_t sEE_WaitEepromStandbyState(I2C_T *i2c, uint8_t driver_Addr)      
{
  __IO uint32_t sEETrials = 0;
  
  /* Keep looping till the slave acknowledge his address or maximum number 
  of trials is reached*/ 
  
  /* Configure CR2 register : set Slave Address and end mode */
  I2C_HandlingTransfer(i2c, driver_Addr, 0, I2C_RELOAD_MODE_AUTOEND, I2C_GENERATE_NO_STARTSTOP);  
  
  do
  { 
    /* Initialize sEETimeout */
    sEETimeout = I2CT_FLAG_TIMEOUT;
    
    /* Clear NACKF */
    I2C_ClearStatusFlag(i2c, I2C_FLAG_NACK | I2C_FLAG_STOP);
    
    /* Generate start */
    I2C_EnableGenerateStart(i2c);
    
    /* Wait until timeout elapsed */
    while (sEETimeout-- != 0); 
    
    /* Check if the maximum allowed number of trials has bee reached */
    if (sEETrials++ == sEE_MAX_TRIALS_NUMBER)
    {
      /* If the maximum number of trials has been reached, exit the function */
      return sEE_FAIL;
    }
  }
  while(I2C_ReadStatusFlag(i2c, I2C_FLAG_NACK) != RESET);
  
  /* Clear STOPF */
  I2C_ClearStatusFlag(i2c, I2C_FLAG_STOP);
  
  /* Return sEE_OK if device is ready */
  return sEE_OK;
} 
 
 
void sEE_WriteBuffer(I2C_T *i2c, uint8_t driver_Addr, uint16_t WriteAddr, uint8_t *Write_Buffer, uint16_t NumByteToWrite)
{
  uint16_t NumOfPage = 0, NumOfSingle = 0, count = 0;
  uint16_t Addr = 0;
  
  Addr = WriteAddr % sEE_PAGESIZE;
  count = sEE_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / sEE_PAGESIZE;
  NumOfSingle = NumByteToWrite % sEE_PAGESIZE;
  
  /* If WriteAddr is sEE_PAGESIZE aligned  */
  if(Addr == 0) 
  {
    /* If NumByteToWrite < sEE_PAGESIZE */
    if(NumOfPage == 0) 
    {
      /* Store the number of data to be written */
      sEEDataNum = NumOfSingle;
      /* Start writing data */
      I2C_BufferWritePage(i2c, driver_Addr, WriteAddr, Write_Buffer,  (uint8_t*)(&sEEDataNum));        
      sEE_WaitEepromStandbyState(i2c,driver_Addr);
    }
    /* If NumByteToWrite > sEE_PAGESIZE */
    else  
    {
      while(NumOfPage--)
      {
        /* Store the number of data to be written */
        sEEDataNum = sEE_PAGESIZE;        
        I2C_BufferWritePage(i2c, driver_Addr, WriteAddr, Write_Buffer,  (uint8_t*)(&sEEDataNum));  
        sEE_WaitEepromStandbyState(i2c,driver_Addr);
        WriteAddr +=  sEE_PAGESIZE;
        Write_Buffer += sEE_PAGESIZE;
      }
      
      if(NumOfSingle!=0)
      {
        /* Store the number of data to be written */
        sEEDataNum = NumOfSingle;          
        I2C_BufferWritePage(i2c, driver_Addr, WriteAddr, Write_Buffer,  (uint8_t*)(&sEEDataNum)); 
        sEE_WaitEepromStandbyState(i2c,driver_Addr);
      }
    }
  }
  /* If WriteAddr is not sEE_PAGESIZE aligned  */
  else 
  {
    /* If NumByteToWrite < sEE_PAGESIZE */
    if(NumOfPage== 0) 
    {
      /* If the number of data to be written is more than the remaining space 
      in the current page: */
      if (NumByteToWrite > count)
      {
        /* Store the number of data to be written */
        sEEDataNum = count;        
        /* Write the data contained in same page */
       I2C_BufferWritePage(i2c, driver_Addr, WriteAddr, Write_Buffer,  (uint8_t*)(&sEEDataNum)); 
        sEE_WaitEepromStandbyState(i2c,driver_Addr);    
        
        /* Store the number of data to be written */
        sEEDataNum = (NumByteToWrite - count);          
        /* Write the remaining data in the following page */
		 
		  I2C_BufferWritePage(i2c, driver_Addr,  (WriteAddr + count), (Write_Buffer + count),  (uint8_t*)(&sEEDataNum)); 
         sEE_WaitEepromStandbyState(i2c,driver_Addr);  
      }      
      else      
      {
        /* Store the number of data to be written */
        sEEDataNum = NumOfSingle;         
        I2C_BufferWritePage(i2c, driver_Addr, WriteAddr, Write_Buffer,  (uint8_t*)(&sEEDataNum)); 
         sEE_WaitEepromStandbyState(i2c,driver_Addr);     
      }     
    }
    /* If NumByteToWrite > sEE_PAGESIZE */
    else
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / sEE_PAGESIZE;
      NumOfSingle = NumByteToWrite % sEE_PAGESIZE;
      
      if(count != 0)
      {  
        /* Store the number of data to be written */
        sEEDataNum = count;         
         I2C_BufferWritePage(i2c, driver_Addr, WriteAddr, Write_Buffer,  (uint8_t*)(&sEEDataNum)); 
         sEE_WaitEepromStandbyState(i2c,driver_Addr);  
        WriteAddr += count;
        Write_Buffer += count;
      } 
      
      while(NumOfPage--)
      {
        /* Store the number of data to be written */
        sEEDataNum = sEE_PAGESIZE;          
        I2C_BufferWritePage(i2c, driver_Addr, WriteAddr, Write_Buffer,  (uint8_t*)(&sEEDataNum)); 
        sEETimeout = sEE_LONG_TIMEOUT;
         sEE_WaitEepromStandbyState(i2c,driver_Addr);  
        WriteAddr +=  sEE_PAGESIZE;
        Write_Buffer += sEE_PAGESIZE;  
      }
      if(NumOfSingle != 0)
      {
        /* Store the number of data to be written */
        sEEDataNum = NumOfSingle;           
        I2C_BufferWritePage(i2c, driver_Addr, WriteAddr, Write_Buffer,  (uint8_t*)(&sEEDataNum)); 
        sEE_WaitEepromStandbyState(i2c,driver_Addr);  
      }
    }
  }  
}

uint32_t sEE_ReadBuffer (I2C_T* i2c, uint8_t driver_Addr, uint16_t ReadAddr, uint8_t* pBuffer,  uint16_t* NumByteToRead)
{
	uint32_t NumbOfSingle = 0, Count = 0, DataNum = 0, StartCom = 0;

	/* Get number of reload cycles */
	Count = (*NumByteToRead) / 255;  
	NumbOfSingle = (*NumByteToRead) % 255;

	/* Configure slave address, nbytes, reload and generate start */
	I2C_HandlingTransfer(i2c, driver_Addr, 2, I2C_RELOAD_MODE_SOFTEND, I2C_GENERATE_START_WRITE);

	/* Wait until TXIS flag is set */
	sEETimeout = sEE_LONG_TIMEOUT;
	while(I2C_ReadStatusFlag(i2c, I2C_FLAG_TXINT) == RESET)
	{
	if((sEETimeout--) == 0) return sEE_FAIL;
	}

	/* Send MSB of memory address */
	I2C_TxData(i2c, (uint8_t)((ReadAddr & 0xFF00) >> 8));

	/* Wait until TXIS flag is set */
	sEETimeout = sEE_LONG_TIMEOUT;  
	while(I2C_ReadStatusFlag(i2c, I2C_FLAG_TXINT) == RESET)
	{
	if((sEETimeout--) == 0) return sEE_FAIL;
	}

	/* Send LSB of memory address  */
	I2C_TxData(i2c, (uint8_t)(ReadAddr & 0x00FF));

	/* Wait until TC flag is set */
	sEETimeout = sEE_LONG_TIMEOUT;
	while(I2C_ReadStatusFlag(i2c, I2C_FLAG_TXCF) == RESET)
	{
	if((sEETimeout--) == 0) return sEE_FAIL;
	}  

	/* If number of Reload cycles is not equal to 0 */
	if (Count != 0)
	{
	/* Starting communication */
	StartCom = 1;

	/* Wait until all reload cycles are performed */
	while( Count != 0)
	{ 
	  /* If a read transfer is performed */
	  if (StartCom == 0)      
	  {
		/* Wait until TCR flag is set */
		sEETimeout = sEE_LONG_TIMEOUT; 
		while(I2C_ReadStatusFlag(i2c, I2C_FLAG_TCRF) == RESET)
		{
		  if((sEETimeout--) == 0) return sEE_FAIL;
		}
	  }      
	  
	  /* if remains one read cycle */
	  if ((Count == 1) && (NumbOfSingle == 0))
	  {
		/* if starting communication */
		if (StartCom != 0)
		{
		  /* Configure slave address, end mode and start condition */
		  I2C_HandlingTransfer(i2c, driver_Addr, 255, I2C_RELOAD_MODE_AUTOEND, I2C_GENERATE_START_READ);
		}
		else
		{
		  /* Configure slave address, end mode */
		  I2C_HandlingTransfer(i2c, driver_Addr, 255, I2C_RELOAD_MODE_AUTOEND, I2C_GENERATE_NO_STARTSTOP);          
		}
	  }
	  else 
	  {
		/* if starting communication */
		if (StartCom != 0)
		{
		  /* Configure slave address, end mode and start condition */
		  I2C_HandlingTransfer(i2c, driver_Addr, 255, I2C_RELOAD_MODE_RELOAD, I2C_GENERATE_START_READ);
		}
		else
		{
		  /* Configure slave address, end mode */
		  I2C_HandlingTransfer(i2c, driver_Addr, 255, I2C_RELOAD_MODE_RELOAD, I2C_GENERATE_NO_STARTSTOP);          
		} 
	  }
	  
	  /* Update local variable */
	  StartCom = 0;      
	  DataNum = 0;
	  
	  /* Wait until all data are received */
	  while (DataNum != 255)
	  {        
		/* Wait until RXNE flag is set */
		sEETimeout = sEE_LONG_TIMEOUT;
		while(I2C_ReadStatusFlag(i2c, I2C_FLAG_RXBNE) == RESET)
		{
		  if((sEETimeout--) == 0) return sEE_FAIL;
		}
		
		/* Read data from RXDR */
		pBuffer[DataNum]= I2C_RxData(i2c);
		
		/* Update number of received data */
		DataNum++;
		(*NumByteToRead)--;
	  }      
	  /* Update Pointer of received buffer */ 
	  pBuffer += DataNum;  
	  
	  /* update number of reload cycle */
	  Count--;
	}

	/* If number of single data is not equal to 0 */
	if (NumbOfSingle != 0)
	{            
	  /* Wait until TCR flag is set */
	  sEETimeout = sEE_LONG_TIMEOUT;   
	  while(I2C_ReadStatusFlag(i2c, I2C_FLAG_TCRF) == RESET)
	  {
		if((sEETimeout--) == 0) return sEE_FAIL;
	  }
	  
	  /* Update CR2 : set Nbytes and end mode */
	  I2C_HandlingTransfer(i2c, driver_Addr, (uint8_t)(NumbOfSingle), I2C_RELOAD_MODE_AUTOEND, I2C_GENERATE_NO_STARTSTOP);
	  
	  /* Reset local variable */
	  DataNum = 0;
	  
	  /* Wait until all data are received */
	  while (DataNum != NumbOfSingle)
	  {        
		/* Wait until RXNE flag is set */
		sEETimeout = sEE_LONG_TIMEOUT;
		while(I2C_ReadStatusFlag(i2c, I2C_FLAG_RXBNE) == RESET)
		{
		  if((sEETimeout--) == 0) return sEE_FAIL;
		}

		/* Read data from RXDR */
		pBuffer[DataNum]= I2C_RxData(i2c);

		/* Update number of received data */
		DataNum++;
		(*NumByteToRead)--;
	  } 
	}
	}
	else
	{
	/* Update CR2 : set Slave Address , set read request, generate Start and set end mode */
	I2C_HandlingTransfer(i2c, driver_Addr, (uint32_t)(NumbOfSingle), I2C_RELOAD_MODE_AUTOEND, I2C_GENERATE_START_READ);

	/* Reset local variable */
	DataNum = 0;

	/* Wait until all data are received */
	while (DataNum != NumbOfSingle)
	{
	  /* Wait until RXNE flag is set */
	  sEETimeout = sEE_LONG_TIMEOUT; 
	  while(I2C_ReadStatusFlag(i2c, I2C_FLAG_RXBNE) == RESET)
	  {
		if((sEETimeout--) == 0) return sEE_FAIL;
	  }
	  
	  /* Read data from RXDR */
	  pBuffer[DataNum]= I2C_RxData(i2c);
	  
	  /* Update number of received data */
	  DataNum++;
	  (*NumByteToRead)--;
	}    
	}  

	/* Wait until STOPF flag is set */
	sEETimeout = sEE_LONG_TIMEOUT;
	while(I2C_ReadStatusFlag(i2c, I2C_FLAG_STOP) == RESET)
	{
	if((sEETimeout--) == 0) return sEE_FAIL;
	}

	/* Clear STOPF flag */
	I2C_ClearStatusFlag(i2c, I2C_FLAG_STOP);

	/* If all operations OK, return sEE_OK (0) */
	return sEE_OK;	
	
}




uint32_t I2C_BufferWritePage(I2C_T* i2c,uint8_t driver_Addr,uint8_t WriteAddr,uint8_t *pBuffer, uint8_t* NumByteToWrite)
{
	uint32_t DataNum = 0;

	/* Configure slave address, nbytes, reload and generate start */
	I2C_HandlingTransfer(i2c, driver_Addr, 2, I2C_RELOAD_MODE_RELOAD, I2C_GENERATE_START_WRITE);

	/* Wait until TXIS flag is set */
	sEETimeout = sEE_LONG_TIMEOUT;
	while(I2C_ReadStatusFlag(i2c, I2C_FLAG_TXINT) == RESET)
	{
		if((sEETimeout--) == 0) return sEE_FAIL;
	}

	/* Send MSB of memory address */
	I2C_TxData(i2c, (uint8_t)((WriteAddr & 0xFF00) >> 8));  

	/* Wait until TXIS flag is set */
	sEETimeout = sEE_LONG_TIMEOUT;
	while(I2C_ReadStatusFlag(i2c, I2C_FLAG_TXINT) == RESET)
	{
		if((sEETimeout--) == 0) return sEE_FAIL;
	}

	/* Send LSB of memory address  */
	I2C_TxData(i2c, (uint8_t)(WriteAddr & 0x00FF));

	/* Wait until TCR flag is set */
	sEETimeout = sEE_LONG_TIMEOUT;
	while(I2C_ReadStatusFlag(i2c, I2C_FLAG_TCRF) == RESET)
	{
		if((sEETimeout--) == 0) return sEE_FAIL;
	}

	/* Update CR2 : set Slave Address , set write request, generate Start and set end mode */
	I2C_HandlingTransfer(i2c, driver_Addr, (uint8_t)(*NumByteToWrite), I2C_RELOAD_MODE_AUTOEND, I2C_GENERATE_NO_STARTSTOP);

	while (DataNum != (*NumByteToWrite))
	{      
	/* Wait until TXIS flag is set */
		sEETimeout = sEE_LONG_TIMEOUT;
	while(I2C_ReadStatusFlag(i2c, I2C_FLAG_TXINT) == RESET)
	{
		if((sEETimeout--) == 0) return sEE_FAIL;
	}  

	/* Write data to TXDR */
		I2C_TxData(i2c, (uint8_t)(pBuffer[DataNum]));

	/* Update number of transmitted data */
		DataNum++;   
	}  

	/* Wait until STOPF flag is set */
	sEETimeout = sEE_LONG_TIMEOUT;
	while(I2C_ReadStatusFlag(i2c, I2C_FLAG_STOP) == RESET)
	{
		if((sEETimeout--) == 0) return sEE_FAIL;
	}   

	/* Clear STOPF flag */
	I2C_ClearStatusFlag(i2c, I2C_FLAG_STOP);

	/* If all operations OK, return sEE_OK (0) */
	return sEE_OK;
}

