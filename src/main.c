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


int main(void)
{
	DBGMCU_Config(DBGMCU_IWDG_STOP, ENABLE); //DEBUG时看门狗关闭
	nvic_init();
	int i = 0;
	uint16_t data[20] = { 0 };

//    USB_Port_Set(0);
//    delay_us(100000);
//    USB_Port_Set(1);
//    USB_Config();

	SPI_INIT();
	SPI_I2S_ReceiveData(SPI1);
//	SPI_DataSizeConfig(SPI1, SPI_DataSize_8b);

	WriteEN();
//	CheckBusy();
	delay_us(1000);
	CS_LOW;
	SPI_write(FLASH_ERASE_PAGE);
	SPI_write(0x00);
	SPI_write(0x00);
	SPI_write(0x00);

	CS_HIGH;
//	CheckBusy();
	delay_us(22000);
	CS_LOW;
	SPI_write(FLASH_READ_SR_CMD);


//	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
//	{
//	} //等待接收


	SPI_I2S_SendData(SPI1,FLASH_READ_SR_CMD); //发送数据
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
		{
		} //等待接收
	SPI_I2S_SendData(SPI1, 0xFF); //发送数据
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
		{
		} //等待接收

	data[i] = SPI_I2S_ReceiveData(SPI1);


	CS_HIGH;




	WriteEN();
//	CheckBusy();
	delay_us(1000);
	CS_LOW;
	SPI_write(FLASH_WRITE_PAGE);
	SPI_write(0x00);
	SPI_write(0x00);
	SPI_write(0x00);

	SPI_write(0x01);
	SPI_write(0x02);
	SPI_write(0x03);
	CS_HIGH;

//	CheckBusy();


	delay_us(1000);
	CS_LOW;

	SPI_write(FLASH_FASTREAD_DATA);
	SPI_write(0x00);
	SPI_write(0x00);
	SPI_write(0x00);
	for (i = 0; i < 9; i++)
	{
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
		{
		} //等待发送区为空
		SPI_I2S_SendData(SPI1, 0x00); //发送数据

		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
		{
		} //等待接收
		data[i] = SPI_I2S_ReceiveData(SPI1);

	}
	delay_us(1000);
	CS_HIGH;

	int count = 0;
	while (1)
	{
		count++;
	}
}

