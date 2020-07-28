/*
 * GPS.h
 *
 *  Created on: 2020年6月3日
 *      Author: wxujs
 */

#ifndef FUNCTIONS_GPS_GPS_H_
#define FUNCTIONS_GPS_GPS_H_
#include "UserConfig.h"
#include "DATAstruct/DATAstruct.h"

#define GPS_Res_Pin GPIO_Pin_8
#define GPS_Erase_Pin GPIO_Pin_7

void writeUTC(char *time, char *data);
void writeL(char *lan, char *data);
void copyData(char *speed, char *data);
void clearStr(char *str, uint8_t i);
void copyData(char *speed, char *data);
uint8_t CheckHead();
void DecodeRMC();
void DecodeGGA();

/*-------------------GPS相关函数接口-------------------------*/
void GPS_init();
void GPS_Begin_analysis();
void GPS_Stop_analysis();
GPS_DATA anaGPS();


#endif /* FUNCTIONS_GPS_GPS_H_ */
