/*
********************************************************************************
*
*                                 bsp.h
*
* File          : bsp.h
* Version       : V1.0
* Author        : whq
* Mode          : Thumb2
* Toolchain     : 
* Description   : BSPӲ����ʼ��
*                
* History       :
* Date          : 2013.07.21
*******************************************************************************/


#ifndef _BSP_H_
#define _BSP_H_


#include <stdint.h>
#include <stddef.h>

/******************************ϵͳ����****************************************/

typedef enum {
    eLED_0,
    eLED_1,
    eLED_2,
    
    eGPIO_MAX_COUNT,
}GPIO_ENUM_t;



/******************************�궨��******************************************/

//LED��
#define LED_OFF(a)              BSP_GpioSetm(a)
//LED��
#define LED_ON(a)               BSP_GpioResetm(a)


//IO�ø�
#define IO_SET(a)               BSP_GpioSet(a)
//IO�õ�
#define IO_RESET(a)             BSP_GpioReset(a)
//��IO���ϵĵ�ƽ
#define IO_READ(a)              BSP_GpioRead(a)
//IO��ƽ��ת
#define IO_TOGGLE(a)            BSP_GpioToggle(a)


#define BSP_CONEOF(a)           ((sizeof(a)) / (sizeof(*a)))



/******************************************************************************/


void Delay(volatile int32_t nCount);
void IWDG_Init(void);
void SysTickInit(void);
void WDG_Clear(void);
void BSP_Init(void);

void BSP_GpioSet(int32_t n);
void BSP_GpioReset(int32_t n);
int32_t BSP_GpioRead(int32_t n);
void BSP_GpioToggle(int n);

void BSP_PortWrite(int32_t n, int32_t value);
void BSP_SystemReset(void);



#endif
/******************************END*********************************************/

