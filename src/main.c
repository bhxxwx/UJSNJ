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

char receives[15][10][100]; //��֧��19��ָ��(1~19)[0��������],ÿ��ָ��ɽ���5��������Ϣ(0~4),ÿ��������Ϣ�����98(0~97)[99��Ϊ������Ϣ�������ݼ�¼λ]
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

	void RTC_1s_it_init(); //RTC��ʱ1s��ʼ��
	void RTC_Handler(void (*temp_function)); //RTC�жϺ������ݺ���

//	CAN_INIT();	//��ʼ��CAN����
//	CAN_IT_INIT();	//��ʼ��CAN�����ж�
//	delay_us(9000000);	//15s
//	printf("ok");
//	BC28_Init();
	GPSDATA.ATW = true;
	int count = 0;
	while (1)
	{
//		Pack1.Mail_Box[1].ATW = true;	//����1�Ű�����
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
//	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)	//���յ�����
//	{
//		USART_ClearITPendingBit(USART1, USART_IT_RXNE); //��������жϱ�־
//		temp = USART_ReceiveData(USART1); //���մ���1���ݵ�buff������
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
	if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET) //ע�⣡����ʹ��if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)���ж�
	{
		USART_ReceiveData(USART3);
	}
	//���ڴ�����һ��ͨѶ�����յ����ݲ�����
	if (GPSINIT.ATR == true)
	{
		USART_ReceiveData(USART3);
		USART_ClearITPendingBit(USART3, USART_IT_RXNE); //��������жϱ�־
		return;
	}
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //���յ�����
	{
		datas[counts] = USART_ReceiveData(USART3);
		if (datas[counts] == '$')
			datas[0] = '$';
		if (datas[counts] == '\n')
		{
			counts = 0;
			USART_ClearITPendingBit(USART3, USART_IT_RXNE); //��������жϱ�־
			GPSINIT.ATR = true;
			GPSINIT.splitTime=0;
			return;
		}
		counts++;
		USART_ClearITPendingBit(USART3, USART_IT_RXNE); //��������жϱ�־
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

