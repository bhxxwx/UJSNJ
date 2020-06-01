/*
 * oled.h
 *
 *  Created on: 2020��4��11��
 *      Author: WangXiang
 *  LastWrite:2020/4/19
 *  Version:V2.0
 */

#ifndef OLED_OLED_H_
#define OLED_OLED_H_
#include "UserConfig.h"
#include <stdarg.h>
//#include "Gen/GenLib.h"

#define XLevelL           0x00//��λ��ַ
#define XLevelH           0x10//��λ��ַ
#define XLevel            ((XLevelH&0x0F)*16+XLevelL)
#define Max_Column        128//�����Ļ���ؿ��
#define Max_Row           64 //�����Ļ���ؿ��
#define Brightness        0x78 //IIC��ʼ��ַ
#define X_WIDTH           128//��Ļ���ؿ��
#define Y_WIDTH           64 //��Ļ���ظ߶�

void IIC_delayus(unsigned int i);//IICͨѶ��ʱ(һ���ò���)

void IIC_start(void); //��ʼIICͨ��
void IIC_stop(void); //����IICͨ��
void IIC_writebyte(unsigned char IIC_byte); //д�ֽ�
void IIC_writecmd(unsigned char IIC_command); //д����
void IIC_writedata(unsigned char IIC_data); //д����


void oled_Set_Pos(unsigned char x, unsigned char y); //������ʾλ��
void oled_CLS(void); //����
void oled_Init(void); //OLED����ʼ��
void oled_ShowChar(unsigned char x, unsigned char y, unsigned char chr); //��ʾ�ַ�
void oled_ShowString(unsigned char x, unsigned char y, unsigned char *chr); //��ʾ�ַ���
void oled_ShowNum(unsigned char x, unsigned char y, unsigned long num, unsigned char len, unsigned char size2); //��ʾ����
unsigned long oled_pow(unsigned char m, unsigned char n); //�ַ����ݺ���




void ts_itoa(char **buf, unsigned int d, int base);
int ts_formatstring(char *buf, const char *fmt, va_list va);
int ts_formatlength(const char *fmt, va_list va);

#endif /* OLED_OLED_H_ */
