/*		江苏大学
 * 农业装备学院工程学院
 *
 * 收割机项目,使用FreeRTOS操作系统
 * 使用 Github 进行版本控制
 *
 * Author: WangXiang
 *
 */


/*include*/
#include "stm32f10x.h"
#include <stdio.h>
#include <stddef.h>
#include "UserConfig.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"



typedef enum
{
	ERR = 0, OK, ONLINE

} NB_STAT;

Cach1 Pack1;
Cach2 Pack2;
Cach3 Pack3;
Cach4 Pack4;
Cach5 Pack5;
Cach6 Pack6;
Cach7 Pack7;

char receives[15][10][100]; //可支持19条指令(1~19)[0舍弃不用],每条指令可接收5调返回信息(0~4),每条返回信息长度最长98(0~97)[99作为单条信息长度数据记录位]
uint8_t x_axis = 0, y_axis = 0, cmd_axis = 0;
int time_count = 0;
int count = 0;

GPS_DATA GPSDATA = { .ATW = false };
GPS_INIT GPSINIT = { .matchCount = 0, .cmdHead = false, .match[0]='G', .match[1]='N', .match[2]='R',
        .match[3]='M', .match[4]='C', .splitTime = 0, .dataCount = 0 };

void anaGPS();

int main(void)
{
	usart_1_init(9600);
	usart_3_init(9600, 1);

	void RTC_1s_it_init(); //RTC计时1s初始化
	void RTC_Handler(void (*temp_function)); //RTC中断函数传递函数

//	CAN_INIT();	//初始化CAN总线
//	CAN_IT_INIT();	//初始化CAN总线中断
//	delay_us(9000000);	//15s
//	printf("ok");
//	BC28_Init();
	GPSDATA.ATW = true;
	int count = 0;
	while (1)
	{
//		Pack1.Mail_Box[1].ATW = true;	//接收1号包数据
//		Pack2.Mail_Box[1].ATW = true;
//		Pack3.Mail_Box[1].ATW = true;
//		Pack4.Mail_Box[1].ATW = true;
//		Pack5.Mail_Box[1].ATW = true;
//		Pack6.Mail_Box[1].ATW = true;
//		Pack7.Mail_Box[1].ATW = true;
//		while (!(Pack1.Mail_Box[1].ATW == false && Pack2.Mail_Box[1].ATW == false
//		        && Pack3.Mail_Box[1].ATW == false && Pack4.Mail_Box[1].ATW == false
//		        && Pack5.Mail_Box[1].ATW == false && Pack6.Mail_Box[1].ATW == false
//		        && Pack7.Mail_Box[1].ATW == false))
//			;
//		{
//			pack_to_aliyun();
//		}
		GPSDATA.ATW = true;
//		while (GPSDATA.ATW == true)
//			;

			anaGPS();
		count++;

	}
}

//void USART1_IRQHandler(void)
//{
//	char temp;
//	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)	//接收到数据
//	{
//		USART_ClearITPendingBit(USART1, USART_IT_RXNE); //清除接收中断标志
//		temp = USART_ReceiveData(USART1); //接收串口1数据到buff缓冲区
//		receives[cmd_axis][y_axis][x_axis] = temp;
//		x_axis++;
//		if (temp == '\n')
//		{
//			receives[cmd_axis][y_axis][99] = x_axis;
//			y_axis++;
//			x_axis = 0;
//		}
//	}
//}

char GpstempChar;
char GpsCharToConvert[20];
char datas[100];
int counts = 0;
void USART3_IRQHandler(void)
{
//	GPSINIT.ATR=false;
	if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET) //注意！不能使用if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)来判断
	{
		USART_ReceiveData(USART3);
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
		datas[counts] = USART_ReceiveData(USART3);
		if (datas[counts] == '$')
			datas[0] = '$';
		if (datas[counts] == '\n')
		{
			counts = 0;
			USART_ClearITPendingBit(USART3, USART_IT_RXNE); //清除接收中断标志
			GPSINIT.ATR = true;
			GPSINIT.splitTime=0;
			return;
		}
		counts++;
		USART_ClearITPendingBit(USART3, USART_IT_RXNE); //清除接收中断标志
	}
}

void anaGPS()
{
	int x, y;
	if (GPSDATA.ATW == false)
	{
		return;
	}
	if (GPSINIT.ATR)
	{
		GPSINIT.splitTime = 0;
		clearStr(GpsCharToConvert, 20);
		GPSINIT.dataCount = 0;

		for (GPSINIT.matchCount = 1; GPSINIT.matchCount <= 5; GPSINIT.matchCount++)
		{
			if (datas[GPSINIT.matchCount] != GPSINIT.match[GPSINIT.matchCount - 1])
			{
				clearStr(datas, 100);
				GPSINIT.ATR = false;
				GPSINIT.splitTime=0;
				return;
			}
		}

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
						GPSDATA.ATW = false, GPSINIT.ATR = false,GPSINIT.splitTime=0;return;
				}
				clearStr(GpsCharToConvert, 20);
				GPSINIT.dataCount = 0;
				continue;
			}

		}
	}

}

