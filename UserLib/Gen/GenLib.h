/*
 * GenLib.h
 *
 *  Created on: 2019��11��10��
 *      Author: WangXiang
 *  LastWrite:2020/4/19
 *  Version:V2.0
 */

#ifndef GENLIB_H_
#define GENLIB_H_

#include "UserConfig.h"

/***************************************************�궨������**********************************************************/
//Flash ҳ��궨��
#define page250 0x0807D000
#define page251 0x0807D800
#define page252 0x0807E000
#define page253 0x0807E800
#define page254 0x0807F000
#define page255 0x0807F800

//��ʱ������/�ر�
#define systick_on SysTick->CTRL = 0X01
#define systick_off SysTick->CTRL = 0x00

//GPIOģʽ�궨��
#define OUTPUT GPIO_Mode_Out_PP
#define INPUT GPIO_Mode_IN_FLOATING
#define AIN GPIO_Mode_AIN
#define IPU GPIO_Mode_IPU
#define IPD GPIO_Mode_IPD
#define PWM GPIO_Mode_AF_PP
#define AOT GPIO_Mode_AIN
#define HIGH 0x1
#define LOW 0x0

//RTC��ʱ��/��
#define RTC_IT_ON 	RTC_WaitForSynchro(),RTC_ITConfig(RTC_IT_SEC, ENABLE),RTC_WaitForLastTask()//����RTCÿ���ж�
#define RTC_IT_OFF 	RTC_WaitForSynchro(),RTC_ITConfig(RTC_IT_SEC, DISABLE),RTC_WaitForLastTask()//�ر�RTCÿ���ж�

/***************************************************��������************************************************************/

//�жϳ�ʼ��->��4->ֻ����ռ���ȼ�->0~15
void nvic_init();

/***************************************************���� ��ز���********************************************************/
//���ڳ�ʼ��
void usart_1_init(int bandrate);
void usart_2_init(int bandrate, char nvic_pree);
void usart_3_init(int bandrate, char nvic_pree);

//���ڻ��������
int check_usart1_cache();
int check_usart2_cache();

//��ȡ���ڻ�����ȫ������,�����㻺����
char *usart1_readToEnd();
char *usart2_readToEnd();

//���ڷ�������,����1ʹ��printf������һ�㲻��usart_1_send()
void usart_1_send(uint8_t *data, int length);
void usart_2_send(uint8_t *data, int length);
void usart_3_send(uint8_t *data, int length);

/***************************************************ʱ��Ƶ�� ��ز���****************************************************/
//����CPUʱ��Ϊ72MHz,����ϵͳ��ʱ��Ƶ����ֵ
RCC_ClocksTypeDef set_cpu_72MHz();

/***************************************************��ʱ ��ز���********************************************************/
void delay_us(__IO u32 time); //�����ʱ
void delay_tick_us(u32 time); //Ӳ����ʱ,ʹ�ò���ϵͳʱ����ʹ��

/***************************************************��ʱ ��ز���********************************************************/
void RTC_1s_it_init(); //RTC��ʱ1s��ʼ��
void RTC_Handler(void (*temp_function)); //RTC�жϺ������ݺ���

//ϵͳ��ʱ��SystemTick,ʹ�ò���ϵͳʱ����ʹ��
void systick_delay(u32 time, void (*temp_function)); //ϵͳ��ʱ��SystemTick��ʱ�ж�

void time_counter_init(); //��ʱ����ʼ��
int time_counter_value(); //ϵͳ��ʱ�����ؼ�ʱֵ

void TIM7_init(uint16_t times);

/***************************************************GPIO ��ز���********************************************************/
//GPIOģʽ�趨
void pinModeA(uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode);
void pinModeB(uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode);
void pinModeC(uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode);
void pinModeD(uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode);

//GPIO���Ÿߵ͵�ƽ�趨
#define digitalWriteA(GPIO_Pin, GPIO_Mode) GPIO_WriteBit(GPIOA, GPIO_Pin, GPIO_Mode)
#define digitalWriteB(GPIO_Pin, GPIO_Mode) GPIO_WriteBit(GPIOB, GPIO_Pin, GPIO_Mode)
#define digitalWriteC(GPIO_Pin, GPIO_Mode) GPIO_WriteBit(GPIOC, GPIO_Pin, GPIO_Mode)
#define digitalWriteD(GPIO_Pin, GPIO_Mode) GPIO_WriteBit(GPIOD, GPIO_Pin, GPIO_Mode)

//��ȡGPIO���ŵ�ƽ
#define digitalReadA(GPIO_Pin) GPIO_ReadInputDataBit(GPIOA, GPIO_Pin)
#define digitalReadB(GPIO_Pin) GPIO_ReadInputDataBit(GPIOB, GPIO_Pin)
#define digitalReadC(GPIO_Pin) GPIO_ReadInputDataBit(GPIOC, GPIO_Pin)
#define digitalReadD(GPIO_Pin) GPIO_ReadInputDataBit(GPIOD, GPIO_Pin)

//��ȡADC
uint16_t analogeReadA(uint16_t GPIO_Pin);
uint16_t analogeReadC(uint16_t GPIO_Pin);

//дDAC
void analogWrite(uint16_t GPIO_Pin, uint16_t value);

//����PWMռ�ձ�
void DutyCycleB(uint16_t GPIO_Pin, uint8_t Duty);
void DutyCycleC(uint16_t GPIO_Pin, uint8_t Duty);

void SetPWMfreq(GPIO_TypeDef* GPIOx, int freq);

void adc_init();

/***************************************************CAN������ز���*******************************************************/
void CAN_INIT();

void CAN_POLLING_INIT();
void CAN_POLLING_SEND(CanTxMsg TxMessage);
CanRxMsg CAN_POLLING_REC();

void CAN_IT_INIT();
void CAN_IT_SEND(CanTxMsg TxMessage);
//CanRxMsg CAN_IT_REC();

/***************************************************Flash ��ز���********************************************************/
/*
 * FLASHʹ��ע������
 *	FLASHֻ����16λд�뷽ʽ����һ��д��ַΪ0x0807F800������һ��Ϊ0x0807F802��
 *	FLASH�У���ַÿ��һ����1���ֽ�8λ��
 *	FLASH�У���ν��д�������ǰ�λ�룬
 */
/*
 * ��:
 *   addr = FlashWrite_8bitbuffer(page254, (uint8_t*) "hello", 5); //��page254д��5��8λ����"hello",���õ����һ�����ݵ�flash��ַ
 *
 *   //FLASHֻ����16λд�뷽ʽ������´�д����Ҫ��addr+2
 *   FlashWrite_16bitbuffer(addr + 2, &temp, 1); //��addr��һ����ַд��0x1234��16λ����
 *
 *   addr = FlashRead_8bitbuffer(page254, clean, 3); //��������3��8λ����,�������һ���������ݵĵ�ַ(uint8_t clean[6] = { 0x00 };)
 */
void Flash_Erase(uint32_t page);
uint32_t FlashWrite_8bitbuffer(uint32_t page, uint8_t *buffer, uint32_t length);
uint32_t FlashWrite_16bitbuffer(uint32_t page, uint16_t *buffer, uint32_t length);
uint32_t FlashRead_8bitbuffer(uint32_t address, uint8_t *buffer, uint32_t length);
uint16_t FlashRead_16bitbuffer(uint32_t address);

#endif /* GENLIB_H_ */
