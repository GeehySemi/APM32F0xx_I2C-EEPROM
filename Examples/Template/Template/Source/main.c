/*!
 * @file        main.c
 *
 * @brief       Main program body
 *
 * @version     V1.0.1
 *
 * @date        2021-07-01
 *
 * @attention
 *
 *  Copyright (C) 2020-2022 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be usefull and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

#include "main.h"
#include "Board.h"
#include "apm32f0xx_rcm.h"
#include "apm32f0xx_usart.h"
#include "I2C.h"
#include <stdio.h>

#define DEBUG_USART  USART1

int fputc(int ch, FILE *f)
{
        /** send a byte of data to the serial port */
        USART_TxData(DEBUG_USART,(uint8_t)ch);

        /** wait for the data to be send  */
        while (USART_ReadStatusFlag(DEBUG_USART, USART_FLAG_TXBE) == RESET);

        return (ch);
}

void Delay(void)
{
    volatile uint32_t delay = 0x7ff3;

    while(delay--);
}


uint32_t read_num = 10 ;


int main(void)
{	
	uint8_t data[10]={0x1,0x2,0x3,0x4,0x05,0x06,0x07,0x08, 0x09, 0x0A};
	uint8_t Read_Buff[10]={0};

	int i;
	I2CInit();
  APM_MINI_COMInit(COM1);

    while(1)
    {

     Delay();
     sEE_WriteBuffer(I2C1,0xA0,200,data,10);
	 printf ("\r\nWrite successful\r\n");	
     Delay();
		
	 sEE_ReadBuffer(I2C1, 0xA0, 200, Read_Buff, (uint16_t *)&read_num);
	  printf ("\r\nread sucessful\r\n");	
	 Delay();
			
			for(i=0;i<10;i++)
			{
				printf ("\r\n第%d个数为0x%x",i,Read_Buff[i]);
			}

    }
}
