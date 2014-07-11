//	22.09.2006

#ifndef	STDAFX
#define STDAFX

#include "io.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "ds1307.h"
#include "eeprom.h"

#define PARAMS_COUNT	9				//  ���-�� ������������� ����������

//  ���� ����� g_DefferedWork
#define DW_SENDOHRANAEVENT		0

#define MAX_PASSWORD_LEN		10		//  ���� ����� ������ ������
#define HELLO_TIMEOUT			3

#define MAX_MAIN_SETUP_ITEMS	6			//7

#define MAX_SENSORS_ITEMS				2	//4 ��������� ������
#define MAX_TELEFON_ITEMS				4
#define MAX_DEVICE_ITEMS				4
#define MAX_TEL_LINEMODE_ITEMS			2
#define MAX_VOLUME_ITEMS				2
#define MAX_SET_SENS_ITEMS				8
#define MAX_X232_ITEMS				2	//3
#define MAX_TEL_AUTO_DIAL_NUMBERS_ITEMS	6
#define MAX_SETTINGS_ITEMS				5
#define	MAX_SENSOR_COUNT				2

//  ����� ����� ����� ��� ������ �������
#define MAX_INPUT				16

#define INPUT_LEN_TEL			15
#define INPUT_LEN_TIME			16
#define INPUT_LEN_PSWD			8
#define INPUT_LEN_BAUDRATE		6
#define INPUT_LEN_TIMEOUT		1

//  ���� ��������, ������������ � "�������� ����������"
#define ITEM_CHECKED			255
#define ITEM_UNCHECKED			0xEF

//  ���������� �������
#define	LINE_1_START_POS		0x00
#define	LINE_2_START_POS		0x40
#define LINE_LEN				0x10
#define ARROW_START_POS			0x0B
#define CHECKBOX_START_POS		0x0F

#define HF			_BV(PB7)		//  ���������� �������� ��������
#define PWR			_BV(PD4)		//
#define CSR			PB5				//  ����� MCP42010 �� SPI

#define MC_8K		_BV(PD7)		//  8 kHz ��� MC14LC5480
#define MC_DT		_BV(PG3)		//	DT ��� MC14LC5480
#define MC_DR		_BV(PG4)		//  DR ��� MC14LC5480
#define MC_2048		_BV(PE3)		//  2048 ��� MC14LC5480

#define LCD_RW		_BV(PG0)		//  ���������� ����������
#define LCD_E		_BV(PG1)		//  ���������� ����������
#define LCD_RS		_BV(PG2)		//  ���������� ����������

enum	{	MD_TEL,
/* 1*/		MD_SETUP_TIME,			//  ��������� �������
		 	MD_SETUP_WAITPSWD, 		//  �������� ����� ������ � Setup
/* 3*/		MD_HELLO,
			MD_OHRANA,				//  ������� ��� �������
			MD_SETUP_NEWPSWD,
			MD_SETUP_SENSORS_NEWBAUDRATE,
/* 7*/		MD_SETUP_SENSORS_LIST_DEVICE_TIMEOUT,
			MD_SETUP_SENSORS_LIST_DEVICE_SETPORT,
			MD_SETUP_SENSORS_LIST_DEVICE_SENS_VALUE,
			MD_SENSORS,
			MD_SETUP,
			MD_SETUP_TEL,
			MD_SETUP_SENSORS_LIST,
			MD_SETUP_SENSORS_LIST_DEVICE,
/*15*/		MD_SETUP_SENSORS_ENUM,
			MD_SETUP_TEL_LINEMODE,
			MD_SETUP_VOLUME,
			MD_SETUP_VOLUME_DINAMIC,
			MD_SETUP_VOLUME_TELEFON,
			MD_SETUP_SENSORS_LIST_DEVICE_SENSLIST,
			MD_SETUP_X232,
			MD_SETUP_X232_TIMEOUT,
			MD_SETUP_X232_INTERVAL,
/*24*/		MD_SETUP_X232_PACKET,
			MD_SETUP_TEL_AUTO_DIAL_NUMBERS,
			MD_SETUP_TEL_AUTO_DIAL_NUMBER,
			MD_SETUP_SETTINGS,
			MD_SETUP_SETTINGS_RAM,
/*29*/		MD_SETUP_SETTINGS_EEPROM,
/*30*/		MD_SETUP_SENSORS_LIST_DEVICE_SENSOR,
/*31*/		MD_SETUP_SENSORS_LIST_DEVICE_SENSOR_TIMEOUT1,
/*32*/		MD_REDUCE_EDIT,
			MD_REDUCE_EDIT_


		} ;//g_mdenum;

//  ������ �������� ����
enum	{	SIN_SENSORS,
	//		SIN_GSM,
			SIN_DATETIME,
			SIN_TELEFON,
			SIN_VOLUME,
			SIN_X232,
			//SIN_ZVUK,
			SIN_SETTINGS

		} ;//g_SetupItemNames;

//  ������ ���� Sensors
enum	{
			SEN_LIST,
			//SEN_ENUM,
			SEN_BAUDRATE,
			//SEN_POLL
		} ;//g_SensorsItemnames;

//  ������ ���� Device
enum	{
			SDN_SETPORT,		//  ����� ���� ����� ������, ����� �������
			SDN_SENSORS,		//  ����� ��������������� �����-�����
			SDN_TIMEOUT,		//  �������� ����� ����������� ��� ������
			SDN_HF_POWER

		} ;//g_DevicetemNames;

//  ������ ���� �����
enum 	{
			SRN_LEVELS,
			SRN_TIMEOUT
		};

//  ������ ���� Telefon
enum	{
			STN_AUTO_DIAL_NUMBERS,
			STN_AON,
			STN_LINE_MODE,
			STN_ADV_LINE_MODE
		} ;//g_TelItemNames;

//  ������ ���� Volume
enum	{
			SVN_DINAMIC,
			SVN_TELEFON

		} ;//g_VolItemNames;

//  ������ ���� X232
enum	{
			SXN_TIMEOUT,
			SXN_INTERVAL,
			SXN_PACKET
		};

//  ������ ���� ���������
enum	{
			SSE_RAM,
			SSE_EEPROM,
			SSE_VERSION,
			SSE_PASSWORD,
			SSE_SETDEFAULT
		};

//  ������ � EEPROM

//  --������ �� ���� � Setup
#define EADR_SETUP_PSWD_		0
//  --����� ������� �������
#define EADR_START_TIME			EADR_SETUP_PSWD_ + MAX_PASSWORD_LEN
#define EADR_USART_BAUDRATE		EADR_START_TIME + sizeof(RTC_TIME)
//  --���-�� ������������ ��������� �� ������ ���������� ����������
#define EADR_DEVICE_COUNT		EADR_USART_BAUDRATE + sizeof(unsigned char)
//  ����� ���������� ����� - Pulse ��� Tone
#define	EADR_LINE_MODE			EADR_DEVICE_COUNT + sizeof(unsigned char)
//  ������� ��������� �������� ������� �����
#define	EADR_VOLUME_DINAMIC		EADR_LINE_MODE + sizeof(unsigned char)
//  ������� ��������� ��������
#define	EADR_VOLUME_TELEFON		EADR_VOLUME_DINAMIC + sizeof(unsigned char)
//  ������� ��������� X232
#define	EADR_X232_TIMEOUT		EADR_VOLUME_TELEFON + sizeof(unsigned char)
//  �������� ������ ��������� ��������� X232
#define	EADR_X232_INTERVAL		EADR_X232_TIMEOUT + sizeof(unsigned char)

#define EADR_AUTO_DIAL_NUMBERS	EADR_X232_INTERVAL + sizeof(unsigned char)

#define EADR_DTMF_PSWD			EADR_AUTO_DIAL_NUMBERS + 10 * (MAX_INPUT + 1)
#define EADR_SETUP_PSWD			EADR_DTMF_PSWD + 16

#define EADR_LAST_MODE			EADR_SETUP_PSWD + 16
#define EADR_ADV_LINE_MODE			EADR_LAST_MODE + 1
#define EADR_CAN_SHOW_TIME			EADR_ADV_LINE_MODE + 1

//  --������ ������������ ��� ������������ ���������
#define EADR_DEVICE_LIST		0x100


#define	USER_TIME_COUNTER		OCR3C

#define DB	"\xDB"
/*
#ifndef F_CPU
	#define F_CPU	16000000L
#endif
*/
#endif

