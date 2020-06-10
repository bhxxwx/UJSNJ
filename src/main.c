/*		姹熻嫃澶у
 * 鍐滀笟瑁呭瀛﹂櫌宸ョ▼瀛﹂櫌
 *
 * 鏀跺壊鏈洪」鐩�,浣跨敤FreeRTOS鎿嶄綔绯荤粺
 * 浣跨敤 Github 杩涜鐗堟湰鎺у埗
 *
 * Author: WangXiang
 * 20200610
 *
 *///hahahaha
//THIS IS ZRH 123
/*include*/
#include "stm32f10x.h"
#include <stdio.h>
#include <stddef.h>
#include "UserConfig.h"//鐢ㄦ埛搴撹皟鐢�

/*鎿嶄綔绯荤粺搴撳嚱鏁拌皟鐢�*/
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



	void RTC_1s_it_init(); //RTC璁℃椂1s鍒濆鍖�
	void RTC_Handler(void (*temp_function)); //RTC涓柇鍑芥暟浼犻�掑嚱鏁�

//	CAN_INIT();	//鍒濆鍖朇AN鎬荤嚎
//	CAN_IT_INIT();	//鍒濆鍖朇AN鎬荤嚎涓柇
//	delay_us(9000000);	//15s
//	printf("ok");
//	BC28_Init();

	int count = 0;
	while (1)
	{
//		Pack1.Mail_Box[1].ATW = true;	//鎺ユ敹1鍙峰寘鏁版嵁
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



