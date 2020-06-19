/*
 * CAN.c
 *
 *  Created on: 2020年6月3日
 *      Author: wxujs
 */
#include "CAN.h"

/*Define CAN bus structure variables*/
Cach1 Pack1;
Cach2 Pack2;
Cach3 Pack3;
Cach4 Pack4;
Cach5 Pack5;
Cach6 Pack6;
Cach7 Pack7;
ID_INFO ID_INFOs;

/*
 * 解析CAN接收数据包的ID和数据格式
 */
void Analysys(CanRxMsg CANRX)
{
	switch (CANRX.ExtId)
	{
		case P1ID:
			Analysys_DATA(get_pack_currentPack(CANRX), CANRX);
			break;
		case P2ID:
			Analysys_DATA(get_pack_currentPack(CANRX), CANRX);
			break;
		case P3ID:
			Analysys_DATA(get_pack_currentPack(CANRX), CANRX);
			break;
		case P4ID:
			Analysys_DATA(get_pack_currentPack(CANRX), CANRX);
			break;
			//		default:
	}
}

/*
 * 解析数据包ID中的Pri
 */
uint8_t set_Pri(uint32_t id)
{
	id = (id & 0xFF000000) >> 24;
	switch (id)
	{
		case 0x0C:
			return 3;
		case 0X10:
			return 4;
		default:
			return 5;
	}
}

/*
 * 分析数据包的ID
 * Pack_No:	包序号,对应第几个数据包,决定将本条数据解析到第几个结构体中,对应EXCEL表中数据序号(1~7)
 * CANRX:	接收的CAN总线数据结构体
 */
void Analysys_ID(uint8_t Pack_NO, CanRxMsg CANRX)
{
	ID_INFOs.Pri = set_Pri(CANRX.ExtId);
	ID_INFOs.PGN = (CANRX.ExtId & 0x00FFFF00) >> 8;
	ID_INFOs.PF = (CANRX.ExtId & 0x00FF0000) >> 16;
	ID_INFOs.PSGE = (CANRX.ExtId & 0x0000FF00) >> 8;
	ID_INFOs.SA = (CANRX.ExtId & 0x000000FF);
	ID_INFOs.DLC = CANRX.DLC;
	if (Pack_NO == 1)
		Pack1.ID_INFO = ID_INFOs, Pack1.ID = CANRX.ExtId;
	else if (Pack_NO == 2)
		Pack2.ID_INFO = ID_INFOs, Pack2.ID = CANRX.ExtId;
	else if (Pack_NO == 3)
		Pack3.ID_INFO = ID_INFOs, Pack3.ID = CANRX.ExtId;
	else if (Pack_NO == 4)
		Pack4.ID_INFO = ID_INFOs, Pack4.ID = CANRX.ExtId;
	else if (Pack_NO == 5)
		Pack5.ID_INFO = ID_INFOs, Pack5.ID = CANRX.ExtId;
	else if (Pack_NO == 6)
		Pack6.ID_INFO = ID_INFOs, Pack6.ID = CANRX.ExtId;
	else if (Pack_NO == 7)
		Pack7.ID_INFO = ID_INFOs, Pack7.ID = CANRX.ExtId;
}

/*
 * 按照EXCEL表格中数据顺序返回其是第几条数据,便于使用不同方法进行解析数据
 */
uint8_t get_pack_currentPack(CanRxMsg CANRX)
{
	if (CANRX.ExtId == P1ID)
		return (CANRX.Data[0] & 0x0F);
	else
	{
		if (CANRX.ExtId == P2ID)
			return 5;
		if (CANRX.ExtId == P3ID)
			return 6;
		if (CANRX.ExtId == P4ID)
			return 7;
	}
	return 0;
}

/*
 * 解析数据
 * Pack_No:	使用第几个方式解包,不同序号的数据包对应不同的数据解析方式
 * CANRX:	接收的CAN总线数据包
 *
 * !!使用前需要先将Mail_Box解锁,即将ATW置true!!
 */
void Analysys_DATA(uint8_t Pack_NO, CanRxMsg CANRX)
{
	if (Pack_NO == 1)
	{
		Pack1.ID = CANRX.ExtId;
		Pack1.Mail_Box.total_pack = (CANRX.Data[0] & 0xF0) >> 4;
		Pack1.Mail_Box.current_pack = (CANRX.Data[0] & 0x0F);
		Pack1.Mail_Box.whell_speed = (CANRX.Data[1] & 0xFE) >> 1;
		Pack1.Mail_Box.is_on_work = (CANRX.Data[1] & 0x01);
		Pack1.Mail_Box.ultrasonic_sensor = (CANRX.Data[3] << 8) | CANRX.Data[2];
		Pack1.Mail_Box.rotating_speed = CANRX.Data[4];
		Pack1.Mail_Box.drive_speed = (CANRX.Data[6] << 8) | CANRX.Data[5];
		Pack1.Mail_Box.car_speed = CANRX.Data[7];
		Pack1.Mail_Box.ATW = false;
	} else if (Pack_NO == 2)
	{
		Pack2.ID = CANRX.ExtId;
		Pack2.Mail_Box.total_pack = (CANRX.Data[0] & 0xF0) >> 4;
		Pack2.Mail_Box.current_pack = (CANRX.Data[0] & 0x0F);
		Pack2.Mail_Box.roller_speed = (CANRX.Data[2] << 8) | CANRX.Data[1];
		Pack2.Mail_Box.Yroller_speed = (CANRX.Data[4] << 8) | CANRX.Data[3];
		Pack2.Mail_Box.wind_speed = (CANRX.Data[6] << 8) | CANRX.Data[6];
		Pack2.Mail_Box.driver_speed = CANRX.Data[7];
		Pack2.Mail_Box.ATW = false;
	} else if (Pack_NO == 3)
	{
		Pack3.ID = CANRX.ExtId;
		Pack3.Mail_Box.total_pack = (CANRX.Data[0] & 0xF0) >> 4;
		Pack3.Mail_Box.current_pack = (CANRX.Data[0] & 0x0F);
		Pack3.Mail_Box.shock_speed = (CANRX.Data[2] << 8) | CANRX.Data[1];
		Pack3.Mail_Box.Xroller_speed = (CANRX.Data[4] << 8) | CANRX.Data[3];
		Pack3.Mail_Box.Xrest_speed = (CANRX.Data[6] << 8) | CANRX.Data[5];
		Pack3.Mail_Box.keep = CANRX.Data[7];
		Pack3.Mail_Box.ATW = false;
	} else if (Pack_NO == 4)
	{
		Pack4.ID = CANRX.ExtId;
		Pack4.Mail_Box.total_pack = (CANRX.Data[0] & 0xF0) >> 4;
		Pack4.Mail_Box.current_pack = (CANRX.Data[0] & 0x0F);
		Pack4.Mail_Box.high = (CANRX.Data[2] << 8) | CANRX.Data[1];
		Pack4.Mail_Box.ATW = false;
	} else if (Pack_NO == 5)
	{
		Pack5.ID = CANRX.ExtId;
		Pack5.Mail_Box.cleanlost_sensor = (CANRX.Data[1] << 8) | CANRX.Data[0];
		Pack5.Mail_Box.cliplost_sensor = (CANRX.Data[3] << 8) | CANRX.Data[2];
		Pack5.Mail_Box.angle = CANRX.Data[4];
		Pack5.Mail_Box.ATW = false;
	} else if (Pack_NO == 6)
	{
		Pack6.ID = CANRX.ExtId;
		Pack6.Mail_Box.pure_value = CANRX.Data[0];
		Pack6.Mail_Box.break_value = CANRX.Data[1];
		Pack6.Mail_Box.ATW = false;
	} else if (Pack_NO == 7)
	{
		Pack7.ID = CANRX.ExtId;
		Pack7.Mail_Box.ATW = false;
	}
}

/*
 *将所有CAN数据接收缓冲区的ATW标记为置位
 */
void SetFalgATW()
{
	Pack1.Mail_Box.ATW = true;						//允许CAN总线刷新获取数据
	Pack2.Mail_Box.ATW = true;
	Pack3.Mail_Box.ATW = true;
	Pack4.Mail_Box.ATW = true;
	Pack5.Mail_Box.ATW = true;
	Pack6.Mail_Box.ATW = true;
	Pack7.Mail_Box.ATW = true;
}

/*
 * 返回CAN解析之后的数据
 */
CANpacks GetCANRx()
{
	CANpacks packs;
	packs.CANpack1 = Pack1;
	packs.CANpack2 = Pack2;
	packs.CANpack3 = Pack3;
	packs.CANpack4 = Pack4;
	packs.CANpack5 = Pack5;
	packs.CANpack6 = Pack6;
	packs.CANpack7 = Pack7;
	return packs;
}

/*
 *返回CAN数据有多少位没有被更新
 */
uint8_t CheckATW()
{
	uint8_t number = 0;
	if (Pack1.Mail_Box.ATW)
		number++;
	if (Pack2.Mail_Box.ATW)
		number++;
	if (Pack3.Mail_Box.ATW)
		number++;
	if (Pack4.Mail_Box.ATW)
		number++;
	if (Pack5.Mail_Box.ATW)
		number++;
	if (Pack6.Mail_Box.ATW)
		number++;
	if (Pack7.Mail_Box.ATW)
		number++;
	return number;
}

/* Clear CAN bus data packets */
void Clear_CAN_Box()
{
	Pack1.Mail_Box.whell_speed = 0;
	Pack1.Mail_Box.is_on_work = 0;
	Pack1.Mail_Box.ultrasonic_sensor = 0;
	Pack1.Mail_Box.rotating_speed = 0;
	Pack1.Mail_Box.drive_speed = 0;
	Pack1.Mail_Box.car_speed = 0;
	Pack2.Mail_Box.roller_speed = 0;
	Pack2.Mail_Box.Yroller_speed = 0;
	Pack2.Mail_Box.wind_speed = 0;
	Pack2.Mail_Box.driver_speed = 0;
	Pack3.Mail_Box.shock_speed = 0;
	Pack3.Mail_Box.Xroller_speed = 0;
	Pack3.Mail_Box.Xrest_speed = 0;
	Pack4.Mail_Box.high = 0;
	Pack5.Mail_Box.cleanlost_sensor = 0;
	Pack5.Mail_Box.cliplost_sensor = 0;
	Pack5.Mail_Box.angle = 0;
	Pack6.Mail_Box.pure_value = 0;
	Pack6.Mail_Box.break_value = 0;
	Pack7.Mail_Box.float_value = 0;
}

/* Get CAN bus data */
void Get_CAN_data(CAN_buffer1 CAN_Buffer_1, CAN_buffer2 CAN_Buffer_2, CAN_buffer3 CAN_Buffer_3,
        CAN_buffer4 CAN_Buffer_4, CAN_buffer5 CAN_Buffer_5, CAN_buffer6 CAN_Buffer_6,
        CAN_buffer7 CAN_Buffer_7)
{
	CAN_Buffer_1.whell_speed = Pack1.Mail_Box.whell_speed;
	CAN_Buffer_1.is_on_work = Pack1.Mail_Box.is_on_work;
	CAN_Buffer_1.ultrasonic_sensor = Pack1.Mail_Box.ultrasonic_sensor;
	CAN_Buffer_1.rotating_speed = Pack1.Mail_Box.rotating_speed;
	CAN_Buffer_1.drive_speed = Pack1.Mail_Box.drive_speed;
	CAN_Buffer_1.car_speed = Pack1.Mail_Box.car_speed;
	CAN_Buffer_2.roller_speed = Pack2.Mail_Box.roller_speed;
	CAN_Buffer_2.Yroller_speed = Pack2.Mail_Box.Yroller_speed;
	CAN_Buffer_2.wind_speed = Pack2.Mail_Box.wind_speed;
	CAN_Buffer_2.driver_speed = Pack2.Mail_Box.driver_speed;
	CAN_Buffer_3.shock_speed = Pack3.Mail_Box.shock_speed;
	CAN_Buffer_3.Xroller_speed = Pack3.Mail_Box.Xroller_speed;
	CAN_Buffer_3.Xrest_speed = Pack3.Mail_Box.Xrest_speed;
	CAN_Buffer_4.high = Pack4.Mail_Box.high;
	CAN_Buffer_5.cleanlost_sensor = Pack5.Mail_Box.cleanlost_sensor;
	CAN_Buffer_5.cliplost_sensor = Pack5.Mail_Box.cliplost_sensor;
	CAN_Buffer_5.angle = Pack5.Mail_Box.angle;
	CAN_Buffer_6.pure_value = Pack6.Mail_Box.pure_value;
	CAN_Buffer_6.break_value = Pack6.Mail_Box.break_value;
	CAN_Buffer_7.float_value = Pack7.Mail_Box.float_value;
	CAN_Buffer_1.ATW = Pack1.Mail_Box.ATW;
	CAN_Buffer_2.ATW = Pack2.Mail_Box.ATW;
	CAN_Buffer_3.ATW = Pack3.Mail_Box.ATW;
	CAN_Buffer_4.ATW = Pack4.Mail_Box.ATW;
	CAN_Buffer_5.ATW = Pack5.Mail_Box.ATW;
	CAN_Buffer_6.ATW = Pack6.Mail_Box.ATW;
	CAN_Buffer_7.ATW = Pack7.Mail_Box.ATW;
}
