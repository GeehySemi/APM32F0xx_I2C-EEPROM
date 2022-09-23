#include "apm32f0xx_can.h"
#include "CAN_Geehy.h"
#include "apm32f0xx_gpio.h"
#include "apm32f0xx_rcm.h"
CAN_Tx_Message        TxMessage;
void CAN_GPIOInit(void)
{
    GPIO_Config_T gpioConfig;

    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOB);
    
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_8, GPIO_AF_PIN4);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_9, GPIO_AF_PIN4);

    gpioConfig.pin = GPIO_PIN_8 | GPIO_PIN_9;
    gpioConfig.mode = GPIO_MODE_AF;
    gpioConfig.outtype = GPIO_OUT_TYPE_PP;
    gpioConfig.pupd = GPIO_PUPD_NO;
    gpioConfig.speed = GPIO_SPEED_50MHz;
    GPIO_Config(GPIOB, &gpioConfig);
}


//void CANI(void){

//	CAN_Config_T          canConfig;
//    CAN_FilterConfig_T    filterConfig;
//    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_CAN);

//    CAN_Reset();
////	RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_CAN);
//    /** Enable FIF0 message pending interrupt */
////    CAN_EnableInterrupt(CAN_INT_F0MP);

//    /** CAN configuration */
//    CAN_ConfigStructInit(&canConfig);
//    canConfig.autoBusOffManage = DISABLE;/*自动总线关闭处理*/
//    canConfig.autoWakeUpMode   = DISABLE;/*自动唤醒模式*/
//    canConfig.nonAutoRetran    = DISABLE ;/*无自动重传*/
//    canConfig.timeTrigComMode  = DISABLE;/*时间触发通信模式，失能*/
//    canConfig.txFIFOPriority   = DISABLE;/*传输FIFO优先级*//*0*/
//    canConfig.rxFIFOLockMode   = DISABLE;/*接收FIFO锁定模式*/
//    canConfig.mode             = CAN_MODE_NORMAL;
//    canConfig.syncJumpWidth    = CAN_SJW_1;
//    canConfig.timeSegment1     = CAN_TIME_SEGMENT1_8;
//    canConfig.timeSegment2     = CAN_TIME_SEGMENT2_7;
//    canConfig.prescaler        =3;  /*1M 波特率*/
//    CAN_Config(&canConfig);

//    /** CAN filter configuration */
//    filterConfig.filterNumber     = 0;
//    filterConfig.filterMode       = CAN_FILTER_MODE_IDMASK;
//    filterConfig.filterScale      = CAN_FILTER_SCALE_32BIT;
//    filterConfig.filterIdHigh     = 0;
//    filterConfig.filterIdLow      = 0;
//    filterConfig.filterMaskIdHigh = 0;
//    filterConfig.filterMaskIdLow  = 0;
//    filterConfig.filterFIFO       = 0;
//    filterConfig.filterActivation = ENABLE;
//    CAN_ConfigFilter(&filterConfig);
//}
void CAN_Normal(void)
{
    CAN_Config_T          canConfig;
    CAN_FilterConfig_T    filterConfig;
    CAN_Tx_Message        TxMessage;

    CAN_GPIOInit();
    CAN_Reset();

    /** Enable FIF0 message pending interrupt */
    CAN_EnableInterrupt(CAN_INT_F0MP);

    /** CAN configuration */
    CAN_ConfigStructInit(&canConfig);
    canConfig.autoBusOffManage = DISABLE;
    canConfig.autoWakeUpMode   = DISABLE;
    canConfig.nonAutoRetran    = DISABLE;
    canConfig.timeTrigComMode  = DISABLE;
    canConfig.txFIFOPriority   = DISABLE;
    canConfig.rxFIFOLockMode   = DISABLE;
    canConfig.mode             = CAN_MODE_NORMAL;
    canConfig.syncJumpWidth    = CAN_SJW_1;
    canConfig.timeSegment1     = CAN_TIME_SEGMENT1_7;
    canConfig.timeSegment2     = CAN_TIME_SEGMENT2_8;
    canConfig.prescaler        = 6;
    CAN_Config(&canConfig);

    /** CAN filter configuration */
    filterConfig.filterIdHigh     = 0x0000;
    filterConfig.filterIdLow      = 0x0000;
    filterConfig.filterMaskIdHigh = 0x0000;
    filterConfig.filterMaskIdLow  = 0x0000;
    filterConfig.filterFIFO       = CAN_FIFO_0;
    filterConfig.filterNumber     = CAN_FILTER_NUMBER_1;
    filterConfig.filterMode       = CAN_FILTER_MODE_IDMASK;
    filterConfig.filterScale      = CAN_FILTER_SCALE_32BIT;
    filterConfig.filterActivation = ENABLE;
    CAN_ConfigFilter(&filterConfig);

    /** CAN Tx Message configuration */
    TxMessage.stanID         = 0x0000;
    TxMessage.extenID        = 0x1111;
    TxMessage.typeID         = CAN_TYPEID_EXT;
    TxMessage.remoteTxReq    = CAN_RTXR_DATA;
    TxMessage.dataLengthCode = 2;
    TxMessage.data[0]        = 0x55;
    TxMessage.data[1]        = 0xAA;
    CAN_TxMessage(&TxMessage);
}

void CAN_TX(void){

    /** CAN Tx Message configuration */
    TxMessage.stanID         = 0x321;
    TxMessage.typeID         = CAN_TYPEID_STD;
    TxMessage.remoteTxReq    = CAN_RTXR_DATA;
    TxMessage.dataLengthCode = 8;
    TxMessage.data[0]        = 0x55;
    TxMessage.data[1]        = 0xAA;
    TxMessage.data[2]        = 0x55;
	TxMessage.data[3]        = 0x1A;
    TxMessage.data[4]        = 0x03;
    TxMessage.data[5]        = 0xAA;
    TxMessage.data[6]        = 0x55;
    TxMessage.data[7]        = 0xAA;
    CAN_TxMessage(&TxMessage);

}
