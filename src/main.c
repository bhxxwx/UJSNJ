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
char Deviceerr = 0;														//4G设备故障
int time_count = 0;												//Global variable ———— timing time
extern uint8_t x_axis, y_axis, cmd_axis;
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
		Get_CAN_data(CAN_Buffer_1, CAN_Buffer_2, CAN_Buffer_3, CAN_Buffer_4, CAN_Buffer_5,
		        CAN_Buffer_6, CAN_Buffer_7);							//Get CAN bus data packet
		while (!(CAN_Buffer_1.ATW == false || CAN_Buffer_2.ATW == false			//有一个CAN包被刷新，则退出循环
		|| CAN_Buffer_3.ATW == false || CAN_Buffer_4.ATW == false || CAN_Buffer_5.ATW == false
		        || CAN_Buffer_6.ATW == false || CAN_Buffer_7.ATW == false))
		{
			Get_CAN_data(CAN_Buffer_1, CAN_Buffer_2, CAN_Buffer_3, CAN_Buffer_4, CAN_Buffer_5,
			        CAN_Buffer_6, CAN_Buffer_7);			//Get CAN bus data packet
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

void Send_save_message()
{
	while (1)
	{
		cmd_axis = 3;													//cmd_axis在send_cmd函数使用后自加1
		x_axis = 0;
		y_axis = 0;
		send_cmd("AT+QMTPUB=0,0,0,1,\"/a1f2CH9BSx7/ZRH_4G/user/put\" \r\n");
		delay_us(100000); //0.1s
		send_cmd("{\"adc\":5} \r\n");
//		vTaskDelay(50);
		delay_us(100000); //0.1s
		if (check_receives(5, "+QMTPUB: 0,0,0"))
		{
			;
		} else
		{
			Deviceerr = 1;
			digitalWriteC(GPIO_Pin_14, HIGH);		 //连网指示灯灭
		}
		vTaskDelay(5000);    //单位2ms
	}

}

void Exception_handler()
{
	char error_handling = 0;				//标志位——错误处理，防止处理函数多重嵌套
	while (1)
	{
		if (Deviceerr == 1 && error_handling == 0)
		{
			error_handling = 1;
			Connect_MQTT_again();
		} else
		{
			error_handling = 0;
		}
		vTaskDelay(500);    //单位2ms
	}

}

int main(void)
{
	DBGMCU_Config(DBGMCU_IWDG_STOP, ENABLE); //DEBUG时看门狗关闭
	nvic_init();
	pinModeB(GPIO_Pin_5, OUTPUT);			 //IOT设备复位引脚
	pinModeC(GPIO_Pin_14, OUTPUT);			 //连网成功指示灯

	digitalWriteB(GPIO_Pin_5, HIGH);
	digitalWriteC(GPIO_Pin_14, HIGH);		 //低电平点亮
	delay_us(10000000);
	usart_1_init(115200);
	IOT_init();

//	xTaskCreate(CAN_Analysis, "GetCAN", 1024, NULL, 3, NULL);			//CAN解析线程
	xTaskCreate(Send_save_message, "sendtext", 1024, NULL, 2, NULL); 	//上传数据到阿里云且缓存数据
	xTaskCreate(Exception_handler, "handler", 1024, NULL, 4, NULL); 	//异常处理线程
	vTaskStartScheduler();												//开启多线程
	while (1)
	{

	}
}

