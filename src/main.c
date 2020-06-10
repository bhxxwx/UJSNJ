/*		江苏大学
 * 农业装备学院工程学院
 *
 * 收割机项目,使用FreeRTOS操作系统
 * 使用 Github 进行版本控制
 *
 * Author: WangXiang
 * 20200610
 *
 */

/*include*/
#include "stm32f10x.h"
#include <stdio.h>
#include <stddef.h>
#include "UserConfig.h"//用户库调用

/*操作系统库函数调用*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"



typedef enum
{
	ERR = 0, OK, ONLINE

} NB_STAT;








int main(void)
{
//	DBGMCU_IWDG_STOP;



	void RTC_1s_it_init(); //RTC计时1s初始化
	void RTC_Handler(void (*temp_function)); //RTC中断函数传递函数

//	CAN_INIT();	//初始化CAN总线
//	CAN_IT_INIT();	//初始化CAN总线中断
//	delay_us(9000000);	//15s
//	printf("ok");
//	BC28_Init();

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

//		while (GPSDATA.ATW == true)
//			;

			anaGPS();
		count++;

	}
}



