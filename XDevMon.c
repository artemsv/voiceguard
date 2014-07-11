//  28.09.2006
#include <stdlib.h>
#include <string.h>
#include "stdafx.h"

#include "eeprom.h"
#include "uart.h"
#include "utils.h"
#include "lcd.h"
#include "at45.h"
#include "fatx.h"
#include "a2d.h"

#include "kbd.h"

#include "vg.h"
#include "tim.h"

#include "xdevmon.h"

//  �� VG.C
extern unsigned char*	g_szInput;
extern unsigned char	g_curDev;
extern unsigned char	g_ShowTime;
extern unsigned short 	g_params[PARAMS_COUNT];

extern unsigned short 	rootdir[FILE_ENTRY_IN_PAGE];
void GSM_Transmit(unsigned char data);
//  ���� ���� state ��������� XDEVICE
#define DEVSTATE_EXIST				0x01	//  ���������� ����������
#define DEVSTATE_CONNECTED			0x02	//  ���������� ����������

//  ������� ������ ��������� ����� �� GetRespond
#define WAIT_FOR_RESPOND			5

//	���������� ������� ������� �����
#define MAX_TRY_COUNT				3

//  ����� ���������� �� ����
#define VG_ADR						2

//  ������� ��������� X232
#define CMD_CONNECT       			0x0000
#define CMD_GETSTATUS     			0x0001
#define CMD_SETPORT      			0x0002
#define CMD_OUTPORT       			0x0003
#define CMD_SETADR        			0x0004
#define CMD_SHOWCLOCK    			0x0005
#define CMD_SETOHRANA				CMD_SHOWCLOCK
#define CMD_DISCONNECT    			0x0006
#define CMD_HIDECLOCK     			0x0007
#define CMD_CLEARSCREEN   			0x0008
#define CMD_WRITEDIGIT    			0x0009
#define CMD_FINDDEV					0x000A
#define CMD_SETSENSORSENSITIVITY	0x000B
#define CMD_GETSENSORVALUE			0x000C
#define CMD_ADC_ENABLE				0x000D
#define CMD_SETOHRTIMEOUT			0x000E
#define CMD_GETEEPROM				0x000F
#define CMD_GETRAM					0x0010

//  extended messages codes
#define CMD_SETTIME					0x0011	//  ���������� �����
#define CMD_GETTIME					0x0012	//  �������� �����
#define CMD_GETSENSORSENSITIVITY	0x0013
#define CMD_AT45					0x0014
#define CMD_FATX					0x0015
#define CMD_TEL						0x0016
#define CMD_SPEAKERVOLUME			0x0017
#define CMD_GSM						0x0018
#define CMD_ZVUK					0x0019

// CMD_AT45
#define CMD_AT45_READ_BUFFER		0x0000
#define CMD_AT45_READ_PAGE_TO_BUF	0x0001
#define CMD_AT45_WRITE_BUFFER		0x0002
#define CMD_AT45_WRITE_BUF_TO_PAGE	0x0003
#define CMD_AT45_SET_READ_POS		0x0004
#define CMD_AT45_SET_WRITE_POS		0x0005
#define CMD_AT45_FILL_BUF			0x0006
#define CMD_AT45_READ_BYTE			0x0007
#define CMD_AT45_SEND_OKTET			0x0008
#define CMD_AT45_ERASE_PAGES		0x0009

// CMD_FATX
#define CMD_FATX_FILE_LIST			0x0000
#define CMD_FATX_CREATE_FILE		0x0001
#define CMD_FATX_DELETE_FILE		0x0002
#define CMD_FATX_ADD_PAGE			0x0003
#define CMD_FATX_FORMAT				0x0004
#define CMD_FATX_GET_FILE_PAGES		0x0005
#define CMD_FATX_WRITE_PAGE			0x0006
#define CMD_FATX_OPEN_FILE			0x0007
#define CMD_FATX_GET_ROOTDIR		0x0008
#define CMD_FATX_READ_PAGE			0x0009
#define CMD_FATX_SEEK				0x000A
#define CMD_FATX_READ_BYTE			0x000B
#define CMD_FATX_WRITE_BYTE			0x000C
#define CMD_FATX_FLUSH_FILE			0x000D
#define CMD_CLOSE_FILE				0x000E

// CMD_TEL
#define CMD_TEL_BUTTON				0x0000

//  ���-�� ���� EEPROM'�, ������� ���������� � ����� �� CMD_GETEEPROM
#define EEPROM_SEND_SIZE	8

PACKET g_packetReply;

PACKET g_lastPacket;			//  ��������� ��������� �����
PACKET g_readyPacket;			//	���������, ������������ � ������� ��������
unsigned char g_PollEnable;		//  1 - �������� ����� ������������ ���������

typedef unsigned char* pMas;

unsigned char g_packetReplyPos = 0;

//  ������� ��������� X232. ���������� � �������������.
//	���� ���������� �� �������� � ������� ����� �������, ���������,
//  ��� ��������� ���� ��� �����, � ���������� ��������� "���������"
unsigned char g_x232TimeOut;
//  �������� ������ ��������� ��������� X232
unsigned char g_x232Interval;

//  ���������� ���������
//unsigned char g_xDevCount = 0;
//  ���������� ������� ������� �����
//unsigned char g_tryCount = 0;

#define LINK_STATUS_NOREPLAY	0x01		//  1 - ���� ������������ ������
#define LINK_STATUS_SPECPACKET	0x02		//  1 - ����� ��������� ��� �������
#define LINK_STATUS_RESPOND		0x04		//  1 - ������ ����� �� GetRespond

#define LINK_STATUS_DOR			0x08
#define LINK_STATUS_FE			0x10
#define LINK_STATUS_PE			0x20

//  ��������� ����� � ������������ - ������ ������
//unsigned char g_linkStatus = 0;

LINK_STATE link_state;

//  ������ � ������������� ������������. ����� ������ ������� ������������� �����
//  ����, �������� ����� 3, ������ - ����� 4 � �.�
XDEVICE g_devList[MAX_DEVICE_COUNT];

unsigned short g_devListReactionTimeouts[MAX_DEVICE_COUNT][8];

CUR_DEVICE g_CurDev;
unsigned char g_WaitableTimer;
unsigned char g_DecTimeoutDev;

//	�������� ������ ��������
unsigned char devStatus = 0;

//  ��������� ����� ��� ������ ����� - ���������� �������� �� ������
unsigned char 	g_StreamMode;				//  TRUE - ����� �������
unsigned short	g_StreamStartPage;			//  � ����� �������� �������� ������ �����
unsigned short	g_StreamEndPage;			// 	�� ����� �������� ������������
// 	������� ������ ������� �������� ��� ��������
unsigned short  g_StreamCurByteCounter;

void InitDevices(void)
{
	unsigned char k;

	//  ������ ���������� ������������ ���������
	//  �� ������� ���� ���� ����������(����������) ������������ ������
	link_state.g_xDevCount = eeprom_read_byte_1((unsigned char*)EADR_DEVICE_COUNT);
	link_state.g_xDevCount = 1;

	//  ������ �� ���� - ����� �� ���������� ����� ������ ��� ���������
	//link_state.g_linkStatus |= LINK_STATUS_NOREPLAY;

	//  � ������� ������� ������� ���������� ��� - �� ������ ��� ���������
	//link_state.g_linkStatus &= ~LINK_STATUS_SPECPACKET;

	g_x232TimeOut = eeprom_read_byte_1((unsigned char*)EADR_X232_TIMEOUT);
	g_x232Interval = eeprom_read_byte_1((unsigned char*)EADR_X232_INTERVAL);
	if (g_x232Interval > 16)
		g_x232Interval = 10;

	//  ������ ������ ��������� � �������� � �����������
	//  ������ ����� ��������� XDEVICE, ��� ������� ���������, ������� ������ � ���
	for (k = 0; k < MAX_DEVICE_COUNT; k++)
	{
		epmrbl(&g_devList[k], (const uint8_t *)(EADR_DEVICE_LIST + k*sizeof(XDEVICE)),
			sizeof(XDEVICE)/* - 16*/);

		memset(&g_devList[k].sensorTimeout, 0, 16);
	}

	g_devList[0].adr = 0;
	g_devList[0].state = DEVSTATE_EXIST;
#ifdef NEWVG
	g_CurDev.ports = g_devList[0].ports;
	g_CurDev.ohrTimeout = g_devList[0].ohrTimeout;
#endif
}

//  ������������ � ������ �������� (� ������)
void _SendPacket(void)
{
	unsigned char k;

	for (k = 0; k < PACKET_SIZE; k++)
		((pMas)(&g_readyPacket))[k] = ((pMas)&g_packetReply)[k];

	SendPacket_();
}

//  ���������� �� ����������� ���������� �� �������
void SendPacket_(void)
{
	unsigned char k;

	for (k = 0; k < PACKET_SIZE; k++)
	{
		//delay_loop_2(65530);
		USART_Transmit(((pMas)(&g_readyPacket))[k]);
		((pMas)(&g_lastPacket))[k] = ((pMas)(&g_readyPacket))[k];
	}
}

void SendPacket(PPACKET packet)
{
	unsigned char k;

	packet->marker = PACKET_MARKER;

	for (k = 0; k < PACKET_SIZE; k++)
		((pMas)(&g_readyPacket))[k] = ((pMas)packet)[k];

	link_state.g_linkStatus |= LINK_STATUS_SPECPACKET;
}

void SendCommand(unsigned char adr, unsigned char type)
{
	PACKET packet;

	packet.adr = adr;
	packet.type = type;

	SendPacket(&packet);
}

//  ��������� ������� ������ ��������� -  ����� ������������ ���������
//  �������� ���������� SIG_OVERFLOW3 ���������� � ����������
void HandleProtocol(void)
{
	unsigned char state, devCount = link_state.g_xDevCount;
	unsigned short sensorTimeout;

	if (g_WaitableTimer)
		g_WaitableTimer--;
/*
	if (link_state.g_linkStatus & LINK_STATUS_NOREPLAY)
	{	//  �� ���������� ����� ������ �������� �� ����

		if (link_state.g_tryCount > MAX_TRY_COUNT)
		{	//  �� ������� ��� �����!

			//  ��������� ��� ���������� - ��� ��� ��� ������...
			return;
		}

		link_state.g_tryCount++;
	}
*/
	if (link_state.g_linkStatus & LINK_STATUS_SPECPACKET)
	{	//  ���� � ������� ���� ���������� � ��������
		SendPacket_();
	}
	else
	{	//  �������� ��������� ����� ���������

		if (g_PollEnable)
		{
			g_readyPacket.type		=	CMD_GETSTATUS;

			memset(&g_readyPacket.data, 0x41, 5);

			g_readyPacket.marker 	= 	PACKET_MARKER;

			//  �������� ��������� ����������
			state = g_devList[link_state.g_x232CurPollDev].state;

			if ((state & DEVSTATE_EXIST) && (state & DEVSTATE_CONNECTED))
			{	//  ���� ����������, �������� ��� ����������� ������������ �����
				g_readyPacket.adr = g_devList[link_state.g_x232CurPollDev].adr;

				SendPacket_();
			}

			//  ��������� ���������� ��������� ����������
			if (++link_state.g_x232CurPollDev == devCount)
				link_state.g_x232CurPollDev = 0;
		}
	}

	//  ������ �� ���� - ����� �� ���������� ����� ������ ��� ���������
	link_state.g_linkStatus |= LINK_STATUS_NOREPLAY;

	//  � ������� ������� ������� ���������� ��� - �� ������ ��� ���������
	link_state.g_linkStatus &= ~LINK_STATUS_SPECPACKET;

	//  ���������� ������� �������� ������� �� ������������ ������� ���������� �
	for (state = 0; state < 8; state++)
	{
		//  ���� ������� ��� �� ��������, ��������� ��� �� ����� ������, �������
		//  ��� ���������� ���������� ��-�� ������� ������ ���������
		//  ��� ������� ��-�� ����������� ���������� ��������� ���� ���������
		//  � ����� ����� �����, ������� ��� ������ ������ - ���� ����������
		sensorTimeout = g_devList[g_DecTimeoutDev].sensorTimeout[state];

		if (sensorTimeout >= devCount)
			sensorTimeout = sensorTimeout - devCount;
		else
			sensorTimeout = 0;

		g_devList[g_DecTimeoutDev].sensorTimeout[state] = sensorTimeout;
	}

	//  � ��������� ��� ����� �������� � ������ ����������� (����������� �������)
	if (++g_DecTimeoutDev >= devCount)
		g_DecTimeoutDev = 0;
}

void GetTime(void)
{
	RTC_TIME rtc;

	ReadTime(&rtc);

	g_packetReply.data[0] = rtc.hour;
	g_packetReply.data[1] = rtc.min;
	g_packetReply.data[2] = rtc.day;
	g_packetReply.data[3] = rtc.month;
	g_packetReply.data[4] = rtc.year;
}

void SetTime(void)
{
	RTC_TIME rtc;

	rtc.hour = g_packetReply.data[0];
	rtc.min = g_packetReply.data[1];
	rtc.sec = 0;
	rtc.day = g_packetReply.data[2];
	rtc.month = g_packetReply.data[3];
	rtc.year = g_packetReply.data[4];

	if ((rtc.hour > 0x23) || (rtc.min > 0x59) || (rtc.sec > 0x59) ||
		(rtc.day > 0x31) || (rtc.month > 0x12) || (rtc.year < 0x06))
		return ;

	WriteTime(&rtc);
}

//  �������� ���������� SIG_UART0_RECV
void HandleReply(void)
{
	unsigned char e;
	unsigned char byte = UDR0;

	if (UCSR0A & _BV(FE))
	{
		link_state.g_linkStatus |= LINK_STATUS_FE;
		ClearScreen();
		OutStr("FE");
		e = UDR0;		//  ��� ����, ����� ���������� �����???
		g_packetReplyPos = 0;
		return;
	};
	if (UCSR0A & _BV(UPE))
	{
		link_state.g_linkStatus |= LINK_STATUS_PE;
		ClearScreen();
		OutStr("PE");
		e = UDR0;
		g_packetReplyPos = 0;
		return;
	};
	if (UCSR0A & _BV(DOR))
	{
		link_state.g_linkStatus |= LINK_STATUS_DOR;
		ClearScreen();
		OutStr("DOR");
		e = UDR0;e = UDR0;
		g_packetReplyPos = 0;		//  ������� ���� ������� �����
		return;
	}

	if (g_StreamMode)
	{
		AT45WriteByteToBuffer(g_packetReplyPos, byte);

		g_StreamCurByteCounter++;

		if (512 == g_StreamCurByteCounter)
		{	//  ��� ���, ���������, ��������� ����� ��������...

			g_StreamCurByteCounter = 0;

			AT45WriteBufferToPage(g_packetReplyPos, g_StreamStartPage);
			ClearScreen();
			OutStr(itoa(g_StreamStartPage, (unsigned char*)&g_szInput, 10));

			if (g_packetReplyPos)
				g_packetReplyPos = 0;
			else
				g_packetReplyPos = 1;

			AT45SetWriteBufferPos(g_packetReplyPos, 0);
			g_StreamStartPage++;

			if (g_StreamStartPage == g_StreamEndPage)
			{	//  �������� ���� �����
				g_StreamMode = 0;
				g_packetReplyPos = 0;				//  �� ������ ������
				ClearScreen();
				OutStr("End write");
				g_ShowTime = 1;
			}
		}

		return;
	}

	unsigned char packetReplyPos = g_packetReplyPos;

	//  ���� ���� ������ ������, � ������ ���������-��� - ������ �� �����
	if (!packetReplyPos)
		if (PACKET_MARKER != byte)
			return;

	((pMas)(&g_packetReply))[packetReplyPos] = byte;

	packetReplyPos++;		//  ����� � ������ ��� ���������� �����

	if (PACKET_SIZE == packetReplyPos)
	{	//  ������ ������� �����
		packetReplyPos = 0;

		if (VG_ADR == g_packetReply.adr)
		{	//  ��� "������" ����� ��� ���� -
			HandlePrivatePacket();
		}else
		{
			if ((g_packetReply.adr == g_lastPacket.adr) &&
				(g_packetReply.type == g_lastPacket.type))
			{
				//  ���� ��� ������ �� ��������� ������ - ������������ �������� ���
				link_state.g_linkStatus &= ~LINK_STATUS_NOREPLAY;

				PORTB ^= HF;

				//  ���������� ���� �������� ������ �� GetRespond
				link_state.g_linkStatus &= ~LINK_STATUS_RESPOND;
				link_state.g_tryCount = 0;

				if (CMD_CONNECT == g_packetReply.type)
					//  ��� ����� �� ������ ����������� - ���������� ����������
					g_devList[link_state.g_x232CurPollDev].state |= DEVSTATE_CONNECTED;
				else
				if (CMD_GETSTATUS == g_packetReply.type)
				{
					g_CurDev.adr = g_packetReply.adr;

					//  ��������� ��������� ������ �����, ����� ��������� ������
					//  ���������� ��� ������� - � �� ��� ������ ������...
					if ((g_packetReply.data[0] & _BV(DS_OHRANA)) == _BV(DS_OHRANA))
						SensorsStateChangeNotify(g_packetReply.data[2]);

					g_CurDev.ports_state = g_packetReply.data[2];
				}
			}else
			{
/*
				OutData(g_packetReply.adr);
				OutData(g_lastPacket.adr);
				OutData(g_packetReply.type);
				OutData(g_lastPacket.type);
*/			}
		}
	}

	g_packetReplyPos = packetReplyPos;

	return;
}

// ������������ � ���������� � ���� ������
char ConnectToDevice(void)
{
	PACKET packet;

	packet.marker	=	0xA5;
	packet.type		=	CMD_CONNECT;
	packet.adr		=	g_CurDev.adr;
	packet.data[0]	=	0;

	return GetRespond(&packet);
}

void SendAll(unsigned char cmd)
{
	PACKET packet;
	unsigned char k;

	packet.marker	=	0xA5;
	packet.type		=	cmd;
	packet.data[0]	=	0;
	packet.data[1]	=	1;

	for (k = 0; k < link_state.g_xDevCount; k++)
	{
		packet.adr = k;
		SendPacket(&packet);

		delay_loop_2(6553);
	}
}

void SwitchDeviceOhrana(void)
{
	SendCommand(g_CurDev.adr, CMD_SETOHRANA);
}

char SetOhranaState(unsigned char num, unsigned char action)
{
	PACKET packet;

	packet.marker	=	0xA5;
	packet.type		=	CMD_SETOHRANA;
	packet.adr		=	num;
	packet.data[0]	=	action;

	return GetRespond(&packet);
}

char GetCurDeviceInfo()
{
#ifndef NEWVG
	char res;
#endif
	PACKET packet;

	packet.adr = g_CurDev.adr;
	packet.type = CMD_GETSTATUS;

#ifndef NEWVG
	//  �������� - ������ � ��������� ������
	res = GetRespond(&packet);

	//  ���� ���������� ������ �������� �� EEPROM ��������, � �� �� ���������
	//g_CurDev.ports = g_packetReply.data[0];
	g_CurDev.ports = g_devList[packet.adr - 3].ports;

	g_CurDev.ohrTimeout = g_packetReply.data[3];

	return res;
#else
	//  �������� - ������ � ��������� ������
	g_CurDev.ports = g_devList[0].ports;
	g_CurDev.ohrTimeout = g_devList[0].ohrTimeout;

	return 1/*res*/;
#endif
}

void ChangeDelayTimeBeforeSetOhrana(unsigned short timeout)
{
	PACKET packet;

	packet.adr = g_CurDev.adr;
	packet.type = CMD_SETOHRTIMEOUT;
	*(unsigned short*)&packet.data[0] = timeout;

#ifndef NEWVG
	SendPacket(&packet);
#endif

#ifdef NEWVG
	//  �������� - � ���� ������ � ���������� ����������� � ��� ����
	SetDefBuiltinTimeout(timeout);

#endif
}

void ChangePortPurpose(void)
{
	PACKET packet;

	packet.adr = g_CurDev.adr;
	packet.type = CMD_SETPORT;
	packet.data[0] = g_CurDev.ports;

#ifdef NEWVG
	packet.adr = 3;		//  ���������� ������
#endif

	g_devList[packet.adr - 3].ports = g_CurDev.ports;

	unsigned short ge = (EADR_DEVICE_LIST + (packet.adr - 3)*sizeof(XDEVICE) + 2);

	eeprom_write_byte_1((unsigned char *)ge, g_CurDev.ports);

#ifndef NEWVG
	SendPacket(&packet);
#endif
}

char GetRespond(PPACKET packet)
{
	//  ����� ���� ���� ������� ������!!!!!!!!
	link_state.g_linkStatus |= LINK_STATUS_RESPOND;

	SendPacket(packet);

	//  ��������� �����

	g_WaitableTimer = 5;

#ifdef PROTEUS
	g_WaitableTimer = 1;
#endif

	while (link_state.g_linkStatus & LINK_STATUS_RESPOND)
	{
		//  ���� �������� ����� ������� - WAIT_FOR_RESPOND

		if (!g_WaitableTimer)
			return 0;
	}

	//  � ����� ������� LINK_STATUS_RESPOND ��� ������� (��� ������ ���� �������)

	return 1;
}

//  ���������� �������� ������� � �������� ������ ��� ��������� �����
char GetSensorThreshold(unsigned char index)
{
	char res;
	PACKET packet;

	packet.adr = g_CurDev.adr;
	packet.type = CMD_GETSENSORSENSITIVITY;
	packet.data[0] = index;

	res = GetRespond(&packet);

	g_CurDev.threshold1 = 70;
	g_CurDev.threshold2 = 700;

	if (res)
	{
		g_CurDev.threshold1 = *(unsigned short*)&g_packetReply.data[1];
		g_CurDev.threshold2 = *(unsigned short*)&g_packetReply.data[3];
	}

#ifdef PROTEUS
	return 1;
#endif

	return res;
}

char ChangeSensorThreshold(unsigned char index)
{
	char res;
	PACKET packet;

	packet.adr = g_CurDev.adr;
	packet.type = CMD_SETSENSORSENSITIVITY;
	packet.data[0] = index;
	*(unsigned short*)&packet.data[1] = g_CurDev.threshold1;
	*(unsigned short*)&packet.data[3] = g_CurDev.threshold2;

	res = GetRespond(&packet);

	if (res)
	{
	}

	return res;
}

void ChangeX232(unsigned char* valueType, unsigned char value)
{
	eeprom_write_byte_1((unsigned char*)valueType, value);
	if ((unsigned char*)EADR_X232_TIMEOUT == valueType)
		g_x232TimeOut = value;
	else
		g_x232Interval = value;
}

void HandlePrivatePacket(void)
{
	unsigned short e;
	unsigned char k, byte, bufno, send_all;
	byte = g_packetReply.data[0];

	if (CMD_CONNECT == g_packetReply.type)
	{
		devStatus |= _BV(DS_CONNECTED);
		if (111 != g_packetReply.data[0])
			g_packetReply.data[0] = 0;
	}else
		if (devStatus & _BV(DS_CONNECTED))
			//  ���� �� ������������
			switch (g_packetReply.type)
			{
				case CMD_CONNECT:
					devStatus |= _BV(DS_CONNECTED);
					g_packetReply.data[0] = 0;
					break;
				case CMD_GETSTATUS:
					break;
				case CMD_SETPORT:
					break;
				case CMD_OUTPORT:
					break;
				case CMD_SETADR:
					break;
				//case CMD_SHOWCLOCK:
				case CMD_SETOHRANA:
					g_ShowTime = !g_ShowTime;
					break;
				case CMD_DISCONNECT:
					devStatus &= ~_BV(DS_CONNECTED);
					break;
				case CMD_HIDECLOCK:
					break;
				case CMD_CLEARSCREEN:
					ClearScreen();
					break;
				case CMD_WRITEDIGIT:
					OutData(0x30 + g_packetReply.data[0]);
					break;
				case CMD_FINDDEV:
					break;
				case CMD_SETSENSORSENSITIVITY:
					break;
				case CMD_GETSENSORVALUE:
					g_packetReply.data[0] = AT45ReadStatus();
					break;
				case CMD_ADC_ENABLE:
					break;
				case CMD_SETOHRTIMEOUT:
					break;
				case CMD_GETEEPROM:
					e = *(uint16_t *)(&g_packetReply.data[0]);
					eeprom_read_block((uint16_t *)&(g_packetReply.data[2]),
						(uint16_t *)e, EEPROM_SEND_SIZE);
					break;
				case CMD_GETRAM:
					e = *(uint16_t *)(&g_packetReply.data[0]);
					*(uint16_t *)(&g_packetReply.data[11]) = SP;

//					if (0xFFF > e)
						for (byte = 0; byte < 8; byte++)
							g_packetReply.data[byte + 2] = *(uint8_t *)(e + byte);
//					else
//					{
//						for (byte = 0; byte < 8; byte++)
//							g_packetReply.data[byte + 2] = *(uint8_t *)(e - byte);
//					}
					break;
				case CMD_SETTIME:
					SetTime();
					break;
				case CMD_GETTIME:
					GetTime();
					break;
				case CMD_AT45:
					switch (g_packetReply.data[0])
					{	//  ������ 8 ���� ������ ������, ������� ��
						//  ����� ����� g_packetReply.data[0]
						case CMD_AT45_READ_BUFFER:
						    bufno = g_packetReply.data[4];
							send_all = g_packetReply.data[3];
							g_packetReply.data[3] = bufno;	//  ��� XDevMon'a

							//  ������������� ������� � ������, � ���. ������
							AT45SetReadBufferPos(bufno,
								*(uint16_t*)&g_packetReply.data[1]);

							if (!send_all)
								for (byte = 0; byte < 8; byte++)
									g_packetReply.data[byte + 4] =
										AT45ReadByteFromBuffer(bufno);
							else
							{
	for (byte = 0; byte < PACKET_SIZE; byte++)
		((pMas)(&g_readyPacket))[byte] = ((pMas)&g_packetReply)[byte];

								AT45SetReadBufferPos(0, 0);
								for (k = 0; k < 64; k++)
								{
									for (byte = 0; byte < 8; byte++)
										g_readyPacket.data[byte + 4] =
											AT45ReadByteFromBuffer(bufno);

									SendPacket_();
								}
							}

							break;
						case CMD_AT45_READ_PAGE_TO_BUF:
							AT45ReadPageToBuffer(0,
								*(uint16_t*)&g_packetReply.data[1]);
							break;
						case CMD_AT45_WRITE_BUFFER:
							//  ������������� ������� � ������, � ���. ������
							AT45SetWriteBufferPos(0,
								*(uint16_t*)&g_packetReply.data[1]);

							AT45WriteByteToBuffer(0, g_packetReply.data[3]);
							break;
						case CMD_AT45_WRITE_BUF_TO_PAGE:
							//  �������� ����� � �������� �����
							AT45WriteBufferToPage(0,
								*(uint16_t*)&g_packetReply.data[1]);
						case CMD_AT45_SET_READ_POS:
							AT45SetReadBufferPos(0,
								*(uint16_t*)&g_packetReply.data[1]);
							break;
						case CMD_AT45_SET_WRITE_POS:
							AT45SetWriteBufferPos(0,
								*(uint16_t*)&g_packetReply.data[1]);
							break;
						case CMD_AT45_FILL_BUF:
							AT45FillBuffer(0, g_packetReply.data[1]);

							break;
						case CMD_AT45_READ_BYTE:
							g_packetReply.data[0] =
								AT45ReadByteFromPage(
									*(uint16_t*)&g_packetReply.data[1],
									*(uint16_t*)&g_packetReply.data[3]);
							break;
						case CMD_AT45_SEND_OKTET:
							for (k = 1; k < 9; k++)
								AT45WriteByteToBuffer(0, g_packetReply.data[k]);

	for (byte = 0; byte < PACKET_SIZE; byte++)
		((pMas)(&g_readyPacket))[byte] = ((pMas)&g_packetReply)[byte];

								//  ����� ��� ��������, ��������� �� �� ������
								SendPacket_();		//  ����������� �����

							return;
						case CMD_AT45_ERASE_PAGES:
							e = *(uint16_t*)&g_packetReply.data[1];
/* �������� ����������*/	g_StreamEndPage = *(uint16_t*)&g_packetReply.data[3];
							for (k = e; k < g_StreamEndPage; k++)
								AT45PageErase(k);
							break;
					}
					break;
				case CMD_FATX:
					if (CMD_FATX_FILE_LIST == byte)
					{
						//  ���������� ���-�� ������, � ��� ������� �� ���
						//  ������ �������������� �������
						for (k = 0; k < 100; k++)
						{
							//  �������� ������ �������� �����
							e = fatxFileExists(k);
							if (e)
							{	//  ���� ���� ����

								*(uint16_t*)&g_packetReply.data[1] = k;
								// �������� ����� �����
								*(uint16_t*)&g_packetReply.data[5] =
									rootdir[(k << 1) + 1];

								while (e != 0xFFFF)
								{
									//  �������� ����. �������� �����
									*(uint16_t*)&g_packetReply.data[3] = e;
									e = fatxGetNextPage(e);
									_SendPacket();
								}
							}
						}
						return;
					}else
					if (CMD_FATX_CREATE_FILE == byte)
							fatxCreateFile();
					else if (CMD_FATX_DELETE_FILE == byte)
							fatxDeleteFile(g_packetReply.data[1]);
					else if (CMD_FATX_ADD_PAGE == byte)
							fatxAddPageToFile(g_packetReply.data[1],
								*(uint16_t*)&g_packetReply.data[3]);
					else if (CMD_FATX_FORMAT == byte)
							fatxFormat();
					else if (CMD_FATX_GET_FILE_PAGES == byte);
					else if (CMD_FATX_WRITE_PAGE == byte);
					else if (CMD_FATX_OPEN_FILE == byte)
							g_packetReply.data[1] =
								fatxOpenFile(g_packetReply.data[1],
									g_packetReply.data[2]);
					else if (CMD_FATX_READ_PAGE == byte)
							g_packetReply.data[1] =
								fatxReadByteFromFile(&g_packetReply.data[2]);
					else if (CMD_FATX_SEEK == byte)
							g_packetReply.data[1] =
								fatxFileSeek(g_packetReply.data[1]);
					else if (CMD_FATX_READ_BYTE == byte)
							g_packetReply.data[1] =
								fatxReadByteFromFile(&g_packetReply.data[2]);
					else if (CMD_FATX_WRITE_BYTE == byte)
							g_packetReply.data[1] =
								fatxWriteByteToFile(g_packetReply.data[1]);
					else if (CMD_FATX_FLUSH_FILE == byte)
							fatxFlushFile();
					else if (CMD_CLOSE_FILE == byte)
							fatxCloseFile();
				case CMD_TEL:
					if (!g_packetReply.data[0])		//   CMD_TEL_BUTTON
					{
						AddKey(g_packetReply.data[1]);
					};
					break;
				case CMD_SPEAKERVOLUME:
					//SetSpeakerVolume(g_packetReply.data[0]);
					break;
				case CMD_GSM:
					if (!byte)
						PORTD &= ~PWR;
					else if (1 == byte)
							PORTD |= PWR;
					else if (byte == 2)
					{	// �������� ��������� ����� ������ �� ������ -
						g_StreamMode = 1;
						g_ShowTime = 0;
						//  ���� ���������� ������
						g_StreamStartPage = *(uint16_t*)&g_packetReply.data[1];
						//  ������� ������� � ������� ������ ��������� � ���� ������
						g_StreamEndPage = g_StreamStartPage +
							*(uint16_t*)&g_packetReply.data[3];
						g_StreamCurByteCounter = 0;
						g_packetReplyPos = 0;		//  � ���� ������ - �������� �����
						AT45SetWriteBufferPos(0, 0);
						ClearScreen();
					}
					else if (byte == 3)
							InitPWM();
					else if (byte == 4)
							TCCR1B = 0;
					else if (byte == 5)
							OCR1B = g_packetReply.data[1];
					else if (byte == 6)
						 {
							*(uint16_t*)&g_packetReply.data[1] = a2dConvert10bit(1);
							g_packetReply.data[3] = 0;
							g_packetReply.data[4] = a2dConvert8bit(1);
						 }
					else if (byte == 7)
							StartLineAnalize(g_packetReply.data[1]);
							//*(uint16_t*)&g_packetReply.data[1] = SP;
					else if (byte == 8)
							g_params[g_packetReply.data[1]] =
								*(uint16_t*)&g_packetReply.data[2];
					else if (byte == 9)
							//  ��������� ������ ����� � ����������� ������
							//  ����� �������������� ���-������
							*((unsigned char*)*(unsigned short*)(&g_packetReply.data[2]))
								= g_packetReply.data[1];
					else if (byte == 10)
					{
						GSM_Transmit(g_packetReply.data[1]);
						return;
					}

				case CMD_ZVUK:
					if (!byte)
							//  ���������� ��������� �������� ��� �����
							//zvPage = *(uint16_t*)&g_packetReply.data[1];
							setstart(*(uint16_t*)&g_packetReply.data[1]);
					else
					if (1 == byte)
						SendVoice(*(uint16_t*)&g_packetReply.data[1], 1);
			}

	for (byte = 0; byte < PACKET_SIZE; byte++)
		((pMas)(&g_readyPacket))[byte] = ((pMas)&g_packetReply)[byte];

	SendPacket_();		//  ����������� �����
}

void SendManualPacket(unsigned char* sz)
{
	unsigned char k, x;
	unsigned char buf[3];

	x = 1;

	for (k = 0; k < MAX_INPUT; k += 3)
	{
		buf[0] = sz[k];
		buf[1] = sz[k + 1];
		buf[2] = sz[k + 2];

		sz[x] = atoi(buf);
		x++;
	}

	for (k = x; k < PACKET_SIZE; k++)
		sz[k] = 0;

	SendPacket((PPACKET)sz);
}

void EnablePoll(unsigned char pollValue)
{
	g_PollEnable = pollValue;
}

void ChangeSensorReactionTimeout(unsigned short newValue)
{
	unsigned short adr;

#ifndef NEWVG
	adr = g_CurDev.adr - 3;
#else
	adr = 0;
#endif

	adr = EADR_DEVICE_LIST + (0)*sizeof(XDEVICE) + 3 +
		(g_CurDev.curSensor << 1);

//	g_devList[g_CurDev.adr - 3].sensorTimeout[g_CurDev.curSensor] = newValue;
	g_CurDev.sensorTimeout[g_CurDev.curSensor] = newValue;

//	eeprom_write_word((unsigned char*)adr, newValue);
	eeprom_write_byte_1((unsigned char*)adr, newValue & 0x00FF);
	eeprom_write_byte_1((unsigned char*)adr + 1, newValue >> 8);
}

void SetDefBuiltinTimeout(unsigned short timeout)
{
	eeprom_write_word((unsigned short*)(EADR_DEVICE_LIST + 19), timeout);
	g_devList[0].ohrTimeout = timeout;
	g_CurDev.ohrTimeout = timeout;
}
