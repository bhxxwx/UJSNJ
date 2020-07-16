/*
 * GPS.c
 *
 *  Created on: 2020年6月3日
 *      Author: wxujs
 */
#include "GPS.h"

/*GPS数据包枚举*/
enum
{
	err = 0, GNRMC, GNGGA,
};

/*<GpsCharToConvert>GPS截取的有效数据;<datas>单条GPS数据存放缓冲区*/
char GpsCharToConvert[20], datas[100];

/*GPS数据结构体与初始化结构体数据初始化*/
GPS_DATA GPSDATA;
GPS_INIT GPSINIT = { .match1[0]='G', .match1[1]='N', .match1[2]='R', .match1[3]='M', .match1[4]='C',
        .match2[0]='G', .match2[1]='N', .match2[2]='G', .match2[3]='G', .match2[4]='A', .splitTime =
                0, };

/*
 * GPS初始化
 * 	初始化串口3
 * 	波特率9600,优先级1
 */
void GPS_init()
{
	usart_3_init(9600, 1);
}


/*
 * 允许将解析到的数据写入GPS数据结构体
 */
void GPS_Begin_analysis()
{
	GPSDATA.ATW = true;
}

/*
 * 串口3中断服务函数
 */
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
		__IO uint8_t counts = 0;
		datas[counts] = USART_ReceiveData(USART3);
		if (datas[counts] == '$')
			datas[0] = '$';
		if (datas[counts] == '\n')
		{
			counts = 0;
			USART_ClearITPendingBit(USART3, USART_IT_RXNE); //清除接收中断标志
			GPSINIT.ATR = true; //缓冲区更新完成允许读取
			return;
		}
		counts++;
		USART_ClearITPendingBit(USART3, USART_IT_RXNE); //清除接收中断标志
	}
}

/*
 * GPS写UTC时间
 * 	原数据:data
 * 	目标写入位置:time
 */
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

/*
 * 写GPS数据
 * 	从data -> lan
 */
void writeL(char *lan, char *data)
{
	uint8_t i = 0;
	for (; data[i] != '\0'; i++)
	{
		lan[i] = data[i];
	}
}

/*
 * 复制数据
 * 	从data -> speed
 */
void copyData(char *speed, char *data)
{
	uint8_t i = 0;
	for (; data[i] != '\0'; i++)
		speed[i] = data[i];
}

/*
 * 清空指定长度数组内容
 */
void clearStr(char *str, uint8_t i)
{
	uint8_t j = 0;
	for (; j < i; j++)
	{
		str[j] = '\0';
	}
}

/*
 * GPS数据包包头检测
 * 返回枚举类型
 */
uint8_t CheckHead()
{
	uint8_t matchCount = 0;
	for (matchCount = 1; matchCount <= 5; matchCount++) //检测包头是否符合规范
	{
		if ((datas[matchCount] != GPSINIT.match1[matchCount - 1])
		        && (datas[matchCount] != GPSINIT.match2[matchCount - 1]))
		{
			clearStr(datas, 100);
			GPSINIT.ATR = false; //缓冲区数据作废,不允许再次读取,请求串口更新缓冲区
			GPSINIT.splitTime = 0;
			return err;
		}
	}
	if (datas[4] == 'M')
		return GNRMC;
	if (datas[4] == 'T')
		return GNGGA;
	return err;
}

/*
 * 解析GPS数据
 * 解析条件:
 * 		ATW为true:允许更新数据到结构体中,通过GPS_Begin_analysis()函数置位,
 * 		ATR为true:缓冲区数据已经更新
 */
void anaGPS()
{
	if (GPSDATA.ATW == false) //不允许写入数据结构体,GPS不解析
	{
		return;
	}
	if (GPSINIT.ATR) //允许读取数据,缓冲区数据更新完成
	{
		GPSINIT.splitTime = 0; //分隔符计数
		clearStr(GpsCharToConvert, 20); //缓冲区清零

		if(CheckHead()==GNRMC)
			DecodeRMC();
		if(CheckHead()==GNGGA)
			DecodeGGA();
	}
}

/*
 * 解析GNRMC数据包,并更新解析数据到数据结构体
 */
void DecodeRMC()
{
	uint8_t x, y;
	for (x = 7, y = 0; datas[x] != '\r'; x++)
	{
		if (datas[x] != ',') //如果数据不为',',复制数据
		{
			GpsCharToConvert[y] = datas[x];
			y++;
		} else //如果数据为',',判断为第几个逗号
		{
			y = 0;
			GPSINIT.splitTime++;
			switch (GPSINIT.splitTime)
			{
				case 1:
					writeUTC(GPSDATA.UTCtime, GpsCharToConvert), clearStr(GpsCharToConvert, 20);
					continue;
				case 3:
					writeL(GPSDATA.latitude, GpsCharToConvert), clearStr(GpsCharToConvert, 20);
					continue;
				case 4:
					GPSDATA.NorS = GpsCharToConvert[0], clearStr(GpsCharToConvert, 20);
					continue;
				case 5:
					writeL(GPSDATA.longitude, GpsCharToConvert), clearStr(GpsCharToConvert, 20);
					continue;
				case 6:
					GPSDATA.EorW = GpsCharToConvert[0], clearStr(GpsCharToConvert, 20);
					continue;
				case 7:
					copyData(GPSDATA.SpeedinKnots, GpsCharToConvert), clearStr(GpsCharToConvert, 20);
					continue;
				case 8:
					copyData(GPSDATA.CourseovergroundDegrees, GpsCharToConvert), clearStr(
					        GpsCharToConvert, 20);
					continue;
				case 9:
					GPSDATA.ATW = false, GPSINIT.ATR = false, GPSINIT.splitTime = 0;
					return;
			}
			clearStr(GpsCharToConvert, 20); //若没匹配到switch规则,清空缓存
			continue;
		}
	}
}

/*
 * 解析GNGGA数据包,并更新解析数据到数据结构体
 */
void DecodeGGA()
{
	uint8_t x, y;
	GPSINIT.splitTime = 0;
	for (x = 7, y = 0; datas[x] != '\r'; x++)
	{
		if (datas[x] == ',')
		{
			GPSINIT.splitTime++;
			continue;
		}
		if (GPSINIT.splitTime == 8 && datas[x] != ',') //如果数据不为',',复制数据
		{
			GpsCharToConvert[y] = datas[x];
			y++;
		}
		if (GPSINIT.splitTime == 9)
			copyData(GPSDATA.High, GpsCharToConvert);
		clearStr(GpsCharToConvert, 20);
	}
}
