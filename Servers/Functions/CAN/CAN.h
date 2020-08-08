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

#define P1ID 0x18FF2111
#define P2ID 0x18FF2313
#define P3ID 0x18FF2413
#define P4ID 0x0CFF2715
#define P5ID 0x18FF2815
#define P6ID 0x18FF2915
#define P7ID 0x18FF2515
#define P8ID 0x0CFF2615
#define P9ID 0x18FF2817

void Analysys(CanRxMsg CANRX);
void Analysys_DATA(uint8_t Pack_NO, CanRxMsg CANRX);

CAN_DATA GetCanData();
CAN_STATUS CheckStatus_AfterLastGetData();




#endif /* FUNCTIONS_CAN_CAN_H_ */
