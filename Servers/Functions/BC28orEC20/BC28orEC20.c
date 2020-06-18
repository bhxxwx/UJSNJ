/*
 * BC28orEC20.c
 *
 *  Created on: 2020年6月2日
 *      Author: wxujs
 */

#include "BC28orEC20.h"

//可支持19条指令(1~19)[0另作他用],每条指令可接收10条返回信息(0~9),每条返回信息长度最长97(0~96)[98为溢出标记位][99作为单条信息长度数据记录位]
//char receives[20][10][100]; //BC28或者EC20返回数据的缓冲区
char receives[8][4][40];

/*	无符号8位整型变量
 *  x_axis:横轴计数,即串口单次接受的数据长度(以\r\n作为一条数据结束标记)
 *  y_axis:纵轴计数,即串口同一条命令下接受的数据条数(以\r\n作为一条数据结束标记)
 *  cmd_axis:z轴计数:即对应第几条命令命令
 */
uint8_t x_axis = 0, y_axis = 0, cmd_axis = 0;

/*
 * 清除物联网设备的命令接收缓冲区
 */
void ClearCmdBuffer()
{
	uint8_t x, y, z;
	for (z = 0; z < 20; z++)
		for (y = 0; y < 10; y++)
			for (x = 0; x < 100; x++)
				receives[z][y][x] = '\0';
}

/*
 * 单独物联网设备复位
 */
void IOT_Reset()
{
	digitalWriteB(GPIO_Pin_5, LOW);
	delay_us(3000000);
	digitalWriteB(GPIO_Pin_5, HIGH);
}

/*
 * 物联网设备和单片机先后进行复位
 */
void FullSystemReset()
{
	digitalWriteB(GPIO_Pin_5, LOW);
	delay_us(3000000);
	NVIC_SystemReset();
}

void send_cmd(char *str)
{
	printf(str); //发送命令给物联网设备
	cmd_axis++; //命令行加一
	x_axis = 0; //清零
	y_axis = 0;
}

/*
 * 物联网设备初始化
 *
 * 工作流程:
 * 	首先,IOT设备复位,并延时12秒钟等待设备复位成功,并打开串口
 * 	其次,按标准流程发布指令,并检测返回信息,若异常则整个系统(包含单片机与物联网设备)复位重启
 * 	最后,发送字符'~'判断是否联网成功,是则退出,否则系统复位.
 */
void IOT_init()
{
	IOT_Reset(); //物联网设备复位
	delay_us(12000000);
	usart_1_init(115200);

	send_cmd("ATE0 \r\n"); //第1条指令,对应cmd_axis为1
	delay_us(200000); //延时0.2s
	while (!check_receives(1, "OK"))
		//检测指令1返回的数据中是否包含"ok"
		;

	send_cmd(
	        "AT+QMTCFG=\"aliauth\",0,\"a1f2CH9BSx7\",\"ZRH_4G\",\"TEnbrWdkBXfLkca73A9Nhyzqe9o19HM6\" \r\n"); //第2条指令
	delay_us(500000); //延时0.5s

	send_cmd("AT+QMTOPEN=0,\"iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883 \r\n"); //第3条指令
	delay_us(1000000); //延时1s

	if (check_receives(3, "ERROR"))
	{
		NVIC_SystemReset();
	}
	delay_us(500000);
	if (check_receives(3, "+QMTOPEN: 0,-1"))
	{
		NVIC_SystemReset();
	}
	if (check_receives(3, "+QMTOPEN: 0,3"))
	{
		NVIC_SystemReset();
	}
	if (check_receives(3, "+QMTOPEN: 0,2"))
	{
		NVIC_SystemReset();
	}

	send_cmd("AT+QMTCONN=0,\"ZRH_4G\" \r\n"); //第4条指令
	delay_us(10000000);						  //1s
	if (check_receives(4, "ERROR"))
	{
		NVIC_SystemReset();
	}
	if (check_receives(4, "+QMTCONN: 0,1"))
	{
		NVIC_SystemReset();
	}
	cmd_axis = 6; //使用12号数组检测是否发送成功。
	printf("AT+QMTPUB=0,0,0,1,\"/a1f2CH9BSx7/ZRH_4G/user/put\" \r\n");
	delay_us(100000); //0.1s
	x_axis = 0;
	y_axis = 0;
	printf("~ \r\n");

	delay_us(200000);
	while (!check_receives(6, "+QMTPUB: 0,0,0"))
	{
		if (check_receives(6, "ERROR"))
		{
			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < 100; j++)
					receives[12][i][j] = '\0';
			}
			NVIC_SystemReset();
			break;
		}
	}
	delay_us(1000000); //1s
}

/*
 * 串口1接收中断
 */
void USART1_IRQHandler(void)
{
	//注意！不能使用if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)来判断
	//异常捕获，防止串口陷入死循环
	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
	{
		USART_ReceiveData(USART1);
	}
	char temp;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)	//接收到数据
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE); //清除接收中断标志
		temp = USART_ReceiveData(USART1); //接收串口1数据到buff缓冲区
		receives[cmd_axis][y_axis][x_axis] = temp;
		x_axis++;
		if (x_axis >= 97) //防止数据长度溢出,造成精确的数据访问冲突
		{
			x_axis = 0;
			receives[cmd_axis][y_axis][98] = 1;
		}
		if (y_axis > 9) //防止数据条数溢出,造成精确的数据访问冲突
			y_axis = 0;
		if (temp == '\n')
		{
			receives[cmd_axis][y_axis][99] = x_axis;
			y_axis++;
			x_axis = 0;
		}
	}
}

bool iscontants(char *str, char *cmd)
{
	int i = 0, start = 0, count = 0;
	uint8_t lens = strlen(cmd);
	for (i = 0; i < strlen(str); i++)
	{
		if (*(str + i) == *cmd)
		{
			if (*(str + i + 1) == *(cmd + 1))
			{
				start = i;
				break;
			}
		}
	}

	for (i = 0; i < lens; i++)
	{
		if (*(str + start + i) == *(cmd + i))
			count++;
		else
			break;
	}

	if (count == lens)
		return true;
	else
		return false;
}

bool check_receives(uint8_t cmd_number, char *cmd)
{
	int i = 0, j = 0;
	char str[20] = { '\0' };
	for (i = 0; i < y_axis; i++)
	{
		for (j = 0; j < receives[cmd_number][i][99] - 2; j++)
		{
			str[j] = receives[cmd_number][i][j];
		}
		if (str[0] == '\0' || str[0] == '\r' || str[0] == '\n')
		{
			continue;
		} else if (iscontants(str, cmd))
			return true;
	}
	return false;
}

void SendToCloud()
{
	printf("AT+QMTPUB=0,0,0,1,\"/a1f2CH9BSx7/ZRH_4G/user/put\"\r\n");
	delay_us(100000); //0.1s
	x_axis = 0;
	y_axis = 0;
	//	printf(
	//		"{\"Mark\":\"A1001\",\"Time\":\"16:04:09\",\"N\":\"230.125\",\"E\":\"1920.658\",\"Bohelun\":\"%d\",\"Zuoye\":\"%d\",\"Fukuan\":\"%d\",\"Getai\":\"%d\",\"Shusongzhou\":\"%d\",\"Chesu\":\"%d\",\"QieLTL\":\"%d\",\"ZongZTL\":\"%d\",\"FongJZS\":\"%d\",\"QuDL\":\"%d\",\"ZhengDS\":\"%d\",\"LiZSP\":\"%d\",\"ZaYSP\":\"%d\",\"GeCGD\":\"%d\",\"QinXSS\":\"%d\",\"JiaDSS\":\"%d\",\"YuLSD\":\"%d\",\"HanZL\":\"%d\",\"PoSL\":\"%d\",\"LiZLL\":\"%d\"} \r\n",
	//		Pack1.Mail_Box[1].whell_speed, Pack1.Mail_Box[1].is_on_work,
	//		Pack1.Mail_Box[1].ultrasonic_sensor, Pack1.Mail_Box[1].rotating_speed,
	//		Pack1.Mail_Box[1].drive_speed, Pack1.Mail_Box[1].car_speed,
	//		Pack2.Mail_Box[1].roller_speed, Pack2.Mail_Box[1].Yroller_speed,
	//		Pack2.Mail_Box[1].wind_speed, Pack2.Mail_Box[1].driver_speed,
	//		Pack3.Mail_Box[1].shock_speed, Pack3.Mail_Box[1].Xroller_speed,
	//		Pack3.Mail_Box[1].Xrest_speed, Pack4.Mail_Box[1].high,
	//		Pack5.Mail_Box[1].cleanlost_sensor, Pack5.Mail_Box[1].cliplost_sensor,
	//		Pack5.Mail_Box[1].angle, Pack6.Mail_Box[1].pure_value, Pack6.Mail_Box[1].break_value,
	//		Pack7.Mail_Box[1].float_value);
}

void pack_to_aliyun()
{
	printf("AT+QMTPUB=0,0,0,1,\"/a1f2CH9BSx7/ZRH_4G/user/put\" \r\n");
	delay_us(100000); //0.1s
//	x_axis = 0;
//	y_axis = 0;
//	printf(
//		"{\"Mark\":\"A1001\",\"Time\":\"16:04:09\",\"N\":\"230.125\",\"E\":\"1920.658\",\"Bohelun\":\"%d\",\"Zuoye\":\"%d\",\"Fukuan\":\"%d\",\"Getai\":\"%d\",\"Shusongzhou\":\"%d\",\"Chesu\":\"%d\",\"QieLTL\":\"%d\",\"ZongZTL\":\"%d\",\"FongJZS\":\"%d\",\"QuDL\":\"%d\",\"ZhengDS\":\"%d\",\"LiZSP\":\"%d\",\"ZaYSP\":\"%d\",\"GeCGD\":\"%d\",\"QinXSS\":\"%d\",\"JiaDSS\":\"%d\",\"YuLSD\":\"%d\",\"HanZL\":\"%d\",\"PoSL\":\"%d\",\"LiZLL\":\"%d\"} \r\n",
//		Pack1.Mail_Box[1].whell_speed, Pack1.Mail_Box[1].is_on_work,
//		Pack1.Mail_Box[1].ultrasonic_sensor, Pack1.Mail_Box[1].rotating_speed,
//		Pack1.Mail_Box[1].drive_speed, Pack1.Mail_Box[1].car_speed,
//		Pack2.Mail_Box[1].roller_speed, Pack2.Mail_Box[1].Yroller_speed,
//		Pack2.Mail_Box[1].wind_speed, Pack2.Mail_Box[1].driver_speed,
//		Pack3.Mail_Box[1].shock_speed, Pack3.Mail_Box[1].Xroller_speed,
//		Pack3.Mail_Box[1].Xrest_speed, Pack4.Mail_Box[1].high,
//		Pack5.Mail_Box[1].cleanlost_sensor, Pack5.Mail_Box[1].cliplost_sensor,
//		Pack5.Mail_Box[1].angle, Pack6.Mail_Box[1].pure_value, Pack6.Mail_Box[1].break_value,
//		Pack7.Mail_Box[1].float_value);
//
//	delay_us(500000); //0.5s
//	while (!check_receives(12, "+QMTPUB: 0,0,0"))
//	{
//		if (check_receives(12, "ERROR")) //检测指令2返回的数据中是否包含"ok"
//		{
//			for (int i = 0; i < 10; i++)
//			{
//				for (int j = 0; j < 99; j++)
//					receives[12][i][j] = '\0';
//			}
//			BC28_Init();
//			break;
//		}
//	}
}
