/*
 * CAN.h
 *
 *  Created on: 2020年6月3日
 *      Author: wxujs
 */

#ifndef FUNCTIONS_CAN_CAN_H_
#define FUNCTIONS_CAN_CAN_H_
#include "UserConfig.h"
#include "DATAstruct/DATAstruct.h"
#define P1ID 0x0CFF1081
#define P2ID 0x10FF1182
#define P3ID 0x10FF1283
#define P4ID 0x10FF1384

void Analysys(CanRxMsg CANRX);
void Analysys_ID(uint8_t Pack_NO, CanRxMsg CANRX);
uint8_t set_Pri(uint32_t id);
void Analysys_DATA(uint8_t Pack_NO, CanRxMsg CANRX);

void SetFalgATW();
void Clear_CAN_Box();
void Get_CAN_data(CAN_buffer1 CAN_Buffer_1, CAN_buffer2 CAN_Buffer_2, CAN_buffer3 CAN_Buffer_3,
        CAN_buffer4 CAN_Buffer_4, CAN_buffer5 CAN_Buffer_5, CAN_buffer6 CAN_Buffer_6,
        CAN_buffer7 CAN_Buffer_7);
CANpacks GetCANRx();
uint8_t CheckATW();
uint8_t get_pack_currentPack(CanRxMsg CANRX);





#endif /* FUNCTIONS_CAN_CAN_H_ */
