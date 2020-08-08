/*
 * CAN.c
 *
 *  Created on: 2020年6月3日
 *      Author: wxujs
 */
#include "CAN.h"

/*Define CAN bus structure variables*/
CAN_DATA CANDATA;
uint8_t CANSTAT_Flag;
CAN_STATUS CANSTATUS;

/*
 * 解析CAN接收数据包的ID和数据格式
 */
void Analysys(CanRxMsg CANRX)
{
	switch (CANRX.ExtId)
	{
		case P1ID:
			Analysys_DATA(1, CANRX);
			CANSTAT_Flag=CANSTAT_Flag|0x01;
			break;
		case P2ID:
			Analysys_DATA(2, CANRX);
			CANSTAT_Flag=CANSTAT_Flag|0x02;
			break;
		case P3ID:
			Analysys_DATA(3, CANRX);
			CANSTAT_Flag=CANSTAT_Flag|0x04;
			break;
		case P4ID:
			Analysys_DATA(4, CANRX);
			CANSTAT_Flag=CANSTAT_Flag|0x08;
			break;
		case P5ID:
			Analysys_DATA(5, CANRX);
			CANSTAT_Flag=CANSTAT_Flag|0x10;
			break;
		case P6ID:
			Analysys_DATA(6, CANRX);
			CANSTAT_Flag=CANSTAT_Flag|0x20;
			break;
		case P7ID:
			Analysys_DATA(7, CANRX);
			CANSTAT_Flag=CANSTAT_Flag|0x40;
			break;
		case P8ID:
			Analysys_DATA(8, CANRX);
			CANSTAT_Flag=CANSTAT_Flag|0x80;
			break;
		default:
			break;
			//		default:
	}
}


/*
 * 分析数据包的ID
 * Pack_No:	包序号,对应第几个数据包,决定将本条数据解析到第几个结构体中,对应EXCEL表中数据序号(1~7)
 * CANRX:	接收的CAN总线数据结构体
 */
void Analysys_DATA(uint8_t Pack_NO, CanRxMsg CANRX)
{
	switch (Pack_NO)
	{
		case 1:
			CANDATA.HXCS = CANRX.Data[0];
			CANDATA.ZXCS = CANRX.Data[1];
			CANDATA.XLGWY = CANRX.Data[3] << 8;
			CANDATA.XLGWY += CANRX.Data[2];
			CANDATA.XLGZJ = CANRX.Data[5] << 8;
			CANDATA.XLGZJ += CANRX.Data[4];
			break;
		case 2:
			CANDATA.GTGD = CANRX.Data[1] << 8;
			CANDATA.GTGD += CANRX.Data[0];
			CANDATA.BHLGD = CANRX.Data[3] << 8;
			CANDATA.BHLGD += CANRX.Data[2];
			CANDATA.XGD = CANRX.Data[5] << 8;
			CANDATA.XGD += CANRX.Data[4];
			break;
		case 3:
			CANDATA.DPZQ = CANRX.Data[1] << 8;
			CANDATA.DPZQ += CANRX.Data[0];
			CANDATA.DPYQ = CANRX.Data[3] << 8;
			CANDATA.DPYQ += CANRX.Data[2];
			CANDATA.DPZH = CANRX.Data[5] << 8;
			CANDATA.DPZH += CANRX.Data[4];
			CANDATA.DPYH = CANRX.Data[7] << 8;
			CANDATA.DPYH += CANRX.Data[6];
			break;
		case 4:
			CANDATA.QJSD = CANRX.Data[3] << 8;
			CANDATA.QJSD += CANRX.Data[2];
			CANDATA.LZLL = CANRX.Data[5] << 8;
			CANDATA.LZLL += CANRX.Data[4];
			break;
		case 5:
			CANDATA.ZGDZS = CANRX.Data[1] << 8;
			CANDATA.ZGDZS += CANRX.Data[0];
			CANDATA.BHLZS = CANRX.Data[3] << 8;
			CANDATA.BHLZS += CANRX.Data[2];
			CANDATA.SSCZS = CANRX.Data[5] << 8;
			CANDATA.SSCZS += CANRX.Data[4];
			CANDATA.TLGT = CANRX.Data[7] << 8;
			CANDATA.TLGT += CANRX.Data[6];
			break;
		case 6:
			CANDATA.FJZS = CANRX.Data[1] << 8;
			CANDATA.FJZS += CANRX.Data[0];
			CANDATA.SLJLZS = CANRX.Data[3] << 8;
			CANDATA.SLJLZS += CANRX.Data[2];
			CANDATA.ZYJLZS = CANRX.Data[5] << 8;
			CANDATA.ZYJLZS += CANRX.Data[4];
			break;
		case 7:
			CANDATA.QXSS = CANRX.Data[1] << 8;
			CANDATA.QXSS += CANRX.Data[0];
			CANDATA.JDSS = CANRX.Data[3] << 8;
			CANDATA.JDSS += CANRX.Data[2];
			CANDATA.TLJX = CANRX.Data[4];
			CANDATA.DLBKD += CANRX.Data[5];
			CANDATA.JFKKD += CANRX.Data[6];
			CANDATA.YLKSD += CANRX.Data[7];
			break;
		case 8:
			CANDATA.GFKD = CANRX.Data[4] << 8;
			CANDATA.GFKD += CANRX.Data[3];
			CANDATA.HZL = CANRX.Data[5];
			CANDATA.PSL += CANRX.Data[6];
			break;
		default:
			break;
	}
}

CAN_DATA GetCanData()
{
	CANSTAT_Flag=0;
	CANSTATUS.Flashed_number=0;
	CANSTATUS.is_AllFlashed=false;
	return CANDATA;
}
CAN_STATUS CheckStatus_AfterLastGetData()
{
	uint8_t number=0,i=0;
	for(i=0;i<8;i++)
	{
		if(CANSTAT_Flag&(0x1<<i))
			number++;
	}
	CANSTATUS.Flashed_number=number;
	if(number==8)
		CANSTATUS.is_AllFlashed=true;
	return CANSTATUS;

}
