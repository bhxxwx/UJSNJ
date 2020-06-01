/*
 * Genlib.c
 *
 *  Created on: 2019��11��10��
 *      Author: WangXiang
 */
#include "GenLib.h"
#include "UserConfig.h"

char temp[usart_buffer_size] = { '\0' }; //��������ݴ�����
uint16_t pwm_freq_B;
uint16_t pwm_freq_C;

uint8_t DB1, DB2, DB3, DB4, DC1, DC2, DC3, DC4;

typedef struct
{
	char RX_buffer[usart_buffer_size];
	uint16_t number;
	bool is_full;
} usart_cache; //���ڻ��������ݽṹ��

//��ʼ���������ݻ�����
usart_cache Uart1_RX_Buffer;
usart_cache Uart2_RX_Buffer;

//static int uart1_in_cache = 0;
//static int uart2_in_cache = 0;

//����Ƿ��Ѿ���ʼ������ع���
static bool adcinit_flag = false;
static bool pwminit_flag_B = false;
static bool pwminit_flag_C = false;

void (*function_handler)(void); //RTC��ʱ���ж���ת����ָ��
void (*systick_handler)(void); //�ж���ת����ָ��

//���ֳ�ʼ���ṹ��
GPIO_InitTypeDef GPIO_InitStruct;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;

/*
 * �ж����ȼ����Ƴ�ʼ��
 * ֻ����ռʽ���ȼ���û����Ӧ���ȼ�
 * ��ռ���ȼ�4λ,�ɽ���Ƕ��,��ѡֵΪ:(�����ȼ�)0~15(�����ȼ�)
 * ʹ�ô���2,3���жϵ�ʱ����Ҫ�ȳ�ʼ���ú���
 */
void nvic_init()
{
	//ֻ����ռʽ���ȼ���û����Ӧ���ȼ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}

/*
 * ����1��ʼ��
 * ���ȼ�Ϊ0
 */
void usart_1_init(int bandrate)
{
	Uart1_RX_Buffer.is_full = false;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	USART_DeInit(USART1);
	nvic_init();

	//USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//Usart1 NVIC
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //��ռ���ȼ�
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;//��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//USART
	USART_InitStructure.USART_BaudRate = bandrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//�򿪽��յ������ж�
	USART_Cmd(USART1, ENABLE);
}

/*
 * ����2���Ƴ�ʼ��
 * bandrate:����������
 * nvic_pree:��ռ���ȼ�4λ,�ɽ���Ƕ��,��ѡֵΪ:(�����ȼ�)0~15(�����ȼ�)
 */
void usart_2_init(int bandrate, char nvic_pree)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	USART_DeInit(USART2);

	//USART2_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART2_RX	  PA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//Usart2 NVIC
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = nvic_pree;	//��ռ���ȼ�
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;//��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//USART
	USART_InitStructure.USART_BaudRate = bandrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART2, &USART_InitStructure);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	//�򿪽��յ������ж�
	USART_Cmd(USART2, ENABLE);
}

/*
 * ����3���Ƴ�ʼ��
 * bandrate:����������
 * nvic_pree:��ռ���ȼ�4λ,�ɽ���Ƕ��,��ѡֵΪ:(�����ȼ�)0~15(�����ȼ�)
 */
void usart_3_init(int bandrate, char nvic_pree)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	USART_DeInit(USART3);

	//USART3_TX   PB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//USART3_RX	  PB.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//Usart3 NVIC
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = nvic_pree;	//��ռ���ȼ�
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;//��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//USART
	USART_InitStructure.USART_BaudRate = bandrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART3, &USART_InitStructure);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	//�򿪽��յ������ж�
	USART_Cmd(USART3, ENABLE);
}

/*
 * ��⴮��1���ݻ�����δ��ȡ���ݸ���
 * ����ֵ:	���������ݸ���(>=0)
 * 			-1->�������Լ�������������ݶ�ʧ
 */
int check_usart1_cache()
{
	if (Uart1_RX_Buffer.is_full == true)
		return -1;
	else
		return Uart1_RX_Buffer.number;
}

/*
 * ��⴮��2���ݻ�����δ��ȡ���ݸ���
 * ����ֵ:	���������ݸ���(>=0)
 * 			-1->�������Լ�������������ݶ�ʧ
 */
int check_usart2_cache()
{
	if (Uart2_RX_Buffer.is_full == true)
		return -1;
	else
		return Uart2_RX_Buffer.number;
}

/*
 * �Ӵ���1���ݻ�������ȡȫ������,�����㻺����
 */
char *usart1_readToEnd()
{
	int i = 0;
	for (i = 0; i < Uart1_RX_Buffer.number; i++)	//�����������ݸ��Ƶ��ݴ�����
	{
		temp[i] = Uart1_RX_Buffer.RX_buffer[i];
	}
	for (; i < usart_buffer_size; i++)	//���ݴ�����֮�����������
	{
		temp[i] = '\0';
	}
	Uart1_RX_Buffer.number = 0;	//��������λ��,��һ�����ݽ��շŵ�������0λ
	return temp;
}

/*
 * �Ӵ���2���ݻ�������ȡȫ������,�����㻺����
 */
char *usart2_readToEnd()
{
	int i = 0;
	for (i = 0; i < Uart2_RX_Buffer.number; i++)	//�����������ݸ��Ƶ��ݴ�����
	{
		temp[i] = Uart2_RX_Buffer.RX_buffer[i];
	}
	for (; i < usart_buffer_size; i++)	//���ݴ�����֮�����������
	{
		temp[i] = '\0';
	}
	Uart2_RX_Buffer.number = 0;	//��������λ��,��һ�����ݽ��շŵ�������0λ
	return temp;
}

//void USART1_IRQHandler(void)
//{
//	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)	//���յ�����
//	{
//		USART_ClearITPendingBit(USART1, USART_IT_RXNE); //��������жϱ�־
//
//		if (Uart1_RX_Buffer.number < usart_buffer_size) //�����ǰ����С�ڻ�������С
//		{
//			Uart1_RX_Buffer.RX_buffer[Uart1_RX_Buffer.number] = USART_ReceiveData(USART1); //���մ���1���ݵ�buff������
////			uart1_in_cache++;
//			Uart1_RX_Buffer.number++; //����λ��+1
//		} else
//		{
//			Uart1_RX_Buffer.is_full = true; //����������
//		}
//	}
//}

void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //���յ�����
	{
		USART_ClearITPendingBit(USART2, USART_IT_RXNE); //��������жϱ�־
		if (Uart2_RX_Buffer.number < usart_buffer_size)
		{
			Uart2_RX_Buffer.RX_buffer[Uart2_RX_Buffer.number] = USART_ReceiveData(USART2); //���մ���1���ݵ�buff������
//			uart2_in_cache++;
			Uart2_RX_Buffer.number++;
		} else
		{
			Uart2_RX_Buffer.is_full = true;
		}
	}
}

//void USART3_IRQHandler(void)
//{
//	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //���յ�����
//	{
//		USART_ClearITPendingBit(USART3, USART_IT_RXNE); //��������жϱ�־
//		USART_ReceiveData(USART3);
//	}
//}

/*
 * ����1��������
 * ����:  data->��Ҫ���͵����ݵ�char��ָ��
 * 		  length->���͵�����λ��
 */
void usart_1_send(uint8_t *data, int length)
{
	int i = 0;
	for (; i < length; i++)
	{
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
			//�ȴ���ǰ���ݷ������
			;
		USART_SendData(USART1, *(data + i));
	}
}

/*
 * ����2��������
 * ����:  data->��Ҫ���͵����ݵ�char��ָ��
 * 		  length->���͵�����λ��
 */
void usart_2_send(uint8_t *data, int length)
{
	int i = 0;
	for (; i < length; i++)
	{
		while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
			//�ȴ���ǰ���ݷ������
			;
		USART_SendData(USART2, *(data + i));
	}
}

/*
 * ����3��������
 * ����:  data->��Ҫ���͵����ݵ�char��ָ��
 * 		  length->���͵�����λ��
 */
void usart_3_send(uint8_t *data, int length)
{
	int i = 0;
	for (; i < length; i++)
	{
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
			//�ȴ���ǰ���ݷ������
			;
		USART_SendData(USART3, *(data + i));
	}
}

/*
 * ����CPUʱ��Ϊ72MHz
 * ����ϵͳ��ʱ��Ƶ����ֵ
 * һ���ò���
 */
RCC_ClocksTypeDef set_cpu_72MHz()
{
	RCC_ClocksTypeDef get_rcc_clock;
	//set the CPU clock as 72MHz
//	RCC_HSEConfig(RCC_HSE_OFF);
//	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
//	RCC_HSEConfig(RCC_HSE_ON);
//	RCC_PCLK1Config(RCC_HCLK_Div4); //APB1
//	RCC_WaitForHSEStartUp();
//	RCC_HCLKConfig(RCC_SYSCLK_Div1);
//	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	RCC_GetClocksFreq(&get_rcc_clock);
	return get_rcc_clock;
}

/* ��ʱ���ӳ�,���ж�,ʹ���ⲿʱ���ź�
 * (��ȷ)
 * �����ʱ1.8s����1864135us
 * ע�⣬ʹ�ò���ϵͳʱ����ʹ�øú���
 */
void delay_tick_us(u32 time)
{
	u32 temp;
	SysTick->LOAD = 9 * time;
	SysTick->VAL = 0X00;   //��ռ�����
	SysTick->CTRL = 0X01;   //ʹ�ܣ����������޶����������ⲿʱ��Դ
	do
	{
		temp = SysTick->CTRL;   //��ȡ��ǰ������ֵ
	} while ((temp & 0x01) && (!(temp & (1 << 16)))); //�ȴ�ʱ�䵽��
	SysTick->CTRL = 0x00; //�رռ�����
	SysTick->VAL = 0X00; //��ռ�����
}

/* RTC��ʱ����ʼ��
 * �趨���ȼ�Ϊ1
 * �趨ÿһ����һ���ж�
 * Ĭ���жϿ���
 *
 * ʹ�� RTC_IT_ON; ����RTC�ж�
 * ʹ�� RTC_IT_OFF; �ر�RTC�ж�
 */
void RTC_1s_it_init()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //��ռ���ȼ�
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;//��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	PWR_BackupAccessCmd(ENABLE);
	RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128);	//ѡ��LSEʱ����ΪRTCʱ�ӣ����⻹����ѡ��LSI��HSE_Div128
	RCC_RTCCLKCmd(ENABLE);	//ʹ��RTCʱ��
	RTC_WaitForSynchro();
	RTC_WaitForLastTask();

	RTC_EnterConfigMode();	/// ��������
	RTC_SetPrescaler(62500);
	RTC_WaitForLastTask();
	RTC_ExitConfigMode(); //�˳�����ģʽ

	RTC_ClearITPendingBit(RTC_IT_SEC);
	RTC_WaitForSynchro();
	RTC_ITConfig(RTC_IT_SEC, ENABLE); //���ж�ʹ����,��������һ���ɱ�̵��������ж��ź�(��ɴ�1��)��
	RTC_WaitForLastTask();

}

/* RTC�жϺ������ݺ���
 * ����:�û��Զ��庯���ĺ�����(��ַ)
 * ����:RTC_Handler(test_hander)
 */
void RTC_Handler(void (*temp_function))
{
	//RTC_ITConfig(RTC_IT_SEC, ENABLE); //���ж�ʹ����,��������һ���ɱ�̵��������ж��ź�(��ɴ�1��)��
	function_handler = temp_function;
}

void RTC_IRQHandler(void) //RTC�жϺ���
{
	if (function_handler == 0x0)
	{
		RTC_ClearITPendingBit(RTC_IT_SEC); //����жϱ��
		return;
	}
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET) //���ж�
	{
		function_handler();
		RTC_ClearITPendingBit(RTC_IT_SEC); //����жϱ��
	}
}

/* �����ʱ,ϵͳʱ��Ϊ72MHzʱʹ��,���򲻾�ȷ
 * ���1us
 * (��ȷ)
 */
void delay_us(__IO u32 time)
{
	int i = 0;
	unsigned char temp = 4;
	for (i = time; i >= 0; i--)
	{
		while (temp)
		{
			//__NOP();//������ָ�� 1/72M=0.01389us
			temp--;
		}
		temp = 4;
	}
}

/*
 * ϵͳ��ʱ���жϣ�����ж���ʱ233ms
 * ����:
 * 		u32 time:��СֵΪ43,
 * 			���ֵΪ72000000	,
 * 			����ȡֵ��:
 * 				1000 -> 1ms�ж�һ��
 * 				100000 -> 10us�ж�һ��
 * 				1000000 -> 1us�ж�һ��
 * 		(*temp_function):�жϷ�������ַ
 *
 * ʾ��:systick_delay(1000,TEST_FUN);
 *
 * ע��!�������,�뽫src/stm32f1xx_it.c�е�SysTick_Handler(void)����ע�͵�
 */
void systick_delay(u32 time, void (*temp_function))
{
	u32 temp = SystemCoreClock / 10;
	temp = SystemCoreClock / time;
	if (temp > 0xFFFFFF)
		temp = 0xFFFFFF;
	if (SysTick_Config(temp))
	{
		/* Capture error */
		while (1)
			;
	}
	systick_handler = temp_function;
//	SysTick->CTRL = 0x00;   //ʧ��SysTick
//	SysTick->VAL = 0x00;    //��ǰֵ����
}

//#ifndef FREERTOS_CONFIG_H
//
//void SysTick_Handler(void)
//{
//	systick_handler();
//}
//#endif
/*
 * flash��ʼ��
 * ����һҳ��22ms,�ҹ�����CPU�������κ�����
 * page:��ʼ��ҳ��ַ
 */
void Flash_Erase(uint32_t page)
{
	FLASH_SetLatency(FLASH_Latency_2); //FLASHʱ���ӳټ������ڣ��ȴ�����ͬ������
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); //����FLASHԤ�����幦�ܣ�����FLASH�Ķ�ȡ
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(page);
	FLASH_Lock();
}

/*
 * ��16λд�뷽ʽ8λbuffer
 * page:д��ҳ��ַ
 * buffer:��Ҫд���8λ����ָ��
 * length:��Ҫд�����ݵĳ���
 * �ú����������һ��д�����ݵ�flash��ַ�����߷��صڼ�λ����д��ʧ��
 */
uint32_t FlashWrite_8bitbuffer(uint32_t page, uint8_t *buffer, uint32_t length)
{
	uint16_t i, data = 0;
	uint32_t lastaddr = 0;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	for (i = 0; i < length; i += 2)
	{
		data = (*(buffer + i + 1) << 8) + (*(buffer + i));
		if (FLASH_ProgramHalfWord((uint32_t) (page + i), data) != FLASH_COMPLETE)
		{
			return (uint32_t) i;
		}
		lastaddr = (uint32_t) (page + i);
	}
	FLASH_Lock();
	return (uint32_t) lastaddr;
}

/*
 * ��16λд�뷽ʽ16λbuffer
 * page:д��ҳ��ַ
 * buffer:��Ҫд���16λ����ָ��
 * length:��Ҫд�����ݵĳ���
 * �ú����������һ��д�����ݵ�flash��ַ�����߷��صڼ�λ����д��ʧ��
 */
uint32_t FlashWrite_16bitbuffer(uint32_t page, uint16_t *buffer, uint32_t length)
{
	uint16_t i, data = 0;
	uint32_t lastaddr = 0;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	for (i = 0; i < length; i += 2)
	{
		data = *(buffer + i);
		if (FLASH_ProgramHalfWord((uint32_t) (page + i), data) != FLASH_COMPLETE)
		{
			return i;
		}
		lastaddr = (uint32_t) (page + i);
	}
	FLASH_Lock();
	return lastaddr;
}

/*
 * ����8λ��������
 * address:��ȡ�����׵�ַ
 * *buffer:�������ݴ�ŵ�Ŀ���ַ
 * length:��Ҫ��ȡ���ݸ���
 * ���ص�ǰ���ݵ�ַ
 */
uint32_t FlashRead_8bitbuffer(uint32_t address, uint8_t *buffer, uint32_t length)
{
	memcpy(buffer, (void *) address, length);
	return address + length - 1;
}

/*
 *	����ָ����ַ16λ����
 *	�͵�ַ�ڵ�λ
 */
uint16_t FlashRead_16bitbuffer(uint32_t address)
{
	return *(__IO uint16_t*) address;
}

/*
 * ��ʱ����ʼ��,����ʱ1.8s,���û���κ�����,ע��ʹ�ó���,����ϵͳ�в���ʹ��
 * systick_on;//��ʼ��ʱ
 * systick_off;//ֹͣ��ʱ
 */
void time_counter_init()
{
	SysTick->LOAD = 0xFFFFFF;
	SysTick->VAL = 0X00;   //��ռ�����
	SysTick->CTRL = 0x00; //�رռ�����
}

/*
 * ���ؼ�ʱֵ����λus
 */
int time_counter_value()
{
	int finishtime = 0, time = 0;
	time = (SysTick->VAL);
	finishtime = (0xFFFFFF - time) / 9;
	return finishtime;
}

void adc_init()
{
	ADC_InitTypeDef ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //ʹ��ADC1ͨ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE); //ʹ��ADC2ͨ��ʱ��
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6,72M/6=12MHz,ADC����ܳ���14M
	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ
	ADC_DeInit(ADC2);  //��λADC2,������ ADC2 ��ȫ���Ĵ�������Ϊȱʡֵ
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���
	ADC_Init(ADC2, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���

	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼
	while (ADC_GetResetCalibrationStatus(ADC1))
		;	//�ȴ���λУ׼����
	ADC_StartCalibration(ADC1);	 //����ADУ׼
	while (ADC_GetCalibrationStatus(ADC1))
		;	 //�ȴ�У׼����

	ADC_Cmd(ADC2, ENABLE);	//ʹ��ָ����ADC2
	ADC_ResetCalibration(ADC2);	//ʹ�ܸ�λУ׼
	while (ADC_GetResetCalibrationStatus(ADC2))
		;	//�ȴ���λУ׼����
	ADC_StartCalibration(ADC2);	 //����ADУ׼
	while (ADC_GetCalibrationStatus(ADC2))
		;	 //�ȴ�У׼����
	adcinit_flag = true;
}

/*
 * GPIOA����ģʽ����
 * GPIO_Pin_:GPIO_Pin_X
 * GPIO_Mode: OUTPUT �������
 *			  INPUT ��������
 *			  AIN ģ������(ADC)
 *			  IPU ��������
 *			  IPD ��������
 *			  AOT ģ�����(DAC)
 */
void pinModeA(uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	if (GPIO_Mode == GPIO_Mode_AIN)
	{
		if (adcinit_flag == false)
		{
			adc_init();
			/***************************************DAC******************************************************/
			DAC_InitTypeDef DAC_InitType;
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);	  //ʹ��DACͨ��ʱ��
			DAC_InitType.DAC_Trigger = DAC_Trigger_None;	//��ʹ�ô������� TEN1=0
			DAC_InitType.DAC_WaveGeneration = DAC_WaveGeneration_None;	//��ʹ�ò��η���
			DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;	//���Ρ���ֵ����
			DAC_InitType.DAC_OutputBuffer = DAC_OutputBuffer_Disable;	//DAC1�������ر� BOFF1=1
			DAC_Init(DAC_Channel_1, &DAC_InitType);	 //��ʼ��DACͨ��1
			DAC_Init(DAC_Channel_2, &DAC_InitType);	 //��ʼ��DACͨ��2

			DAC_Cmd(DAC_Channel_1, ENABLE);  //ʹ��DACͨ��1
			DAC_Cmd(DAC_Channel_2, ENABLE);  //ʹ��DACͨ��2

			DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12λ�Ҷ������ݸ�ʽ����DACֵ
			DAC_SetChannel2Data(DAC_Align_12b_R, 0);  //12λ�Ҷ������ݸ�ʽ����DACֵ
		}
	}
}

/*
 * GPIOB����ģʽ����
 * GPIO_Pin_:GPIO_Pin_X
 * GPIO_Mode: OUTPUT �������
 *			  INPUT ��������
 *			  IPU ��������
 *			  IPD ��������
 *			  PWM PWM���,Ĭ��Ƶ��10kHz
 */
void pinModeB(uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	if (GPIO_Mode == GPIO_Mode_AF_PP)
	{
		if (pwminit_flag_B == false)
		{
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

			//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
			//PWMƵ��:72000/(899+1)=80khz
			TIM_TimeBaseStructure.TIM_Period = PWM_Freq_B;
			pwm_freq_B = PWM_Freq_B;

			TIM_TimeBaseStructure.TIM_Prescaler = 0; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
			TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
			TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
			TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

			TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
			TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
			TIM_OCInitStructure.TIM_Pulse = 0; //���ô�װ�벶��ȽϼĴ���������ֵ
			TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ը�
			TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
			TIM_OC2Init(TIM4, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx

#ifdef PWM_Complementary_B
			TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
#endif
			TIM_OC3Init(TIM4, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
			TIM_OC4Init(TIM4, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx

			TIM_CtrlPWMOutputs(TIM4, ENABLE);	//MOE �����ʹ��

			TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH1Ԥװ��ʹ��
			TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH2Ԥװ��ʹ��
			TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH3Ԥװ��ʹ��
			TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH4Ԥװ��ʹ��

			TIM_ARRPreloadConfig(TIM4, ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���

			TIM_Cmd(TIM4, ENABLE);  //ʹ��TIM1
			pwminit_flag_B = true;
		}
	}
}

/*
 * GPIOC����ģʽ����
 * GPIO_Pin_:GPIO_Pin_X
 * GPIO_Mode: OUTPUT �������
 *			  INPUT ��������
 *			  AIN ģ������
 *			  IPU ��������
 *			  IPD ��������
 *			  PWM PWM���,Ĭ��Ƶ��100kHz
 */
void pinModeC(uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	if (GPIO_Mode == GPIO_Mode_AIN)
	{
		if (adcinit_flag == false)
		{
			adc_init();
		}
	}
	if (GPIO_Mode == GPIO_Mode_AF_PP)
	{
		if (pwminit_flag_C == false)
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

			//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
			//PWMƵ��:72000/(899+1)=80khz
			TIM_TimeBaseStructure.TIM_Period = PWM_Freq_C;
			pwm_freq_C = PWM_Freq_C;

			TIM_TimeBaseStructure.TIM_Prescaler = 0; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
			TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
			TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
			TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

			TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
			TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
			TIM_OCInitStructure.TIM_Pulse = 0; //���ô�װ�벶��ȽϼĴ���������ֵ
			TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ը�
			TIM_OC1Init(TIM8, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
			TIM_OC2Init(TIM8, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
#ifdef PWM_Complementary_C
			        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
#endif
			TIM_OC3Init(TIM8, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
			TIM_OC4Init(TIM8, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx

			TIM_CtrlPWMOutputs(TIM8, ENABLE);	//MOE �����ʹ��

			TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);  //CH1Ԥװ��ʹ��
			TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);  //CH2Ԥװ��ʹ��
			TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);  //CH3Ԥװ��ʹ��
			TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);  //CH4Ԥװ��ʹ��

			TIM_ARRPreloadConfig(TIM8, ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���

			TIM_Cmd(TIM8, ENABLE);  //ʹ��TIM1
			pwminit_flag_C = true;
		}
	}
}

/*
 * GPIOD����ģʽ����
 * GPIO_Pin_:GPIO_Pin_X
 * GPIO_Mode: OUTPUT �������
 *			  INPUT  ��������
 *			  IPU    ��������
 *			  IPD    ��������
 */
void pinModeD(uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/*
 * ��ȡGPIOA�˿� Pin_0 ~ Pin_7��ģ������
 * ʹ��ǰ�뽫A�˿�ģʽ��Ϊģ������ģʽ����:
 * 		pinModeA(GPIO_Pin_7,AIN);
 */
uint16_t analogeReadA(uint16_t GPIO_Pin)
{
	uint8_t ch = 0;
	switch (GPIO_Pin)
	{
		case GPIO_Pin_0:
			ch = 0;
			break;
		case GPIO_Pin_1:
			ch = 1;
			break;
		case GPIO_Pin_2:
			ch = 2;
			break;
		case GPIO_Pin_3:
			ch = 3;
			break;
		case GPIO_Pin_4:
			ch = 4;
			break;
		case GPIO_Pin_5:
			ch = 5;
			break;
		case GPIO_Pin_6:
			ch = 6;
			break;
		case GPIO_Pin_7:
			ch = 7;
			break;
		default:
			ch = 17;
	}
	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	//ʹ��ָ����ADC1�����ת����������

	while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
		;		//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}

/*
 * ��ȡGPIOC�˿� Pin_0 ~ Pin_5��ģ������
 * ʹ��ǰ�뽫C�˿�ģʽ��Ϊģ������ģʽ����:
 * 		pinModeC(GPIO_Pin_7,AIN);
 */
uint16_t analogeReadC(uint16_t GPIO_Pin)
{
	uint8_t ch = 0;
	switch (GPIO_Pin)
	{
		case GPIO_Pin_0:
			ch = 10;
			break;
		case GPIO_Pin_1:
			ch = 11;
			break;
		case GPIO_Pin_2:
			ch = 12;
			break;
		case GPIO_Pin_3:
			ch = 13;
			break;
		case GPIO_Pin_4:
			ch = 14;
			break;
		case GPIO_Pin_5:
			ch = 15;
			break;
		default:
			ch = 17;
	}
	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC2, ch, 1, ADC_SampleTime_239Cycles5);	//ADC2,ADCͨ��,����ʱ��Ϊ239.5����

	ADC_SoftwareStartConvCmd(ADC2, ENABLE);	//ʹ��ָ����ADC2�����ת����������

	while (!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC))
		;		//�ȴ�ת������

	return ADC_GetConversionValue(ADC2);	//�������һ��ADC2�������ת�����
}

/*
 * DACģ�����
 * 		GPIO_Pin  ���ſ���,ֵΪGPIO_Pin_4��5
 * 		value	�����ѹ����(0~3.3),ֵΪ0~4095
 */
void analogWrite(uint16_t GPIO_Pin, uint16_t value)
{
	if (GPIO_Pin == GPIO_Pin_4)
		DAC_SetChannel1Data(DAC_Align_12b_R, value);  //12λ�Ҷ������ݸ�ʽ����DACֵ
	if (GPIO_Pin == GPIO_Pin_5)
		DAC_SetChannel2Data(DAC_Align_12b_R, value);  //12λ�Ҷ������ݸ�ʽ����DACֵ
}

/*
 * GPIOB�ڵ�PWMռ�ձȵ��ں���
 * �������PWM_Complementary��û�취����PB8��PB9��ռ�ձ�,��ʱPB8��PB9��ռ�ձȷֱ���PB6��PB7����
 * ����:
 * 		GPIO_Pin ���ڵ�PB�˿�����
 * 		Duty ���ڵ�ռ�ձ�ֵ(0~100)
 */
void DutyCycleB(uint16_t GPIO_Pin, uint8_t Duty)
{
	int16_t dutycycle = pwm_freq_B * Duty * 0.01;

	if (GPIO_Pin == GPIO_Pin_6)
	{
		DB1 = Duty;
		TIM_SetCompare1(TIM4, dutycycle);
#ifdef PWM_Complementary_B
		DB3=Duty;
		TIM_SetCompare3(TIM4, dutycycle);
#endif
	}
	if (GPIO_Pin == GPIO_Pin_7)
	{
		DB2 = Duty;
		TIM_SetCompare2(TIM4, dutycycle);
#ifdef PWM_Complementary_B
		DB4=Duty;
		TIM_SetCompare4(TIM4, dutycycle);
#endif
	}
#ifndef PWM_Complementary_B
	if (GPIO_Pin == GPIO_Pin_8)
	{
		DB3 = Duty;
		TIM_SetCompare3(TIM4, dutycycle);
	}
	if (GPIO_Pin == GPIO_Pin_9)
	{
		DB4 = Duty;
		TIM_SetCompare4(TIM4, dutycycle);
	}
#endif

}

/*
 * GPIOC�ڵ�PWMռ�ձȵ��ں���
 * ����:
 * 		GPIO_Pin ���ڵ�PB�˿�����
 * 		Duty ���ڵ�ռ�ձ�ֵ(0~100)
 */
void DutyCycleC(uint16_t GPIO_Pin, uint8_t Duty)
{
	int16_t dutycycle = pwm_freq_C * Duty * 0.01;

	if (GPIO_Pin == GPIO_Pin_6)
	{
		DC1 = Duty;
		TIM_SetCompare1(TIM8, dutycycle);
#ifdef PWM_Complementary_C
		DC3=Duty;
		TIM_SetCompare3(TIM8, dutycycle);
#endif
	}
	if (GPIO_Pin == GPIO_Pin_7)
	{
		DC2 = Duty;
		TIM_SetCompare2(TIM8, dutycycle);
#ifdef PWM_Complementary_C
		DC4=Duty;
		TIM_SetCompare4(TIM8, dutycycle);
#endif
	}
#ifndef PWM_Complementary_C
	if (GPIO_Pin == GPIO_Pin_8)
	{
		DC3 = Duty;
		TIM_SetCompare3(TIM8, dutycycle);
	}
	if (GPIO_Pin == GPIO_Pin_9)
	{
		DC4 = Duty;
		TIM_SetCompare4(TIM8, dutycycle);
	}
#endif
}

/*
 *  ����PWMƵ��
 *  ����:
 *  	GPIOx ֵΪGPIOB����GPIOC,����PB�ڻ���PC��PWM��Ƶ��
 *  	freq ����PWMƵ��,��Χ:1010~72000000Hz
 */
void SetPWMfreq(GPIO_TypeDef* GPIOx, int freq)
{
	uint16_t arr = 72000000 / (freq) + 1;
	if (arr > 65535)
		arr = 65535;
	if (GPIOx == GPIOB)
	{
		TIM_TimeBaseStructure.TIM_Period = arr;
		pwm_freq_B = arr;
		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
		TIM_SetCompare1(TIM4, pwm_freq_B * DB1 * 0.01);
		TIM_SetCompare2(TIM4, pwm_freq_B * DB2 * 0.01);
		TIM_SetCompare3(TIM4, pwm_freq_B * DB3 * 0.01);
		TIM_SetCompare4(TIM4, pwm_freq_B * DB4 * 0.01);
	}
	if (GPIOx == GPIOC)
	{
		TIM_TimeBaseStructure.TIM_Period = arr;
		pwm_freq_C = arr;
		TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
		TIM_SetCompare1(TIM8, pwm_freq_C * DC1 * 0.01);
		TIM_SetCompare2(TIM8, pwm_freq_C * DC2 * 0.01);
		TIM_SetCompare3(TIM8, pwm_freq_C * DC3 * 0.01);
		TIM_SetCompare4(TIM8, pwm_freq_C * DC4 * 0.01);
	}
}

/*
 * ��ʱ���ж�!!!��׼����û�в���
 * ʱ��Ƶ��Ϊ2kHz,��Сʱ��Ϊ0.5ms,����ʱʱ��Ϊ32s
 * ����:
 * 		times ��ʱʱ��,��Χ(1~32000)��λ:����
 */
void TIM7_init(uint16_t times)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //�����������ȼ�
	NVIC_Init(&NVIC_InitStructure); //���ô���Ĵ���

	if (times <= 0)
		times = 1000;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	//Ԥ��Ƶϵ��Ϊ36000-1,����������ʱ��Ϊ72MHz/36000 = 2kHz,�ֱ���Ϊ0.5ms
	TIM_TimeBaseStructure.TIM_Prescaler = 36000 - 1;
	//����ʱ�ӷָ� TIM_CKD_DIV1=0x0000,���ָ�
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	//���ü�����ģʽΪ���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//���ü��������С��ÿ��2000�����Ͳ���һ�������¼�
	TIM_TimeBaseStructure.TIM_Period = times * 2 - 1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; //������Ӧ�õ�TIM7��
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
	TIM_UpdateRequestConfig( TIM7, TIM_UpdateSource_Regular); //ʹ�ܼ�����
	TIM_Cmd(TIM7, ENABLE); //ʹ���ж�
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE); //�����־λ
	TIM_ClearFlag(TIM7, TIM_FLAG_Update);
}

void TIM7_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) == SET)
	{
		delay_us(100);
		TIM_ClearFlag(TIM7, TIM_FLAG_Update);
	}
}

/***************************************************CAN������ز���*******************************************************/
/*
 * CAN���߻�������
 * ����,�ж�,ʱ������
 */
void CAN_INIT()
{
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,
	        ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

#ifdef USE_CAN_PB
	//��ӳ�䵽PB�˿�
	/* Configure CAN pin: RX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure CAN pin: TX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);	   //��Ӱ��CAN IO�ŵ� PB8,PB9
#endif

#ifdef USE_CAN_PA
	//������ӳ��,PA�˿�
	/* Configure CAN pin: RX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure CAN pin: TX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif

	NVIC_InitTypeDef NVIC_InitStructure;
	/* Configure the NVIC Preemption Priority Bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	/* VECT_TAB_FLASH  */
	/* Set the Vector Table base location at 0x08000000 */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
	/* enabling interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
 * CAN���߻�������(��ѯģʽ,��ʹ���ж�)
 * ��������ģʽ,����Ƶ��(Ŀǰ250kHz),������
 */
void CAN_POLLING_INIT()
{
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	CAN_DeInit(CAN1);
//	CAN_OperatingModeRequest(CAN1, CAN_OperatingMode_Initialization);
	/* CAN register init */
	CAN_StructInit(&CAN_InitStructure);

	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = 0;
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
	CAN_InitStructure.CAN_Prescaler = 12;	   //  36M/(1+8+3)/12=250k
	CAN_Init(CAN1, &CAN_InitStructure);
//	CAN_OperatingModeRequest(CAN1, CAN_OperatingMode_Normal);
	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber = 1;
	CAN_FilterInitStructure.CAN_FilterActivation = DISABLE;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

}

/*
 * ��ѯģʽ��Ϣ����
 */
void CAN_POLLING_SEND(CanTxMsg TxMessage)
{
	uint8_t TransmitMailbox = 0;
	TransmitMailbox = CAN_Transmit(CAN1, &TxMessage);
	int i = 0;
	while ((CAN_TransmitStatus(CAN1, TransmitMailbox) != CANTXOK) && (i != 0xFF))
	{
		i++;
	}
}

/*
 * ��ѯģʽ��Ϣ����
 * ����CanRxMsg��������
 */
CanRxMsg CAN_POLLING_REC()
{
	int i = 0;
	while ((CAN_MessagePending(CAN1, CAN_FIFO0) < 1) && (i != 0xFF))
	{
		i++;
	}
	/* receive */
	CanRxMsg RxMessage;
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	return RxMessage;
}

/*
 * CAN�����жϷ�ʽ��ʼ��
 * ��������ģʽ,����Ƶ��(Ŀǰ250kHz),������
 */
void CAN_IT_INIT()
{
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;

	/* CAN register init */
	CAN_DeInit(CAN1);
//	CAN_OperatingModeRequest(CAN1, CAN_OperatingMode_Initialization);
	CAN_StructInit(&CAN_InitStructure);

	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
	CAN_InitStructure.CAN_Prescaler = 12;   //  36M/(1+8+3)/12=250k
	CAN_Init(CAN1, &CAN_InitStructure);
//	CAN_OperatingModeRequest(CAN1, CAN_OperatingMode_Normal);
	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber = 1;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	/* CAN FIFO0 message pending interrupt enable */
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);

}

/*
 * �ж�ģʽ���ݷ���
 */
void CAN_IT_SEND(CanTxMsg TxMessage)
{
	CAN_Transmit(CAN1, &TxMessage);
}

//CanRxMsg CAN_IT_REC();

/*
 * CAN���߽����ж�
 */
void USB_LP_CAN1_RX0_IRQHandler(void)
{

	CanRxMsg RxMessage;

	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	Analysys(RxMessage);

}

