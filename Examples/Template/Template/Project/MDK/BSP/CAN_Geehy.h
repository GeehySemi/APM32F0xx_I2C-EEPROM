#ifndef __CANCAN_H
#define __CANCAN_H



#include "apm32f0xx_can.h"
#include "apm32f0xx_gpio.h"
#include "apm32f0xx_rcm.h"

void CAN_TX(void);
void CAN_GPIOInit(void);
void CAN_Normal(void);

#endif

