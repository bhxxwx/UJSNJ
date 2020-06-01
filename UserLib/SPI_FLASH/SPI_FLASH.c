/*
 * SPI_FLASH.c
 *
 *  Created on: 2020��5��22��
 *      Author: WangXiang
 */
#include "SPI_FLASH.h"
#include "UserConfig.h"
#include <stdarg.h>
__attribute__ ((alias("SPI_iprintf"))) int SPI_printf(uint32_t page,const char *fmt, ...);

void SPI_INIT()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1, ENABLE );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


 	GPIO_ResetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//ѡ���˴���ʱ�ӵ���̬:ʱ�����ո�
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//���ݲ����ڵڶ���ʱ����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ8-9M
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

//	SPI_NSSInternalSoftwareConfig(SPI1,SPI_NSSInternalSoft_Reset);
	pinModeA(GPIO_Pin_4,OUTPUT);
	SPI_SSOutputCmd(SPI1,ENABLE);
	SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����
	CS_HIGH;
}



void SPI_write(u8 TxData)
{
	u8 retry=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
		{
		retry++;
		if(retry>200)
			return;
		}
	SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ������



}


void SPI_writeStr(uint32_t page,char *str)
{
	SPI_write((page>>16)&(0xFF));
	SPI_write((page>>8)&(0xFF));
	SPI_write((page)&(0xFF));
	uint16_t j = 0;
	while (str[j] != '\0') //�����ַ�����ֱ���ַ���Ϊ��
	{
		if(j<256)
		{
			SPI_write(str[j]);
			j++;
		}
		else
			break;
	}

	if(j>=256)
	{
		SPI_write(((page+256)>>16)&(0xFF));
		SPI_write(((page+256)>>8)&(0xFF));
		SPI_write(((page+256))&(0xFF));
		while (str[j] != '\0') //�����ַ�����ֱ���ַ���Ϊ��
		{
				SPI_write(str[j]);
				j++;
		}
	}


}

void WREN()
{
	CS_LOW;
	SPI_write(0x60);
//	delay_us(1);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	CS_HIGH;
//	delay_us(1);
	CS_LOW;


}
void ERDI()
{
//	delay_us(1);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	CS_HIGH;
	CS_LOW;
	SPI_write(0x20);
//	delay_us(1);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	CS_HIGH;
}

int SPI_iprintf(uint32_t page,const char *fmt, ...)
{
	int length = 0;
	va_list va;
	va_start(va, fmt);
	length = ts_formatlength(fmt, va);
	va_end(va);
	{
		char buf[length];
		va_start(va, fmt);
		length = ts_formatstring(buf, fmt, va);
//		WREN();
		CS_LOW;
		SPI_write(0x02);


		SPI_writeStr(page,buf);
		CS_HIGH;
//		ERDI();

		va_end(va);
	}
	return length;
}

