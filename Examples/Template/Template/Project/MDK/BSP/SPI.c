#ifndef _HH_
#define _HH_

#include "spi.h"
#include "main.h"
#include "apm32f0xx_spi.h"
#include "apm32f0xx_gpio.h"
#include "apm32f0xx_dma.h"
#endif /*_HH_*/



/*
 * ���ܣ�SPI ��дһ���ֽں���    ---�����ݽ���
 * ����������һ���ֽ�����
 * ����ֵ�����ض�ȡ������
*/
uint16_t spi_read_writeByte(uint8_t TXdata)
{
    while(SPI_ReadStatusFlag(SPI2,SPI_FLAG_TXBE) == RESET);   //�ȴ���һ�ε����ݷ���
    SPI_TxData8(SPI2,TXdata);//��������
     
//    while(SPI_ReadStatusFlag(SPI2,SPI_FLAG_RXBNE) == RESET);  //���ϴ����ݽ�����
    return SPI_RxData8(SPI2);//��������
}

//ID
//0XEF17,W25Q128 	  
uint16_t w25qxx_read_id(void)
{
    uint16_t id = 0;
     
	W25QXX_CSL; //Ƭѡ��Ч
     
    spi_read_writeByte(0x90);   //����0x90����ȡ����ID���豸ID
     
    spi_read_writeByte(0x00);   //����24λ��ַ(3���ֽ�)  ǰ�������ֽڿ������⣬�������ֽڱ�����0x00
    spi_read_writeByte(0x00);
    spi_read_writeByte(0x00);   //һ����0x00
 
    //��㷢2���ֽڵ�����
    id |= spi_read_writeByte(0xFF)<<8;  //id��0xEF17  ����ID��0xEF    
    id |= spi_read_writeByte(0xFF);     //�豸ID��0x17
	
		W25QXX_CSH; //Ƭѡ��Ч
     
    return id;
}

/*
 * ���ܣ�w25q128 ��ȡһ���ֽں���    ---�����ݽ���
 * ������addr     ----->����� addr �����ַ��ʼ��ȡ����
         pbuf     ----->��Ҫ��ȡ���������ڵĻ�����
         lenth    ----->��Ҫ��ȡ���ֽ���
 * ����ֵ�����ض�ȡ������
*/
void w25qxx_read_data(uint32_t addr,uint8_t *pbuf,uint32_t lenth)
{
    uint8_t *p = pbuf;

		W25QXX_CSL;   //Ƭѡ��Ч
	
    spi_read_writeByte(0x03);   //����0x03����ȡ��ȡ����
     
    //��������һ����Ҫ��ȡ��24λ��ַ   0xyy123456
    spi_read_writeByte((addr>>16)&0xFF);    //0x12   ����[23:16]
    spi_read_writeByte((addr>>8)&0xFF);    //0x34    ����[15:8]
    spi_read_writeByte((addr>>0)&0xFF);   //0x56    ����[7:0]
     
    while(lenth--)
    {
        *p++ = spi_read_writeByte(0xFF);    //����ӵģ����Ըĳ�����
    }
     
		W25QXX_CSH;     //Ƭѡ��Ч
}

/*
 * ���ܣ�w25q128  д��һҳ(256Byte)����    ---�����ݽ���
 * ������addr     ----->����� addr �����ַ��ʼ����
          
 * ����ֵ����
*/
void w25qxx_EraseSector(uint32_t addr)
{

		W25QXX_CSL;        //Ƭѡ��Ч
      
    spi_read_writeByte(0x20);   //����0x20����������
     
    //��������һ����Ҫ��ȡ��24λ��ַ   0xyy123456
    spi_read_writeByte((addr>>16)&0xFF);    //0x12   ����[23:16]
    spi_read_writeByte((addr>>8)&0xFF);   //0x34    ����[15:8]
    spi_read_writeByte((addr>>0)&0xFF);   //0x56    ����[7:0]
     
		W25QXX_CSH;        //Ƭѡ��Ч
}


/*
 * ���ܣ�w25q128  ��ȡ״̬�Ĵ���1
 * ��������          
 * ����ֵ��״̬�Ĵ��ֵ
*/
uint8_t w25qxx_read_SR1(void)
{
    uint16_t status = 0;

		W25QXX_CSL;   //Ƭѡ����

    spi_read_writeByte(0x05);       //����0x05����ȡ״̬�Ĵ���1��ֵ   ����0x35����ȡ״̬�Ĵ���2��ֵ
     
    status = spi_read_writeByte(0xFF);        //��������  ����״̬�Ĵ���1+״̬�Ĵ���2
     
		W25QXX_CSH;       //Ƭѡ����
     
    return  status;
}

void w25qxx_wirte_enable(void)
{

		W25QXX_CSL;   //Ƭѡ����

    spi_read_writeByte(0x06);   //����0x06��дʹ��

		W25QXX_CSH;   //Ƭѡ����
}

// �ж�״̬�Ĵ���1��S0Ϊ�Ƿ�Ϊ0��ֵΪ0��������
void w25qxx_wait_busy(void)
{
    while((w25qxx_read_SR1()&(0x01<<0)));   //��busyΪ0�����������  ��busyΪ1�����������ڼ���
}

/*
 * ���ܣ�w25q128  д��һҳ(256Byte)����    ---�����ݽ���
 * ������addr     ----->����� addr �����ַ��ʼд������
         pbuf     ----->��Ҫд����������ڵĻ�����
         lenth    ----->��Ҫд����ֽ���
 * ����ֵ�����ض�ȡ������
*/
void w25qxx_write_page(uint32_t addr,uint8_t *pbuf,uint32_t lenth)
{
    uint8_t *p = pbuf;

    w25qxx_wirte_enable();       //����֮ǰ�������дʹ��
     
    //��������
    //w25qxx_EraseSector(0x000000);
    w25qxx_EraseSector(addr/4096*4096);

    w25qxx_wait_busy();   //��æ

    w25qxx_wirte_enable();       //д��������дʹ��
     
    //��ʼд������
		W25QXX_CSL;   //Ƭѡ����

    spi_read_writeByte(0x02);   //����0x02��д������
     
    //��������һ��Ҫд���24λ��ַ   0xyy123456
    spi_read_writeByte((addr>>16)&0xFF);    //0x12   ����[23:16]
    spi_read_writeByte((addr>>8)&0xFF);    //0x34    ����[15:8]
    spi_read_writeByte((addr>>0)&0xFF);   //0x56    ����[7:0]
     
    while(lenth--)
    {
        spi_read_writeByte(*p++);
    }

		W25QXX_CSH;   //Ƭѡ����
}

/*
���ܣ��޼���д������,д��һ���������Զ���ҳ��
������addr     ----->����� addr �����ַ��ʼд������
     pbuf     ----->��Ҫд����������ڵĻ�����
    lenth    ----->��Ҫд����ֽ���
*/
void w25qxx_NoCheck_write_page(uint32_t addr,uint8_t *pbuf,uint32_t lenth)
{
	uint32_t page_remain = 256-addr%256;    //�����ʼд�����ݵ�ĳһҳ��ʣ��δд�ֽ���
	if(page_remain>=lenth)    //��ҳʣ��Ŀռ��㹻д����Щ����ʱ
	{
		page_remain=lenth;    //��д�볤�ȸ�ֵ
	}
	while(1)
	{
		w25qxx_write_page(addr,pbuf,page_remain);
		if(page_remain==lenth) break;   //�ж��Ƿ�д�꣬���ʣ��Ŀռ��㹻д����Щ���ݣ���ִ��һ������ĺ������������������������䡣
		
		addr+=page_remain;    //��������˵��len��page_remain�����ַƫ���Ѿ�д��ĳ���
		pbuf+=page_remain;    //����ƫ��
		lenth-=page_remain;   //�ܳ��ȼ�ȥ�Ѿ�д��ĳ���
		if(lenth<256)   //�ж�ʣ�೤���Ƿ�дһ��ҳ�������͸�ֵʵ��ʣ�೤�ȣ����͸�ֵ256
		{
			page_remain=lenth;
		}
		else
		{
			page_remain=256;
		}
	}
}


//W25QXX SPI�ӿڶ���  PA3->CS 
void W25QXX_Init(void)
{ 

	GPIO_Config_T gpioConfig;
	SPI_Config_T spiConfig;

	
	/** Enable related Clock */
	RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_SPI2);
	RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOB);

	
	//PB12
	gpioConfig.mode =GPIO_MODE_OUT;
	gpioConfig.outtype =GPIO_OUT_TYPE_PP;
	gpioConfig.pin =GPIO_PIN_12;
	gpioConfig.pupd = GPIO_PUPD_NO;
	gpioConfig.speed = GPIO_SPEED_10MHz;
	GPIO_Config(GPIOB, &gpioConfig);

	W25QXX_CSH;			     			  //Ƭѡ��Ч
	
	/* ����SPI2����SCK��MISO �� MOSIΪ�����������ģʽ */
	gpioConfig.pin = GPIO_PIN_15 | GPIO_PIN_14 | GPIO_PIN_13;
	gpioConfig.speed = GPIO_SPEED_50MHz;
	gpioConfig.mode =GPIO_MODE_OUT;
	gpioConfig.outtype =GPIO_OUT_TYPE_PP;
	GPIO_Config(GPIOB, &gpioConfig);

/***************************************************************/

GPIO_ConfigPinAF(GPIOB,GPIO_PIN_SOURCE_12,GPIO_AF_PIN0);

GPIO_ConfigPinAF(GPIOB,GPIO_PIN_SOURCE_13,GPIO_AF_PIN0);

GPIO_ConfigPinAF(GPIOB,GPIO_PIN_SOURCE_14,GPIO_AF_PIN0);

GPIO_ConfigPinAF(GPIOB,GPIO_PIN_SOURCE_15,GPIO_AF_PIN0);
/***************************************************************/

	SPI_ConfigStructInit(&spiConfig);

	spiConfig.length = SPI_DATA_LENGTH_8B;

	spiConfig.baudrateDiv = SPI_BAUDRATE_DIV_32;
	/**  2 line full duplex  */
	spiConfig.direction = SPI_DIRECTION_2LINES_FULLDUPLEX;//˫��ȫ˫��ͨ��
	/**  MSB first  */
	spiConfig.firstBit = SPI_FIRST_BIT_MSB;//��λ�ȳ������ӻ��������ֲ��ʱ��ͼ��
	/**  MASTER mode  */
	spiConfig.mode = SPI_MODE_MASTER;//Ĭ����������ɫ���������ƴӻ�
	/**  Polarity is HIGH  */
	spiConfig.polarity = SPI_CLKPOL_HIGH; //SPI���߿��е�ʱ��ʱ����Ϊ�ߵ�ƽ  CPOL=1�������ӻ��������ֲ��ʱ��ͼ��
	/**  Software select slave enable  */
	spiConfig.slaveSelect  = SPI_SSC_ENABLE;
	/**  Phase is 2 edge  */
	spiConfig.phase = SPI_CLKPHA_2EDGE; //CPHA = 1,�����������MOSI���Ž��е�ƽ������ʱ�ӵĵڶ��������ء����ӻ��������ֲ��ʱ��ͼ��
	/**  SPI config  */
	SPI_Config(SPI2, &spiConfig);

	SPI_ConfigDatalength(SPI2, SPI_DATA_LENGTH_8B);

	SPI_Enable(SPI2);
	
}  
