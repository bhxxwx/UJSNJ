/*
 * UserConfig.h
 *
 *  Created on: 2020��4��12��
 *      Author: WangXiang
 */

#ifndef USERCONFIG_H_
#define USERCONFIG_H_
#include <stddef.h>
#include "stm32f10x.h"
#include <string.h>
#include <misc.h>
#include <stdbool.h>
#include <math.h>
#include "Gen/GenLib.h"
#include "oled/oled.h"
#include "keys/keys.h"
#include "Servers.h"
#include "SPI_FLASH/SPI_FLASH.h"
#include "stm32f10x_spi.h"

/*---------------------------OLED-IIC����--------------------------*/
//���ѡ����B�˿ڼ��޸�Ϊ IIC_SDA_Poart_B����
#define IIC_SDA_Poart_A

//���ѡ��������������8�����ż� GPIO_Pin_8����
#define IIC_SDA_Pin GPIO_Pin_7

#define IIC_SCL_Poart_A
#define IIC_SCL_Pin GPIO_Pin_6

/*------------------------���ڻ�������С����------------------------*/
#define usart_buffer_size 500

/*-----------------------------PWM����-----------------------------*/
//#define PWM_Complementary_B//����PB��PWM���ڻ������,ȡ��ע�ͺ�PB6��PB8����,PB7��PB9����
//#define PWM_Complementary_C//����PC��PWM���ڻ������,ȡ��ע�ͺ�PC6��PC8����,PC7��PC9����

/*
 * PWMĬ��Ƶ������
 * PWMƵ�ʼ��㹫ʽ:72000/(899+1)=80kHz
 */
#define PWM_Freq_B 7199//10KHz
//#define PWM_Freq_B 1799//40kHz
//#define PWM_Freq_B 899//80kHz
//#define PWM_Freq_B 719//100kHz

#define PWM_Freq_C 719//100KHz


/*------------------------CAN��������------------------------------*/
#define USE_CAN_PA	//ʹ��PA��CAN����<����ӳ��ܽ�>
//#define USE_CAN_PB	//ʹ��PB��CAN����<��ӳ��ܽ�>


/*------------------------���뿪����������-------------------------*/
//��ʹ�û���Ҫ��keys/keys.c�����д�жϷ�����!!
#define E1_1_Poart_B
#define E1_1 GPIO_Pin_1
#define E1_1_IT GPIO_PinSource1

#define E1_2_Poart_B
#define E1_2 GPIO_Pin_0
#define E1_2_IT GPIO_PinSource0

#define E1_3_Poart_C
#define E1_3 GPIO_Pin_5
#define E1_3_IT GPIO_PinSource5

#define E2_1_Poart_B
#define E2_1 GPIO_Pin_11
#define E2_1_IT GPIO_PinSource11

#define E2_2_Poart_B
#define E2_2 GPIO_Pin_10
#define E2_2_IT GPIO_PinSource10

#define E2_3_Poart_B
#define E2_3 GPIO_Pin_2
#define E2_3_IT GPIO_PinSource2

#endif /* USERCONFIG_H_ */
