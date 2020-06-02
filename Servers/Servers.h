/*
 * Servers.h
 *
 *  Created on: 2020��4��26��
 *      Author: WangXiang
 */

#ifndef SERVERS_H_
#define SERVERS_H_
#include "UserConfig.h"

#define P1ID 0x0CFF1081
#define P2ID 0x10FF1182
#define P3ID 0x10FF1283
#define P4ID 0x10FF1384

/*
 * ID����֮�����ݽṹ���ʽ
 */
typedef struct
{
	uint8_t Pri;
	uint16_t PGN;
	uint8_t PF;
	uint8_t PSGE;
	uint8_t SA;
	uint8_t DLC;
} ID_INFO;

/*
 * EXCEL���е�1������Ľ�����ʽ
 */
typedef struct
{
	uint8_t total_pack; 			//�ܰ���
	uint8_t current_pack; 			//��ǰ�����
	uint8_t whell_speed; 			//������ת��/ת�ٴ�����
	bool is_on_work; 				//��ҵ״̬
	uint16_t ultrasonic_sensor; 	//����/������������
	uint8_t rotating_speed; 		//��̨����ת��/ת�ٴ�����
	uint16_t drive_speed; 			//���Ͳ�������ת��/ת�ٴ�����
	uint8_t car_speed; 				//����/�����״ﴫ����
	bool ATW;
} Mail_Box1;

/*
 * EXCEL���е�2������Ľ�����ʽ
 */
typedef struct
{
	uint8_t total_pack;				//�ܰ���
	uint8_t current_pack;			//��ǰ�����
	uint16_t roller_speed;			//����������Ͳ/ת�ٴ�����
	uint16_t Yroller_speed;			//������������Ͳ/ת�ٴ�����
	uint16_t wind_speed;			//���ת��/ת�ٴ�����
	uint8_t driver_speed;			//������ת��/ת�ٴ�����
	bool ATW;
} Mail_Box2;

/*
 * EXCEL���е�3������Ľ�����ʽ
 */
typedef struct
{
	uint8_t total_pack;				//�ܰ���
	uint8_t current_pack;			//��ǰ�����
	uint16_t shock_speed;			//��ɸ������ת��/ת�ٴ�����
	uint16_t Xroller_speed;			//����ˮƽ����ת��/ת�ٴ�����
	uint16_t Xrest_speed;			//����ˮƽ����ת/ת�ٴ�����
	uint8_t keep;					//����
	bool ATW;
} Mail_Box3;

/*
 * EXCEL���е�4������Ľ�����ʽ
 */
typedef struct
{
	uint8_t total_pack;				//�ܰ���
	uint8_t current_pack;			//��ǰ�����
	uint16_t high;					//���߶�/���߶ȴ�����
	bool ATW;
} Mail_Box4;

/*
 * EXCEL���е�5������Ľ�����ʽ
 */
typedef struct
{
	uint16_t cleanlost_sensor;		//��ѡ��ʧ/��ѡ��ʧ������
	uint16_t cliplost_sensor;		//�д���ʧ/�д���ʧ������
	uint8_t angle;					//����ɸ����
	bool ATW;
} Mail_Box5;

/*
 * EXCEL���е�6������Ľ�����ʽ
 */
typedef struct
{
	uint8_t pure_value;				//������/�����ʴ�����
	uint8_t break_value;			//������/�����ʴ�����
	bool ATW;
} Mail_Box6;

/*
 * EXCEL���е�7������Ľ�����ʽ
 */
typedef struct
{
	uint8_t float_value;			//��������/����������⴫����
	bool ATW;
} Mail_Box7;

/*
 * EXCEL���е�1������Ļ���
 */
typedef struct
{
	ID_INFO ID_INFO;				//ID���ݽ����ṹ��
	Mail_Box1 Mail_Box[3];			//���ݻ�����
	uint32_t ID;					//����ID
} Cach1;

/*
 * EXCEL���е�2������Ļ���
 */
typedef struct
{
	ID_INFO ID_INFO;
	Mail_Box2 Mail_Box[3];
	uint32_t ID;
} Cach2;

/*
 * EXCEL���е�3������Ļ���
 */
typedef struct
{
	ID_INFO ID_INFO;
	Mail_Box3 Mail_Box[3];
	uint32_t ID;
} Cach3;

/*
 * EXCEL���е�4������Ļ���
 */
typedef struct
{
	ID_INFO ID_INFO;
	Mail_Box4 Mail_Box[3];
	uint32_t ID;
} Cach4;

/*
 * EXCEL���е�5������Ļ���
 */
typedef struct
{
	ID_INFO ID_INFO;
	Mail_Box5 Mail_Box[3];
	uint32_t ID;
} Cach5;

/*
 * EXCEL���е�6������Ļ���
 */
typedef struct
{
	ID_INFO ID_INFO;
	Mail_Box6 Mail_Box[3];
	uint32_t ID;
} Cach6;

/*
 * EXCEL���е�7������Ļ���
 */
typedef struct
{
	ID_INFO ID_INFO;
	Mail_Box7 Mail_Box[3];
	uint32_t ID;
} Cach7;


typedef struct
{
	char UTCtime[10];
	char latitude[15];
	char longitude[15];//����
	char NorS;
	char EorW;
	char AorP;
	bool ATW;

}GPS_DATA;

typedef struct
{
	bool cmdHead;
	uint8_t matchCount;
	uint8_t dataCount;
	char match[5];
	uint8_t splitTime;
	uint8_t totalmatch;
	bool ATR;

}GPS_INIT;

void Analysys(CanRxMsg CANRX);
void Analysys_ID(uint8_t Pack_NO, CanRxMsg CANRX);
uint8_t set_Pri(uint32_t id);
void Analysys_DATA(uint8_t Pack_NO, CanRxMsg CANRX);

bool iscontants(char *str, char *cmd);
void send_cmd(char *str);
bool check_receives(uint8_t cmd_number, char *cmd);
void BC28_RESET();
void BC28_Init();
void pack_to_aliyun();


void writeUTC(char *time,char *data);
void writeL(char *lan,char *data);
void clearStr(char *str,uint8_t i);

void anaGPS();

uint8_t get_pack_currentPack(CanRxMsg CANRX);


#endif /* SERVERS_H_ */
