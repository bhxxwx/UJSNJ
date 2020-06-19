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

/*标志位、变量定义*/
char CANerr = 0;														//CAN bus interrupt flag
char GPSerr = 0;														//GPS interrupt flag
int time_count = 0;														//Global variable ———— timing time
#define Timeout_time 2													//Constant —— defines the timeout time

/*RTC timing time*/
void time_break_function()
{
	time_count++;
}

/* Buffer for CAN bus parameter transmission */
CAN_buffer1 CAN_Buffer_1;
CAN_buffer2 CAN_Buffer_2;
CAN_buffer3 CAN_Buffer_3;
CAN_buffer4 CAN_Buffer_4;
CAN_buffer5 CAN_Buffer_5;
CAN_buffer6 CAN_Buffer_6;
CAN_buffer7 CAN_Buffer_7;

/*Operating system thread 1 for CAN*/
void CAN_Analysis()
{
	while (1)
	{
		Clear_CAN_Box();								//清零CAN数据包，防止旧数据干扰
		SetFalgATW();									//允许CAN总线刷新获取数据
		Get_CAN_data(CAN_Buffer_1,CAN_Buffer_2,CAN_Buffer_3,CAN_Buffer_4,CAN_Buffer_5,CAN_Buffer_6,CAN_Buffer_7);//Get CAN bus data packet
		while (!(CAN_Buffer_1.ATW == false || CAN_Buffer_2.ATW == false			//有一个CAN包被刷新，则退出循环
		        || CAN_Buffer_3.ATW == false || CAN_Buffer_4.ATW == false
		        || CAN_Buffer_5.ATW == false || CAN_Buffer_6.ATW == false
		        || CAN_Buffer_7.ATW == false))
		{
			Get_CAN_data(CAN_Buffer_1,CAN_Buffer_2,CAN_Buffer_3,CAN_Buffer_4,CAN_Buffer_5,CAN_Buffer_6,CAN_Buffer_7);//Get CAN bus data packet
			if (time_count > Timeout_time)
			{
				time_count = 0;
				CANerr = 1;
				break;
			}
			CANerr = 0;
		}

		vTaskDelay(100);    //单位2ms
	}
}

int main(void)
{
	DBGMCU_Config(DBGMCU_IWDG_STOP, ENABLE); //DEBUG时看门狗关闭
	nvic_init();
	pinModeB(GPIO_Pin_5, OUTPUT);			 //IOT设备复位引脚
	IOT_init();

	xTaskCreate(CAN_Analysis, "GetCAN", 1024, NULL, 3, NULL);
	vTaskStartScheduler();

	while (1)
	{

	}
}

