/*
 * Servers.c
 *
 *      Author: WangXiang
 */
#include "Servers.h"
#include "UserConfig.h"
#include <stdio.h>

/* Buffer for GPS parameter transmission */
extern GPS_DATA GPS_Buffer;
extern GPS_DATA GPSDATA;

void GPS_get_message()
{
	GPS_Buffer.UTCtime[10]=GPSDATA.UTCtime[10];
	GPS_Buffer.latitude[15]=GPSDATA.latitude[15];
	GPS_Buffer.longitude[15]=GPSDATA.longitude[15];
	GPS_Buffer.NorS=GPSDATA.NorS;
	GPS_Buffer.EorW=GPSDATA.EorW;
	GPS_Buffer.High[9]=GPSDATA.High[9];
	GPS_Buffer.ATW=GPSDATA.ATW;
//	GPS_Buffer.xlock=GPSDATA.xlock;
	GPS_Buffer.time_new_status=GPSDATA.time_new_status;
	GPS_Buffer.gps_new_status=GPSDATA.gps_new_status;
}
















