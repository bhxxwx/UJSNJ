/*
 * DATAstruct.h
 *
 *  Created on: 2020年6月2日
 *      Author: wxujs
 */

#ifndef DATASTRUCT_DATASTRUCT_H_
#define DATASTRUCT_DATASTRUCT_H_

typedef struct
{
	uint16_t HXCS;
	uint16_t ZXCS;
	uint16_t XLGWY;
	uint16_t XLGZJ;

	uint16_t GTGD;
	uint16_t BHLGD;
	uint16_t XGD;

	uint16_t DPZQ;
	uint16_t DPYQ;
	uint16_t DPZH;
	uint16_t DPYH;

	uint16_t QJSD;
	uint16_t LZLL;

	uint16_t ZGDZS;
	uint16_t BHLZS;
	uint16_t SSCZS;
	uint16_t TLGT;

	uint16_t FJZS;
	uint16_t SLJLZS;
	uint16_t ZYJLZS;

	uint16_t QXSS;
	uint16_t JDSS;
	uint16_t TLJX;
	uint16_t DLBKD;
	uint16_t JFKKD;
	uint16_t YLKSD;

	uint16_t GFKD;
	uint16_t HZL;
	uint16_t PSL;

} CAN_DATA;

typedef struct
{
	bool is_AllFlashed;
	int8_t Flashed_number;
}CAN_STATUS;

typedef struct
{
	char UTCtime[10];
	char latitude[15];					//维度
	char longitude[15];					//经度
	char NorS;
	char EorW;
	char High[9];						//高度
	bool ATW;
	char SIspeed[7];					//国际标准单位速度
	char SpeedinKnots[6];					//速度,单位:节
	char CourseovergroundDegrees[6];
	char CourseovergroundTrue[6];
	char CourseovergroundMagnetic[6];
	char Status[2];
	char time_new_status;	 			//time时间刷新标志位。0未刷新，1刷新
	char gps_new_status;	 			//GPS刷新标志位。0未刷新，1刷新
} GPS_DATA;

/*
 * GPS初始化结构体
 */
typedef struct
{
	const char match1[5];
	const char match2[5];
	uint8_t splitTime;
	bool ATR;
} GPS_INIT;

#endif /* DATASTRUCT_DATASTRUCT_H_ */
