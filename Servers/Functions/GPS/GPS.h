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
void GPS_init();
void GPS_set_xlock();
void GPS_clear_xlock();
bool GPS_read_xlock();
void GPS_Begin_analysis();
void anaGPS();


uint8_t CheckHead();


#endif /* FUNCTIONS_GPS_GPS_H_ */
