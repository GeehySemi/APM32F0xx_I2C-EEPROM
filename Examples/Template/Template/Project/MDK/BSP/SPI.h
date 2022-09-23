
#define __W25QXX_H
#include "apm32f0xx_gpio.h"
#include "apm32f0xx_rcm.h"
#include <stdint.h>
/************W25QXXоƬIO����************/   
#define W25QXX_CSL	GPIO_ClearBit(GPIOB, GPIO_PIN_12);
#define W25QXX_CSH	GPIO_SetBit(GPIOB, GPIO_PIN_12);
/************W25QXXоƬʶ����************/   
extern uint16_t W25QXX_TYPE;		//W25QXX����ID
#define W25QXX_SSZIE 4096	//W25QXX������С
#define W25QXX_PSZIE 256	//W25QXXҳ��С
#define W25Q80 	0XEF13 	//����1M�ֽ�
#define W25Q16 	0XEF14	//����2M�ֽ�
#define W25Q32 	0XEF15	//����4M�ֽ�
#define W25Q64 	0XEF16	//����8M�ֽ�
#define W25Q128	0XEF17	//����16M�ֽ�
#define W25Q256 0XEF18	//����32M�ֽ�
/************W25QXX����ָ���************/
#define W25X_WriteEnable			0x06 
#define W25X_WriteDisable			0x04 
#define W25X_ReadStatusReg1		0x05 
#define W25X_ReadStatusReg2		0x35 
#define W25X_ReadStatusReg3		0x15 
#define W25X_WriteStatusReg1  0x01 
#define W25X_WriteStatusReg2  0x31 
#define W25X_WriteStatusReg3  0x11 
#define W25X_ReadData					0x03 
#define W25X_FastReadData			0x0B 
#define W25X_FastReadDual			0x3B 
#define W25X_PageProgram			0x02 
#define W25X_BlockErase				0xD8 
#define W25X_SectorErase			0x20 
#define W25X_ChipErase				0xC7 
#define W25X_PowerDown				0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID					0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 
#define W25X_Enable4ByteAddr  0xB7
#define W25X_Exit4ByteAddr    0xE9
#define W25X_SetReadParam			0xC0 
#define W25X_EnterQPIMode     0x38
#define W25X_ExitQPIMode      0xFF
/************W25QXX����������************/

uint16_t spi_read_writeByte(uint8_t TXdata);
uint16_t w25qxx_read_id(void);		//��ȡ����ID
void w25qxx_read_data(uint32_t addr,uint8_t *pbuf,uint32_t lenth);//W25QXX��ȡ����
void w25qxx_EraseSector(uint32_t addr);
uint8_t w25qxx_read_SR1(void);		//��״̬�Ĵ���1 
void w25qxx_wirte_enable(void);		//дʹ�� 
void w25qxx_wait_busy(void);
void w25qxx_write_page(uint32_t addr,uint8_t *pbuf,uint32_t lenth);
void w25qxx_NoCheck_write_page(uint32_t addr,uint8_t *pbuf,uint32_t lenth);
void W25QXX_Init(void);								//��ʼ��W25QXX

