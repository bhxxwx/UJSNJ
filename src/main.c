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

/*
 * TODO BC28orEC20函数中接收缓冲区过于庞大,需要精简;
 */

char read[256];
int main(void)
{
	DBGMCU_Config(DBGMCU_IWDG_STOP, ENABLE); //DEBUG时看门狗关闭
	nvic_init(); //中断初始化


	SPI_INIT();//SPI协议初始化
	SPI_FlashReset();//SPI-Flash复位
	SPI_EraseChip();//SPI-Flash全片擦除
	SPI_FlashFindHeadPage();//查找上次断电前保存的最后写入位置
	int number = 0;
	for (number = 0; number < 65500; number++)
		SPI_printf("hello,this is the %d message at page %d sector %d", number + 1,17 + number, (16 + number) / 16 + 1);

	SPI_FlashLostPower();//如果flash断电,调用该函数保存最后一次写入位置


	int count = 0;
	while (1)
	{
		count++;
	}
}

