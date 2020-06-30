/*
 * GPS.c
 *
 *  Created on: 2020年6月3日
 *      Author: wxujs
 */
#include "GPS.h"

enum
{
	err = 0, GPGGA, GPVTG,
};

void GPS_init()
{
	usart_3_init(9600, 1);
}

char GpsCharToConvert[20], datas[100];

GPS_DATA GPSDATA;
GPS_INIT GPSINIT = { .matchCount = 0, .cmdHead = false, .match1[0]='G', .match1[1]='P', .match1[2
        ]='G', .match1[3]='G', .match1[4]='A', .match2[0]='G', .match2[1]='P', .match2[2]='V',
        .match2[3]='T', .match2[4]='G', .splitTime = 0, .dataCount = 0 };

char GpstempChar;
char GpsCharToConvert[20];
char datas[100];

/* GPS begin analysis*/
void GPS_Begin_analysis()
{
	GPSDATA.ATW = true;
}

///* GPS set x lock flag*/
//void GPS_set_xlock()
//{
//	GPSDATA.xlock = 1;
//}
//
///* GPS clear x lock flag*/
//void GPS_clear_xlock()
//{
//	GPSDATA.xlock = 0;
//}
//
///* GPS read x lock flag*/
//bool GPS_read_xlock()
//{
//	return GPSDATA.xlock;
//}

void USART3_IRQHandler(void)
{
	if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET)
	{
		USART_ReceiveData(USART3);
		return;
	}
	//正在处理上一条通讯，接收到数据不处理
	if (GPSINIT.ATR == true)
	{
		USART_ReceiveData(USART3);
		USART_ClearITPendingBit(USART3, USART_IT_RXNE); //清除接收中断标志
		return;
	}
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //接收到数据
	{
		static uint8_t counts = 0;
		datas[counts] = USART_ReceiveData(USART3);
		if (datas[counts] == '$')
			datas[0] = '$';
		if (datas[counts] == '\n')
		{
			counts = 0;
			USART_ClearITPendingBit(USART3, USART_IT_RXNE); //清除接收中断标志
			GPSINIT.ATR = true;
			return;
		}
		counts++;
		USART_ClearITPendingBit(USART3, USART_IT_RXNE); //清除接收中断标志
	}
}

void writeUTC(char *time, char *data)
{
	time[0] = data[0];
	time[1] = data[1];
	time[2] = ':';
	time[3] = data[2];
	time[4] = data[3];
	time[5] = ':';
	time[6] = data[4];
	time[7] = data[5];
}

void writeL(char *lan, char *data)
{
	int i = 0;
	for (; data[i] != '\0'; i++)
	{
		lan[i] = data[i];
	}
}

void clearStr(char *str, uint8_t i)
{
	uint8_t j = 0;
	for (; j < i; j++)
	{
		str[j] = '\0';
	}
}

uint8_t CheckHead()
{
	uint8_t matchCount = 0;
	for (matchCount = 1; matchCount <= 5; matchCount++) //检测包头是否符合规范
	{
		if (datas[matchCount] != GPSINIT.match1[matchCount - 1])
		{
			clearStr(datas, 100);
			GPSINIT.ATR = false;
			GPSINIT.splitTime = 0;
			return err;
		}
	}
	return GPGGA;
	for (matchCount = 1; matchCount <= 5; matchCount++)
	{
		if (datas[matchCount] != GPSINIT.match2[matchCount - 1])
		{
			clearStr(datas, 100);
			GPSINIT.ATR = false;
			GPSINIT.splitTime = 0;
			return err;
		}
	}
	return GPVTG;
}

void anaGPS()
{
	int x, y;
	if (GPSDATA.ATW == false) //不允许写入缓冲区
	{
		return;
	}
	if (GPSINIT.ATR) //允许读取数据
	{
		GPSINIT.splitTime = 0; //分隔符计数
		clearStr(GpsCharToConvert, 20); //缓冲区清零
		GPSINIT.dataCount = 0;

//switch(CheckHead())
//{
//	case GPGGA:
//}

		for (x = 0, y = 0; datas[x] != '\r'; x++)
		{
			if (datas[x] != ',')
			{
				GpsCharToConvert[y] = datas[x];
				y++;
			} else
			{
				y = 0;

				GPSINIT.splitTime++;
				switch (GPSINIT.splitTime)
				{
					case 2:
						writeUTC(GPSDATA.UTCtime, GpsCharToConvert), clearStr(GpsCharToConvert, 20), GPSINIT.dataCount =
						        0;
						continue;
					case 3:
						GPSDATA.AorP = GpsCharToConvert[0], clearStr(GpsCharToConvert, 20), GPSINIT.dataCount =
						        0;
						continue;
					case 4:
						writeL(GPSDATA.latitude, GpsCharToConvert), clearStr(GpsCharToConvert, 20), GPSINIT.dataCount =
						        0;
						continue;
					case 5:
						GPSDATA.NorS = GpsCharToConvert[0], clearStr(GpsCharToConvert, 20), GPSINIT.dataCount =
						        0;
						continue;
					case 6:
						writeL(GPSDATA.longitude, GpsCharToConvert), clearStr(GpsCharToConvert, 20), GPSINIT.dataCount =
						        0;
						continue;
					case 7:
						GPSDATA.EorW = GpsCharToConvert[0], clearStr(GpsCharToConvert, 20), GPSINIT.dataCount =
						        0;
						continue;
					case 8:
						GPSDATA.ATW = false, GPSINIT.ATR = false, GPSINIT.splitTime = 0;
						return;
				}
				clearStr(GpsCharToConvert, 20);
				GPSINIT.dataCount = 0;
				continue;
			}
		}
	}
}

