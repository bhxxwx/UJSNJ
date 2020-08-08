/*		江苏大学
 * 农业装备学院工程学院
 *
 * 收割机项目,使用FreeRTOS操作系统
 * 使用 Github 进行版本控制
 *
 * 本项目的UserLib将作为独立驱动项目<已完成与Servers文件的解耦工作>,请注意保持该文件夹的独立性!!
 *
 * 本项目所有标点均为英文标点!
 *
 * 所有非驱动服务函数请在Servers文件夹内开发,
 * 		其中:DATAstruct文件夹内为自定义的数据结构体,
 * 			 Functions文件夹内为服务函数.
 *
 * UserLib为底层驱动文件夹.
 *
 * Author: WangXiang
 * 20200610
 */
/*include*/
#include "stm32f10x.h"
#include <stdio.h>
#include <stddef.h>
#include "UserConfig.h"//用户库调用
#include "stm32f10x_spi.h"

/*操作系统库函数调用*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

///*标志位、变量定义*/
//char CANerr = 0;														//CAN bus interrupt flag
//char GPSerr = 0;														//GPS interrupt flag
//char IOTerr = 0;															//IOT interrupt flag
//char Historical_data = 0;													//Historical data flag
int time_count = 0;										//Global variable ———— CAN timing time
//#define Timeout_time 2													//Constant —— defines the timeout time
//char CAN_new_message = 0;								//Global variable ———— CAN get new message
//char GPS_new_message = 0;								//Global variable ———— GPS get new message

/*RTC timing time*/
void time_break_function()
{
	time_count++;
}

/* Buffer for CAN bus parameter transmission */


/* Buffer for GPS parameter transmission */
GPS_DATA GPS_Buffer[10];

/*Operating system thread 1 for CAN*/
//void CAN_Analysis()
//{
//	while (1)
//	{
//		Clear_CAN_Box();								//清零CAN数据包，防止旧数据干扰
//		SetCANFalgATW();									//允许CAN总线刷新获取数据
//		vTaskDelay(20);   								//单位2ms
//		Get_CAN_data(CAN_Buffer_1, CAN_Buffer_2, CAN_Buffer_3, CAN_Buffer_4, CAN_Buffer_5,
//		        CAN_Buffer_6, CAN_Buffer_7);   							//Get CAN bus data packet
//
//		while (!(CAN_Buffer_1.ATW == false || CAN_Buffer_2.ATW == false			//有一个CAN包被刷新，则退出循环
//		|| CAN_Buffer_3.ATW == false || CAN_Buffer_4.ATW == false || CAN_Buffer_5.ATW == false
//		        || CAN_Buffer_6.ATW == false || CAN_Buffer_7.ATW == false))
//		{
//			Get_CAN_data(CAN_Buffer_1, CAN_Buffer_2, CAN_Buffer_3, CAN_Buffer_4, CAN_Buffer_5,
//			        CAN_Buffer_6, CAN_Buffer_7);			//Get CAN bus data packet
//			if (time_count > Timeout_time)
//			{
//				time_count = 0;
//				CANerr = 1;
//				CAN_new_message = 1;						//Set CAN new message flag
//				break;
//			}
//			time_count = 0;
//			CAN_new_message = 1;							//Set CAN new message flag
//			CANerr = 0;
//		}
//		vTaskDelay(100);    //单位2ms
//	}
//}
//
///*Operating system thread 1 for GPS*/
//void GPS_Analysis()
//{
//	while (1)
//	{
////		while (GPS_Buffer.xlock == 1)
////		{
////			vTaskDelay(20);   								//单位2ms,释放线程，等待上报函数释放GPS buffer
////		}
////		GPS_set_xlock();									//对GPS buffer上锁，防止上报脏数据
//		GPS_Begin_analysis();								//GPS允许解析数据
//		anaGPS();											//GPS analysis
//		GPS_get_message();									//Refresh GPS
//		while (!GPS_Buffer.ATW == false)
//		{
//			anaGPS();
//			GPS_get_message();
//			if (time_count > Timeout_time)
//			{
//				time_count = 0;
//				GPSerr = 1;
//				GPS_new_message = 1;						//Set GPS new message flag
//				break;
//			}
//			time_count = 0;
//			GPS_new_message = 1;							//Set GPS new message flag
//			GPSerr = 0;
//		}
////		GPS_clear_xlock();									//对GPS buffer解锁，允许上报函数读取
//		vTaskDelay(100);    								//单位2ms
//	}
//}
//
///*Operating system thread 1 for upload or store*/
//void upload_or_store()
//{
//	while (1)
//	{
//		if ((GPS_new_message == 1 || CAN_new_message == 1) && IOTerr == 0)	//CAN,GPS有一个刷新完成就上报
//		{
//			pack_to_aliyun();
//		} else if ((GPS_new_message == 1 || CAN_new_message == 1) && IOTerr == 1)	//IOT掉线，写入SPI
//		{
//
//		} else if (GPS_new_message == 0 && CAN_new_message == 0 && IOTerr == 0
//		        && Historical_data == 1)		//上报历史数据
//		{
//
//		}
//		vTaskDelay(10);    								//单位2ms,每20ms请求一次上报
//	}
//}
//
///*Operating system thread 1 for abnormal detection*/
//void Abnormal_detection()
//{
//	if (IOTerr == 1)
//	{
//		IOT_init();
//	}
//}
//
//void test()
//{
////	printf("11");
//	delay_us(10);
//	TIM_ClearFlag(TIM7, TIM_FLAG_Update);
//}
//
//__IO uint8_t first_v;
//__IO uint8_t second_v;
//void test1()
//{
//	while(1)
//	{
//		USB_printf("Simulation Device Count=%d\r\n",first_v);
//		vTaskDelay(1);
//	}
//}


int main(void)
{
	DBGMCU_Config(DBGMCU_IWDG_STOP, ENABLE); //DEBUG时看门狗关闭
	nvic_init();


	/*------------------------------SPI FLASH使用示例👇------------------------------------*/
//	char receives[257];
//	for(cc=0;cc<256;cc++)
//		receives[cc]='1';
//	SPI_INIT();//SPI协议初始化
//	SPI_FlashReset();//SPI-Flash复位
//	SPI_EraseChip();//SPI-Flash全片擦除
//	SPI_FlashFindHeadPage();//查找上次断电前保存的最后写入位置
//	int number = 0;
//	for (number = 0; number < 65; number++)
//		SPI_printf("hello,this is the %d message at page %d sector %d", number + 1,17 + number, (16 + number) / 16 + 1);
//	SPI_FlashLostPower();//如果即将断电,调用该函数保存最后一次写入位置
//	SPI_FlashReadPage(17, receives);
	/*------------------------------SPI FLASH使用示例👆------------------------------------*/

	//BSP_TimOpen(TIM_7, 36000 - 1, 1000 * 2 - 1, test);//定时器示例函数
	/*------------------------------GPS使用示例👇------------------------------------------*/
	GPS_init();
	int i=1;
	while (1)
	{
		GPS_Begin_analysis();
		GPS_Buffer[0]=anaGPS();
		if(GPS_Buffer[0].UTCtime[7]=='0')
		{
			GPS_Buffer[1]=GPS_Buffer[0];
		}
		if(GPS_Buffer[0].UTCtime[7]=='1')
		{
			GPS_Buffer[2]=GPS_Buffer[0];
		}
		if(GPS_Buffer[0].UTCtime[7]=='2')
		{
			GPS_Buffer[3]=GPS_Buffer[0];
		}
		if(GPS_Buffer[1].UTCtime[7]=='0'&&GPS_Buffer[2].UTCtime[7]=='1'&&GPS_Buffer[3].UTCtime[7]=='2')
			i++;


	}
	/*------------------------------GPS使用示例👆------------------------------------------*/

//	pinModeB(GPIO_Pin_5, OUTPUT);			 //IOT设备复位引脚
//	IOT_init();
//
//	xTaskCreate(CAN_Analysis, "GetCAN", 1024, NULL, 3, NULL);
//	xTaskCreate(GPS_Analysis, "GetGPS", 1024, NULL, 3, NULL);
//	xTaskCreate(upload_or_store, "upstore", 1024, NULL, 2, NULL);
//	xTaskCreate(Abnormal_detection, "err", 1024, NULL, 4, NULL);
//	xTaskCreate(test1, "err", 1024, NULL, 4, NULL);
//	vTaskStartScheduler();
//	while (1)
//	{
//
//	}
}

