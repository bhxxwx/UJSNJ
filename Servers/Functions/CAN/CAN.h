/*
 * CAN.h
 *
 *  Created on: 2020年6月3日
 *      Author: wxujs
 */

#ifndef FUNCTIONS_CAN_CAN_H_
#define FUNCTIONS_CAN_CAN_H_
#include "UserConfig.h"

#define P1ID 0x0CFF1081
#define P2ID 0x10FF1182
#define P3ID 0x10FF1283
#define P4ID 0x10FF1384

void Analysys(CanRxMsg CANRX);
void Analysys_ID(uint8_t Pack_NO, CanRxMsg CANRX);
uint8_t set_Pri(uint32_t id);
void Analysys_DATA(uint8_t Pack_NO, CanRxMsg CANRX);

void SetFalgATW();
CANpacks GetCANRx();
uint8_t CheckATW();




#endif /* FUNCTIONS_CAN_CAN_H_ */
