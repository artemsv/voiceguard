//3322658
//  27.09.2006

#include <stdlib.h>
#include <string.h>

#include <interrupt.h>
#include "stdafx.h"
#include "a2d.h"
#include "kbd.h"
#include "lcd.h"
#include "tim.h"
#include "spi.h"
#include "uart.h"
#include "utils.h"
#include "vg.h"
#include "XDevMon.h"
#include "at45.h"
#include "fatx.h"
#include "utils.h"

unsigned char* esp;

#include "BuildNo.inc"
/*
3156619		��� ����� ��������

1047669		����-���
1249494		�����-����
1543790		������-���
1835055		���� ����������
2217801		Vligor
3028877		�����-���
3036703		�������-���
3052553		�������-���

3062536		�������
3310582		�����-���
3355574		������-���

1600158		����-��� - ���� ������������
3344637		�� �����

3622128		�����	��� 3 �� 3

���� - 24 �������
4059961		����-���
4062609		����-���

3681450		������� ���������� ���������(�������)
2970112		JK
������		15.03.1987

������ �����, ��������� ����-���
180-57-88	�����
150-16-47	�������

191-93-23	������� �.
222-08-92	����� ����� ���
223-89-93
*/

#include "DTMF.h"

//  ���������������� � ������������ ������
//#define DEBUG

#define HFON		PORTB |= HF
#define HFOFF	PORTB &= ~HF

unsigned char	g_szSetupPassword[MAX_PASSWORD_LEN + 1];

//  ����� ������, � ������� ������� ��������� � ������ ������
unsigned char	g_Mode;

//  � ��������� ������� ��� ������ ������ � ����� �������� ��������� �����
//  � ������, ���� g_checkedItem != FF, �� ����� �� ����� �������
unsigned char	g_checkedItem = 1;

unsigned char	g_ShowTime;		//  ���������� �� ������ ������ �����
unsigned char 	g_canShowTime;	//  � ������, ��������� �� �����

unsigned char	g_numbers[] = {'A', '1', '2', '3', 'B', '4', '5', '6', 'C',
								'7', '8', '9', 'D', 'E', '0', DIAL_PAUSE, 'F',
								'1', '2', '3'};
unsigned char* 	g_szSecretPassword = "1188564";

//  ����� � ��������� � ���������� �������
unsigned char	g_szInput[MAX_INPUT + 1] =
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned char	g_InputPos = 0;	//  ����. ��������� ������� � ������ g_szInput

#define MAX_MODE_ITEMS	33

const unsigned char*	g_szModes[MAX_MODE_ITEMS] =
			{
				//"\x54\x65\xBB\x65\xE4\x6F\xBD",	//�������

				//  �������� �����
				"\x48\x61\xB2\x65\x70\xB8\xBF\x65\x20\xBD\x6F\xBC\x65\x70",
				//"������� �����",
				"\x42\xB3\x65\xE3\xB8\xBF\x65\x20\xB3\x70\x65\xBC\xC7",
				//������� ������
				"\x42\xB3\x65\xE3\xB8\xBF\x65\x20\xBE\x61\x70\x6F\xBB\xC2",
				"VoiceGuard 1.0",

				//��� �������
				"\xA8\x6F\xE3\x20\x6F\x78\x70\x61\xBD\x6F\xB9",
				"\x48\x6F\xB3\xC3\xB9\x20\xBE\x61\x70\x6F\xBB\xC2",//����� ������",
				//�������
				"\x54\x65\xBA\x79\xE6\x65\x65 (",
				"\xA4\x61\xE3\x65\x70\xB6\xBA\x61 (",
				"1 2 3 4 5 6 7 8",
				"\x48\xB8\xB6\xBD\xB8\xA6\x20\x42\x65\x70\x78\xBD\xB8\xA6",
				"",
				"",
				"",
				"",
				"",
				"",
				"",
				"",
				"",
				"",
				"",
				"",
				"",
				"",
				"Packet",
				"",
				"\x42\xB3\x65\xE3\xB8\xBF\x65\x20\xBD\x6F\xBC\x65\x70",
				"",
/*28*/			"\x41\xE3\x70\x65\x63\x20\x20\xA4\xBD\x61\xC0\x65\xBD\xB8\x65",
				"\x41\xE3\x70\x65\x63\x20\x20\xA4\xBD\x61\xC0\x65\xBD\xB8\x65",	//29
				"",
				"\xA8\x6F\xB3\xBF\x6F\x70\x20\x63\x70\x61\xB2\x61\xBF",
/*32*/			""
			};

//  ������ �������� ���� Setup
unsigned char*	g_szSetupItems[MAX_MAIN_SETUP_ITEMS] = {
								"\xE0\x61\xBF\xC0\xB8\xBA\xB8", //  �������
//								"GSM",
								"\x42\x70\x65\xBC\xC7",		//  �����
								"\x54\x65\xBB\x65\xE4\x6F\xBD",	//  �������
								"\xA1\x70\x6F\xBC\xBA\x6F\x63\xBF\xC2",	//���������
								"\xA8\x70\x6F\xBF\x6F\xBA\x6F\xBB",
#ifndef NEWVG
								"\xA4\xB3\x79\xBA",			//  ����
#endif
								"\x48\x61\x63\xBF\x70\x6F\xB9\xBA\x61"	//  ���������

								};

//  ������ ������� Sensors �������� ���� Setup
unsigned char*	g_szSensorsItems[MAX_SENSORS_ITEMS] = {
								//  ������
								"\x43\xBE\xB8\x63\x6F\xBA",
#ifndef NEWVG
								//  �����
								"\xA8\x6F\xB8\x63\xBA",
								//  ��������
#endif
								"\x43\xBA\x6F\x70\x6F\x63\xBF\xC2"
#ifndef NEWVG

								//  �����
								"\x4F\xBE\x70\x6F\x63"
#endif
								};

unsigned char* g_szDeviceItems[MAX_DEVICE_ITEMS] = {
								"\xA8\x6F\x70\xBF\xC3",		//  �����
								"\x42\x78\x6F\xE3\xC3",		//  �����
								//  ��������
								"\xA4\x61\xE3\x65\x70\xB6\xBA\x61",
								"\xA8\xB8\xBF\x61\xBD\xB8\x65"//  �������
								};

unsigned char* g_szDeviceSensorItems[MAX_SENSOR_COUNT] = {
								"\xA9\x70\x6F\xB3\xBD\xB8",
								"\x54\x61\xB9\xBC\x61\x79\xBF"
								};

//  ������ ������� Telefon �������� ���� Setup
unsigned char*	g_szTelefonItems[MAX_TELEFON_ITEMS] = {
								//  ������
								"\x48\x6F\xBC\x65\x70\x61",
								//  ���
								"\x41\x6F\xBD",
								//  ����� �����
								"\x50\x65\xB6\xB8\xBC\x20\xBB\xB8\xBD\xB8\xB8",
								//  ������������
								"\x4F\xBE\x70\x65\xE3\x65\xBB\x65\xBD\xB8\x65"
								};

unsigned char*	g_szVolumeItems[MAX_VOLUME_ITEMS] = {
								"\xE0\xB8\xBD\x61\xBC\xB8\xBA", //�������
								"\x54\x65\xBB\x65\xE4\x6F\xBD" //�������
								};

//  ������ ������� Line mode ���� Telefon
unsigned char*	g_szLinemodeItems[MAX_TEL_LINEMODE_ITEMS] = {
								//  ����������
								"\xA5\xBc\xBe\x79\xBB\xC2\x63\xBD\xC3\xA6",
								//  �������
								"\x54\x6F\xBD\x6F\xB3\xC3\xA6"
								};

unsigned char* g_szX232Items[MAX_X232_ITEMS] = {
								"\x54\x61\xB9\xBC\x61\x79\xBF",			//  TimeOut
								"\xA5\xBD\xBF\x65\x70\xB3\x61\xBB"
#ifndef NEWVG
								,		//  Interval
								"\xA8\x61\xBA\x65\xBF"					//  �����
#endif
								};

unsigned char* g_szSettingsItems[MAX_SETTINGS_ITEMS] = {
								"RAM",
								"EEPROM",
								"Version",
								"\xA8\x61\x70\x6F\xBB\xC2",	  //  ������
								"\x43\xB2\x70\x6F\x63"

								};

#define MAX_INPUT_LENS_COUNT	33

unsigned char	g_InputLens[MAX_INPUT_LENS_COUNT] = {
									INPUT_LEN_TEL + 1, 	// ������� ����� ��� ��������,
									INPUT_LEN_TIME, // ���� g_InputPos ����������
									INPUT_LEN_PSWD + 1,	// � ����
									0,
									0,
									INPUT_LEN_PSWD + 1,
									INPUT_LEN_BAUDRATE + 1,
									INPUT_LEN_TIMEOUT + 1,
									0,
									0,
									0,
									0,
									0,
									0,
									0,
									0,
									0,
									0,
									0,
									0,
									0,
									0,
									0,
									0,
									0,
									0,
									INPUT_LEN_TEL + 1,
									0,
									14,			//	MD_SETUP_SETTINGS_RAM
									14,			//	MD_SETUP_SETTINGS_EEPROM
									0,
									3,	//MD_SETUP_SENSORS_LIST_DEVICE_SENSOR_TIMEOUT1
									INPUT_LEN_TEL + 1,
								};

unsigned char*  g_szAutoDialNumbers[] =
								{
									"105\x0            ",
									"100\x0            ",
									"-3\x0             ",
									"-4\x0             ",
									"-5\x0             ",
									"-6\x0             "
								};

unsigned char*	g_linemodes[6] =
								{
									//  �����������
									"\x4F\xBE\x70\x65\xE3\x65\xBB\x65\xBD\xB8\x65",
									//  �����
									"\x42\xC3\xB7\x6F\xB3",
									//  ������
									"\xA4\x61\xBD\xC7\xBF\x6F",
									//  �� ��� �������
									"\x4F\x43\x20\xB8\xBB\xB8\x20\xBE\x6F\xBB\x6F\xB6\xB8\xBB",
									//  ������
									"\xA8\x6F\xE3\xBD\xC7\xBB",
									//  ��������
									"\x42\xc3\xBA\xBB\xC6\xC0\x65\xBD"
								};

unsigned char 	g_LastKey;
unsigned char	g_SetupCurItem = 0;
unsigned char	g_SetupFirstLine = 0;
unsigned char	g_SetupItem;
unsigned char	g_CurMenuMaxItems;		//  ���������� ������� �������� ����
unsigned char	g_curBarValue;			//  �������
unsigned char	g_telon;				//  1 - ������� ���������� ������
unsigned char 	g_hf;					//  1 - ����� HandsFree �������
unsigned char 	g_speakerVolume;		//  ������� ��������� ��������
unsigned char	g_szLastNumber[MAX_INPUT + 1];		//  ��������� ��������� �����
unsigned char 	g_bSilent;				//  1 - �� �������� � ����� ���������� �����

unsigned char 	g_GalkaPunkt;			//  ����� �������� ���� � ������
unsigned char 	g_Galka;				//  ��� ����� � ����� ������
unsigned char	g_needHello;			//  �������� �����������

//  �������������� ����������
extern unsigned char g_CaretPos;		//  ��������� �������
extern CUR_DEVICE g_CurDev;
extern XDEVICE g_devList[MAX_DEVICE_COUNT];
extern unsigned short wBufPos[2];

//  ������
#define szPoisk			"\xA8\x6F\xB8\x63\xBA"
#define szZvonok		"\xA4\xB3\x6f\xBD\x6F\xBA"
#define szTrevoga		"\x54\x70\x65\xB3\x6F\xB4\x61"
#define szNetOtveta		"\x48\x65\xBF\x20\x6F\xBF\xB3\x65\xBF\x61"
#define szUstr			"\xA9\x63\xBF\x70 - "
#define szVhod			"\x42\x78\x6F\xE3\x20"
#define szStrelka		"\x3C---"
#define szLocalDTMF_YES	"LocalDTMF - YES"
#define sz_est			" - \x65\x63\xBF\xC4"
#define szUstrPodOhr	"\xA9\x63\xBF\x70\x20\xBE\x6F\xE3\x20\x6F\x78\x70\x61\xBD\x6F\xB9"
#define sz_net			" - \xBD\x65\xBF"
//#define szKPVNextNumber "KPV Next number"
/*
const unsigned char SineTable[256] = {
							 133,136,139,142,
                             145,149,152,155,
                             158,161,164,167,
                             169,172,175,178,
                             181,184,186,189,
                             192,194,197,200,
                             202,205,207,209,
                             212,214,216,218,
                             220,222,224,226,
                             228,230,232,233,
                             235,237,238,240,
                             241,242,243,245,
                             246,247,248,248,
                             249,250,251,251,
                             252,252,252,253,
                             253,253,253,253,
                             253,253,252,252,
                             252,251,251,250,
                             249,248,248,247,
                             246,245,243,242,
                             241,240,238,237,
                             235,233,232,230,
                             228,226,224,222,
                             220,218,216,214,
                             212,209,207,205,
                             202,200,197,194,
                             192,189,186,184,
                             181,178,175,172,
                             169,167,164,161,
                             158,155,152,149,
                             145,142,139,136,
                             133,130,127,124,
                             121,118,115,112,
                             109,105,102,99,
                             96,93,90,87,
                             85,82,79,76,
                             73,70,68,65,
                             62,60,57,54,
                             52,49,47,45,
                             42,40,38,36,
                             34,32,30,28,
                             26,24,22,21,
                             19,17,16,14,
                             13,12,11,9,
                             8,7,6,6,
                             5,4,3,3,
                             2,2,2,1,
                             1,1,1,1,
                             1,1,2,2,
                             2,3,3,4,
                             5,6,6,7,
                             8,9,11,12,
                             13,14,16,17,
                             19,21,22,24,
                             26,28,30,32,
                             34,36,38,40,
                             42,45,47,49,
                             52,54,57,60,
                             62,65,68,70,
                             73,76,79,82,
                             85,87,90,93,
                             96,99,102,105,
                             109,112,115,118,
                             121,124,127,127
							 };

*/
unsigned char tmp;
unsigned char record = 0;
unsigned char** pMasItems;	//  ����� ������� � ��������
extern unsigned char g_x232Interval;
extern unsigned char g_StreamMode;
unsigned char BaudRates[7] = {3, 6, 12, 24, 48, 72, 144};

unsigned char ring = 0;				//  1 - ������ ������

unsigned short zvukpos = 0;			//  ������� � ������ ��� ���������� ����� �����
unsigned short zvukpage;			//  ����� ������� ��������
unsigned char activeBuf = 0;

unsigned char modeBeforeZvonok;		//  ����� ����� ��� �� ����� ������?

unsigned char analizeTELIN;
unsigned char telinDivider;
unsigned char linemode;
unsigned char lastLinemode;
unsigned char g_sendOhranaEventMsgInProgress;	// 	1 - ���������� ����������
unsigned char g_DefferedWork;					//  ������ ���  - �����-�� ������
unsigned char g_DefferedSensorsState;			//  ����� ��������� ��������
unsigned char g_DefferedSensorsState_;			//  ��������� ���������
unsigned char g_DefferStage = 0;				//  ������ ������� ���������� �������
unsigned short defferedOhranaEvent;
unsigned char g_LineModeAnalizeResultToSPI;

//  ������� ���������� - ����������
#define PARAM_TELIN_ANALIZE_RESOLUTION	0
#define PARAM_XXXXXXXXXXXXXX			1
#define PARAM_XXXXXXX					2
#define PARAM_GUDOK_LEN					3
#define PARAM_SAME_INTERVAL_CNT			4
#define PARAM_MAXPAUSE_LEN				5
#define PARAM_OS_WAIT_TIME				6
#define PARAM_FIRST_GUDOK_WAIT_TIME		7
#define PARAM_TCNT1						8

unsigned short g_params[PARAMS_COUNT] =
							{	1,				//  PARAM_TELIN_ANALIZE_RESOLUTION
								0,				//  PARAM_XXXXXXXXXXXXXX
								0,				//  PARAM_XXXXXXX
								100,			//	PARAM_GUDOK_LEN
								4,				//	PARAM_SAME_INTERVAL_CNT
								200,			//	PARAM_MAXPAUSE_LEN
								10,				//	PARAM_OS_WAIT_TIME
								10,				//	PARAM_FIRST_GUDOK_WAIT_TIME
								0x7FFF			//  TCNT1
							};

//  ���-�� ������� ���������� �����
#define LMMAXCOUNT		6
//  ������ ���������� �����
#define LM_KPV			1
#define LM_BUSY			2
#define LM_OS_OR_DOWN	3
#define LM_UP			4
#define LM_FAULT		5
#define LM_OS_ABSENT	6

//  ���������, ������������ WaitXXX ���������
#define WR_LM			1
#define WR_TIMEOUT		2

/*	����������� DTMF ��������	*/

#define DTMF_CMD_BUF_LEN	10					//  ����� ������ ������ DTMF

//  ������ DTMF
#define DTMF_MODE_NORMAL						0x00
#define DTMF_MODE_PSWD_CHANGE					0x01
#define DTMF_MODE_AUTO_DIAL_NUMBER_CHANGE		0x02
#define DTMF_MODE_SET_TIME						0x03

#define	DTMF_PASSWORD_LEN						10

#ifdef REDUCE
	unsigned char g_lastDTMFCod;
	unsigned char g_dtmfPasswordPermission;
	unsigned char g_DTMFEnterPressCounter;
#endif

unsigned char g_dtmfCmdBuf[DTMF_CMD_BUF_LEN];
unsigned char g_dtmfCmdBufLast[DTMF_CMD_BUF_LEN];
unsigned char g_dtmfCmdBufPos = 0;
unsigned char g_dtmfCmdBufPosLast;
unsigned char g_dtmfMode;
unsigned char g_dtmfTmp;			//  ��������� ����������, ���. ��� DTMF ������
unsigned char g_dtmfPSWOk;			//  1 - � ������� ������ ������ ������ ���������
unsigned char useLocalDTMF;		//	1 - ����� ������������ �� ��������� DTMF-�����
unsigned char g_wasRequest;		//  ��������� �������� ���� ��������� (������ "1")
unsigned char g_wasConfirm;		//  ����������� ���� ������������(������ "2")

unsigned short	g_sampleCount = 0;
unsigned char g_fallCounter;	//  ���������� ������ �� 100-��������� ����������
unsigned short g_sameIntervalCounter;	//  ������� ���������� 100-��������� ����������
unsigned char g_prevIntervalType;		//  ���� ����������� ���������
signed char   g_sampleDiff;				//  �������� ��������� ���� �������
unsigned char voicePresent;
//  ��� �� ��������� �������� � ��������� ���������(����� ������� ������ ���������)?
unsigned char g_wasOne;
unsigned short g_IntervalCount;
unsigned short g_lastIntervalCount;
unsigned char adcCur = 0;
unsigned char adc_1 = 0;
unsigned char g_oldSampleDiff;
unsigned char g_secCnt = 0;
unsigned char confirmUp = 0;
unsigned char dontStartConfirmTimer;
unsigned char advancedLineMode;
unsigned char g_wasEnteringCall;		//  ��� �������� ������

unsigned char g_cancelPressCount;		//  ������� ����������� ������� Escape
unsigned char g_oldSetupFirstLine;		//  ����� ����� �������� ��������� ��� Escape
unsigned char g_oldSetupCurItem;		//  ���� ��������� �������
unsigned char helloCounter;
unsigned char progress;
unsigned char progressPos;

unsigned char timer0delay;
//  ���-�� ������� ��������� ���� � ���-�� �����, ������� ����� ������ � �� ���� "1"
//  (��� �� �����, �� ���� ��������������)
unsigned char g_attemptCounterTheSameNumber;
//  true - �������� ����� ����
//unsigned char mc14lc;

#define MAX_VOICE_QUEUE	30 + 2
//  ����� ��������� ������� ��� �����.
unsigned char g_nextVoice;
//  ������� ������ �� ������������
unsigned short g_VoiceQueue[MAX_VOICE_QUEUE];
/*
unsigned char g_genDTMF;
unsigned short g_cntDTMF;
*/

/// Passiv module declarations

//  ����� �������, ������� ��������� ����� ������������� � ���

#define MAX_ADC_PORT_COUNT					8
//  �������� ������ ���������� ��������
#define BUILTIN_SENSORS_POLL_INTERVAL_		20

unsigned char psvSensor_num;
unsigned char sensors;
unsigned char builtinSensorsPollInterval;		//  �������� ������ ����������
double sensorsTopLimit;
double sensorsBottomLimit;
unsigned short sensorsTopLimit2;
unsigned short sensorsBottomLimit2;

//  ������ ���������������� ��� ���������� ��������
unsigned short dacThresholdLOW[MAX_ADC_PORT_COUNT];
unsigned short dacThresholdHIGHT[MAX_ADC_PORT_COUNT];

unsigned short dacRes[MAX_ADC_PORT_COUNT];
unsigned short pwr;
double unorm;
unsigned short delaySetOhrana;
unsigned char g_BuiltSensorsPollInterval;

//  ������������ ����� ��������� �������� ������ ��� ���������� �������
#define MAX_RAZGOVOR_TIME	20

//  ��������� ������ � ������������ ����� ������ ��
//  ������ �� ������: �������� zvPage, ����� ������ zvPages ������� ������ ����������
void StartRecord()
{
	record = 1;
	zvukpos = 0;
	TCCR1B = _BV(CS11) | _BV(CS11);		// ����. ��������� �������� 1 = 1
	AT45SetWriteBufferPos(0, 0);
}

//  ��������� zvPages ������� �����, ������� �� �������� zvPage
void StartPlay(unsigned short startPage, unsigned char speacAction)
{
	OCR1B = 0;
	ring = 1;
	zvukpos = 0;
	if (0xFFFF != startPage)			//  ��������
		zvukpage = startPage;
	TCCR1B = _BV(CS11) | _BV(CS11);		// ����. ��������� �������� 1 = 1
	TCNT1 = 0;
	activeBuf = 0;
	AT45ReadPageToBuffer(0, zvukpage);
	AT45ReadPageToBuffer(1, zvukpage + 1);
	zvukpage++;
	AT45SetReadBufferPos(0, 0);

	if (speacAction)
		SpeakerOn();
}

SIGNAL(SIG_UART1_RECV)
{
	USART_Transmit(UDR1);
}

SIGNAL(SIG_UART0_RECV)
{
	HandleReply();
}

void SpeakerOn(void)
{
	if (g_speakerVolume)
		SPEAKERON;
}

//  ����������� 100-��������� �������� � ���������� ���������� ��������� �����
//  ���� ���������� ��������� � �����������, �������� ��������� ������� ���������
void AnalizeInterval(void)
{
	adc_1 = adcCur;						//  ���������� ��������
	adcCur = a2dConvert8bit(1);			//  ������� ��������

	g_sampleCount++;					//  ������� ��������

	if (g_sampleCount > 3)
	{
		g_sampleDiff = adcCur - adc_1;

		if (g_sampleDiff < 0)
			g_sampleDiff = 0;
		else
			g_sampleDiff = 1;

		if (g_sampleDiff != g_oldSampleDiff)
		{	//  ���� ��������� ����� �����
			g_oldSampleDiff = g_sampleDiff;
			if (!g_oldSampleDiff)
				//  ���� �� ������ ����� �����, � ����
				g_fallCounter++;		//  ������� ������
		}
	}

	if (g_sampleCount > 103)
	{	//  �������� ����� �������� ������ - ����������� ���

		g_IntervalCount++;					//  ������� ����������

		//  g_fallCounter - ���������� ������, ��� 425 �� ��� ������ ���� 11-13
		if ((g_fallCounter > 10) && (g_fallCounter < 14))
			g_fallCounter = 1;				//	�������� ������� 425 ��
		else
			g_fallCounter = 0;				//	�������� ��������, ������ ��� ����

		if (g_fallCounter == g_prevIntervalType)
			g_sameIntervalCounter++;					//  ��� ���� ����� ��������
		else
		{	//  ���� �������� �� �����, ��� ����������
			if (g_sameIntervalCounter > 0)
				g_sameIntervalCounter--;

			g_prevIntervalType = g_fallCounter;
		}

		if (g_sameIntervalCounter > g_params[PARAM_SAME_INTERVAL_CNT])
		{	//  ���� ���������� ���������� ���������� ����� �����

			if (voicePresent != g_fallCounter)
			{	//  ���� ��� ���������� ���������� �� �����,�� ���������� ���������
				//  ��� ������ ��� ������ ��������������� ��������� ��������� �����...

				voicePresent = g_fallCounter;
//#ifdef DEBUG
				SetCaret(LINE_1_START_POS + 10);
				OutData(0x30 + voicePresent);
				OutData(32);
				OutStr(itoa(g_IntervalCount, g_szInput, 10));
				OutData(32);
				SetCaret(LINE_2_START_POS);
//#endif
				if (1 == voicePresent)
					g_wasOne = 1;		//  ������ ������ ������� � ������� ������

				if (g_wasOne)
				{
					if (!g_fallCounter)
					{	//  ���� ������ ���������� ������� ��������(�����)
						// 	�, ������, ���������� ��� ��������� ��������

						if ((g_IntervalCount << 1) < g_lastIntervalCount)
						//  ���� ���������� ��������� ����� ��� � ��� ���� ������
						//	�������� ��������
						{
							linemode = LM_KPV;
							lastLinemode = 0;
						}
						else
						{	//  "������" - ������ ������, ���� �������������, ���
							//	��� ��������� "������" - ��� ����� ���� ��� ���� ������
							//  (������-�� ���������� ������,�� ��� ��� ����� ����-���)
							if (LM_BUSY == lastLinemode)
							{
								linemode = LM_BUSY;
							}else
								lastLinemode = LM_BUSY;
						}
					}
				}

				g_lastIntervalCount = g_IntervalCount;
				g_IntervalCount = 0;
			}else
			{	//  ���� ���������� ��������� ��� ������������ � ������������

				if ((g_IntervalCount > g_params[PARAM_GUDOK_LEN]) &&
						g_wasOne &&	voicePresent)
				{ 	//	�������� ����������� �����������  ������� "����� �������"

					linemode = LM_OS_OR_DOWN;
					g_IntervalCount = 0;
				}else

				if (g_IntervalCount > g_params[PARAM_MAXPAUSE_LEN])
				{	//  ���� �� ����� ������ �������� ��� �����

					//  �� ������ ����������� �� ������� ����� ����� ������ ������

					if (g_wasOne)
					{	//  ���� ������ ����� ���-���� ���
						//	(�� ���� ��� �� ��������� ����� ����� ������ ������)

						lastLinemode = 0;

						linemode = LM_OS_OR_DOWN;
						if (!voicePresent)
							linemode = LM_UP;

						g_IntervalCount = 0;
					}
				}
			}

			g_sameIntervalCounter = 0;
			g_prevIntervalType = g_fallCounter;
		}

		g_fallCounter = 0;
		g_sampleCount = 0;
	}
}

SIGNAL(SIG_OVERFLOW1)
{
	unsigned char byte;
/*
	if (g_genDTMF)
	{
		TCNT1 = 0x7F + 64;
		ISR_T1_Overflow();
		g_cntDTMF++;
		if (g_cntDTMF == 2000)
		{
			TCCR1B = 0;
			g_genDTMF = 0;
			OCR1B = 0;
		}
	}
*/
/*
	if (record)
	{	//  ���� ������ ���������� ����
		TCNT1 = g_params[PARAM_TCNT1];

		if (zvukpage < g_params[PARAM_zvPage] + g_params[PARAM_zvPages])
		{
			AT45WriteByteToBuffer(activeBuf, a2dConvert8bit(0));

			zvukpos++;
			if (zvukpos == 512)
			{
				AT45WriteBufferToPage(activeBuf, zvukpage);
				zvukpage++;
				zvukpos = 0;
				if (activeBuf)
					activeBuf = 0;
				else
					activeBuf = 1;
				AT45SetWriteBufferPos(activeBuf, 0);
			}
		} else
		{
			TCCR1B = 0;
			record = 0;
		}
	}
*/
/*	if (dtmfRing)
	{
	}
	*/
	if (ring)
	{	//  ���� ������ ����������� ����
		TCNT1 = g_params[PARAM_TCNT1];

		byte = AT45ReadByteFromBuffer(activeBuf);
		if (byte != 0xFF)
		{
			OCR1B = byte;
			zvukpos++;
			if (zvukpos == 512)
			{
				zvukpage++;
				AT45ReadPageToBuffer(activeBuf, zvukpage);
				zvukpos = 0;
				if (activeBuf)
					activeBuf = 0;
				else
					activeBuf = 1;

				AT45SetReadBufferPos(activeBuf, 0);
			}
		}else
		{
			//OCR1B = 0;
			if (!analizeTELIN)
				TCCR1B = 0;		//  ��������� ������ ������ ��� ���������� ���������
			ring = 0;
			//TCNT1 = 0;

			if (g_nextVoice < MAX_VOICE_QUEUE)
			{	//  �� ��������� �� ���������� ����� ������� ������?

				if (g_VoiceQueue[g_nextVoice])
				{	//  � ���� �� ������� � �������?

					SendVoice(g_VoiceQueue[g_nextVoice], 0);
					g_VoiceQueue[g_nextVoice] = 0;
					g_nextVoice++;
					return;
				}
			}

			timer0delay = 100;
			SPEAKEROFF;
			if (!dontStartConfirmTimer)
				confirmUp = MAX_RAZGOVOR_TIME;
			dontStartConfirmTimer = 0;
		}
	}

	if (analizeTELIN)
	{	//  ���� ������ �������� ����

		AnalizeInterval();

		telinDivider++;
		if (telinDivider == g_params[PARAM_TELIN_ANALIZE_RESOLUTION])
		{
			telinDivider = 0;
			if (g_LineModeAnalizeResultToSPI)
				if (wBufPos[0] < 512)
					AT45WriteByteToBuffer(0, adcCur);
		}
	}
}

//  ������ �� ��������� ������� ������
//  ������ ���������������� � �������� 15625 (16Mhz / 1024)
//  ������������� 15625/256 = ��� � ������� ( ������ 16.38 �� )
SIGNAL(SIG_OVERFLOW0)
{
	TCNT0 = 61;
	if (timer0delay)
		timer0delay--;

	if (builtinSensorsPollInterval)
		if (!(--builtinSensorsPollInterval))
		{	//  ������ ����� ���������� ���������� �������
			if (!ring)
				PollBuiltinSensors2();
			builtinSensorsPollInterval = g_BuiltSensorsPollInterval;
		}
}

/*
char ew = 32, hhh, zvb, gotov;
//  ���������� ������������ MC14LC5480
//  ������� 16000000 ������� �� 32 � ���������� 512000 ����� � �������
//  ����� MC_2048 ��������������� � ������� ������ ������ ��� => �� MC_2048 256 kHz
//  MC_8K ��������������� � ������� ������ 64 ����� ���������� => ��� 8 kHz
SIGNAL(SIG_OVERFLOW0)
{
	if (!(PORTE & MC_2048))
	{	//  ������ ���� ������ ������� MCLK, BCLKT � BCLKR

		//  ������������� ������� ������� �������� �������������
		PORTE |= MC_2048;
	}else
	{	//  ������ ������� ������� MCLK, BCLKT � BCLKR

		//  ������������� ������ ������� �������� �������������
		PORTE &= ~MC_2048;

		//ACSR = PIND & MC_8K;
		if ((unsigned char)(PIND & MC_8K))
		{	//  ���� ����� 8������������ �������

			//w = (PORTG & MC_DR);			//  ������ ��� DT

			if (PING & MC_DR)
				zvb |= 1 << hhh;

			hhh++;								//  ������� �����

			//if (hhh == 8)
			//{
			//	gotov = 1;
			//}
		}

		if (!--ew)
		{
			PORTD |= MC_8K;		//  ����� ��������� ��������������

			ew = 32;			//  �������� ������� 256 kHz ��� ������ 8�
		//	hhh = 0;
		}
	}


	//PORTE ^= MC_2048;		//  ����������� MCLK, BLCKT, BLCKR
}
*/

//  second interrupt
SIGNAL(SIG_OVERFLOW3)
{
	unsigned char k;

   	TCNT3 = TIMER3_SECOND_VALUE;
	//TCNT3 = TIMER3_MAX_INTERVAL + (16 - g_x232Interval) * (TIMER3_200MS_VALUE/2);

	if (g_canShowTime && g_ShowTime && (!ring))		//  ����� ������ �����
		UpdateTimePane();

	//  ��������� ��������� ���-������
	if (MD_SETUP_SENSORS_LIST_DEVICE_SENS_VALUE == g_Mode)
		PrepareToMode(MD_SETUP_SENSORS_LIST_DEVICE_SENS_VALUE);

	if (confirmUp)
	{
		if (!ring)
		{
			SetCaret(LINE_1_START_POS + 14);
			OutStr(itoa(confirmUp, g_szInput, 10));
			OutData(' ');

			if (!--confirmUp)
			{	//  �� ���������� �������� - �����
				if ((advancedLineMode) || (g_wasEnteringCall))
					RingOff();
			}
		}
	}

	//  ������� ������� ������, ������������ ��� �������� ��������
	if (g_secCnt)
		g_secCnt--;

	if (helloCounter)			//  ������������ ������� ������ ������
	{
		if (!--helloCounter)
		{
			PrepareToMode(MD_HELLO);
		}
	}

	//  ���������� ������-���������
	if (progress)
	{
		if (progressPos == LINE_LEN)
		{
			ClearSecondLine();
			progressPos = 0;
		};

		SetCaret(LINE_2_START_POS + progressPos);
		OutData(ITEM_CHECKED);

		progressPos++;
	}

	if (defferedOhranaEvent)
	{	//  ���������� ������� � �������� ��������
		if (!--defferedOhranaEvent)
		{	//  ������� �� ������ �����
			g_DefferedWork |= _BV(DW_SENDOHRANAEVENT);
		}
	}

	if (delaySetOhrana)
	{
		if (!--delaySetOhrana)
		{
			ReduceOhrana();
		}
	}

#ifndef NEWVG
	if(!ring)				//  ������ ������ �����(� ������ ����� �� ����)
		HandleProtocol();	//  ������� ��������� �����(���� �����) � �������� ���
#else
	for (k = 0; k < 8; k++)
	{
		//  ���� ������� ��� �� ��������, ��������� ��� �� ����� ������, �������
		//  ��� ���������� ���������� ��-�� ������� ������ ���������
		//  ��� ������� ��-�� ����������� ���������� ��������� ���� ���������
		//  � ����� ����� �����, ������� ��� ������ ������ - ���� ����������
		if (g_devList[0].sensorTimeout[k])
			g_devList[0].sensorTimeout[k]--;
	}
#endif
}

//  ���������� �� DTMF - �������� MT8870
SIGNAL(SIG_INTERRUPT4)
{
	OCR1B = 0;					//  ����� DTMF ������� ������������ � ����,
									//  ������ ����(delay � SendVoice)

	//  ���� �� ������, � �� ��������� �����������, ����� �������
	linemode = LM_UP;
	g_needHello = 0;				//  ����������� ����� �������� � �� ����

	delayms(50);
	DTMFCmdStorer((PINF & 0xF0) >> 4);
	SetCaret(LINE_1_START_POS + 12);
	OutData(0x30 + ((PINF & 0xF0) >> 4));

//	OutStr(itoa((PINF & 0xF0) >> 4, g_szInput, 10));

}

//  �� ����������
SIGNAL(SIG_INTERRUPT5)
{
	//ClearScreen();
//	OutData('1');

	delayms(50);
	ScanKbd();
	HandleKeys(getcha());
}

#ifndef REDUCE
//  ���������� �������� ������
/*
SIGNAL(SIG_INTERRUPT5)
{
	unsigned char e3, e4;

	g_dtmfPSWOk = 0;		//  ���-�� �� ������ ������, DTMF ����� ��������
#ifdef REDUCE
	g_dtmfPasswordPermission = 0;
#endif

	if ((MD_HELLO == g_Mode) || (MD_TEL == g_Mode))
	{	//  �������� ������ �������������� ������ � ������ 0 � ������ ������ ������
		EIMSK &= ~_BV(INT5); 		//  ��������� ���������� INT5

		//  �������� ��� ���������� ��������
		delay_loop_2(60663);

		e3 = PINE & _BV(PE5);
		e4 = EICRB & _BV(ISC50);

		if ((e3 && e4) || ((!e3) && (!e4)))
		{	//  �������������
			EICRB ^= _BV(ISC50); 		//  ����������� ��� ������ ��� �����

			if (g_telon)
			{	//  �������� ������
				PORTF &= ~TELON;
				PrepareToMode(MD_HELLO);
				g_telon = 0;
				//SetSpeakerVolume(0);
				PORTD &= ~PWA;			//	��������� ���������
				g_hf = 0;
			}else
			{	//  ������� ������ - ������������ � ����� � �������� ���������
				//  ���� �� ������� ����� HandsFree, ������ �� ����������
				g_telon = 1;

				if (!g_hf)
				{
					PORTF |= TELON;
					if (!g_hf)
						Telephone();
				}
			}
		}

		EIMSK |= _BV(INT5); 		//	�������� ���������� INT5
	}
}
*/
#endif

//  ���������� �� ��������� ������ (���������� �����)
SIGNAL(SIG_INTERRUPT6)
{
	if (g_StreamMode) 		//  ��������� ������ ��������� ����� ������
		return;

	g_DefferedSensorsState_ = 0;
	g_dtmfPSWOk = 0;		//  ���-�� �� ������ ������, DTMF ����� ��������
#ifdef REDUCE
	g_dtmfPasswordPermission = 0;
#endif

	if ((MD_HELLO != g_Mode) && (MD_OHRANA != g_Mode))
		return;

	TELUP;								//  ������� ������
	g_wasEnteringCall = 1;

	if (MD_HELLO == g_Mode)
	{
		ClearScreen();

	//	SetSpeakerVolume(g_speakerVolume * 16);

		g_ShowTime = 0;
		OutStr(szZvonok);				//  ������

		modeBeforeZvonok = MD_HELLO;	//  ����� �������, ��������� � 0-��� ������
		delayms(500);
		PlayVoice(VOICE_HELLO, 0);
	}else
	if (MD_OHRANA == g_Mode)
	{	//  ������������ � ������ ������
		g_dtmfPSWOk = 0;				//  ������ �� ������
#ifdef REDUCE
		g_dtmfPasswordPermission = 0;
#endif
		modeBeforeZvonok = MD_OHRANA;	//  ����� �������, ��������� � ������
		delayms(500);
		PlayVoice(VOICE_SYSTEM_POD_OHRANOY, 0);
	}
	//  � ������� xx-�� ������ ������ ����������� ��������
	confirmUp = MAX_RAZGOVOR_TIME;

//	delayms(1000);			//  ����� ����� �� ��� �������...
}

//  ���������� �� ��������� ������ (GSM)
SIGNAL(SIG_INTERRUPT7)
{
}

/*
//  ��������� ������ 200 �����������
SIGNAL(SIG_OUTPUT_COMPARE3A)
{
	OCR3A = TCNT3 + TIMER3_200MS_VALUE;
	if (4 == OCR3A)
		OCR3A = 0xFFFF;

	//HandleProtocol();
}
*/

void ReadEEPROMData(void)
{
	RTC_TIME rtc;

	g_canShowTime = eeprom_read_byte_1((void*)EADR_CAN_SHOW_TIME);
	advancedLineMode = eeprom_read_byte_1((unsigned char*)EADR_ADV_LINE_MODE);
	epmrbl(&g_szSetupPassword, (unsigned char*)EADR_SETUP_PSWD, MAX_PASSWORD_LEN);
	if (g_szSetupPassword[0] == 0xFF)
	{	//  �� � ������ ����� �������� - ������ �� ��������� - "0000"
		memset(&g_szSetupPassword, 0x30, 4);
		g_szSetupPassword[4] = 0;
	}
	g_szSetupPassword[MAX_PASSWORD_LEN] = 0;	//  ����������

	//  ��������� ����� ������ �������
	ReadTime(&rtc);
	//epmwbl(&rtc, (void*)EADR_START_TIME, sizeof(rtc));

	//  ��������� ��������
	g_speakerVolume = eeprom_read_byte_1((unsigned char*)EADR_VOLUME_DINAMIC);

	//  ������ ������ ���������
	unsigned char k;
	for (k = 0; k < MAX_TEL_AUTO_DIAL_NUMBERS_ITEMS; k++)
	{
		epmrbl(&(g_szAutoDialNumbers[k][0]),
			(void*)EADR_AUTO_DIAL_NUMBERS + k*(INPUT_LEN_TEL + 1),
			(INPUT_LEN_TEL + 1));

		g_szAutoDialNumbers[k][15] = 0;

		if (0xFF == g_szAutoDialNumbers[k][0])
		{
			g_szAutoDialNumbers[k][0] = 0x31 + k;
			memset(&g_szAutoDialNumbers[k][1], '.', 9);

			g_szAutoDialNumbers[k][10] = 0;
		}
	}
//#ifdef DEBUG
	memcpy(g_szAutoDialNumbers[0], "105\x0" , 4);
//#endif
}

void Init(void)
{
	useLocalDTMF = 0;
	g_GalkaPunkt = 0xFF;
	g_ShowTime = FALSE;
	g_wasEnteringCall = FALSE;

	// disable analog comparator
	ACSR = ( ACSR & (uint8_t) ~( _BV( ACIE ) ) ) | _BV( ACD );

	DDRA = 0xFF;
	PORTA = 0xFF;
	DDRC = 0xE0;
	DDRE = _BV(PE3);

	PORTF = 0xF0;
	DDR_TELON = TELON;
	DDRG = LCD_RW | LCD_E | LCD_RS | MC_DT;
	DDRB = 0xF6;					//  PB5 = CSR + PB6 - DIG

	DDRD = PWR | PWA;		//  PD4, PD5 - OUT
	PORTD = PWR;					//  PD4 - ��� GSM

	PORTG = 0x00;
	PORTB = 0x66;
	PORTE = 0x04;


	EIFR = 0;				//  ������� �� ������

	//  ��������� ������� ����������
	EICRB = _BV(ISC71)|_BV(ISC70)|
//#ifndef REDUCE
			//_BV(ISC51)|_BV(ISC50)|
//#endif
			_BV(ISC60)|
			_BV(ISC61)|

		 	_BV(ISC40)|
			_BV(ISC41);

	EIMSK = _BV(INT7) |
	_BV(INT6)|
//#ifndef REDUCE
	//_BV(INT5)|
//#endif
	_BV(INT4);

	InitLCD();			//  ��������

	InitRTC();			//  ������������� DS1307

	if (g_canShowTime)
		UpdateTimePane();
	else
		HideCaret();

	InitDevices();

	g_BuiltSensorsPollInterval = g_x232Interval;
	ReadEEPROMData();

#ifndef PROTEUS

	spiInit();

#endif
	uart_init();

	InitTimer3();
	InitTimer0();

	sei();

#ifndef PROTEUS
	fatxInit();

	//initDTMF();
	InitPWM();
	//SetSpeakerVolume(0);

	a2dInit();
	a2dSetPrescaler(ADC_PRESCALE_DIV64);
#endif
}

//  ������ ������� ��� ������ ���� (������������ � ������ ������� �����������)
void ChangeMenuItems(signed char direction)
{
	unsigned char newItem = g_SetupItem + direction;

	strcpy(g_szInput, g_szAutoDialNumbers[newItem]);

	//  ���� ���� ����� ������ �������� ������ �����, �� ������������ ����� ������...
	if ('.' == g_szInput[1])
		g_szInput[0] = 0x31 + g_SetupItem;

	strcpy(g_szAutoDialNumbers[newItem],
		g_szAutoDialNumbers[g_SetupItem]);

	//  ��� ���� ���� ���� ���� �������� ��������, ����� ����� ��������� �
	//  �������������� ������� ���������� ���������� ������ 2..... � 2......,
	//  ����������� � ������ ������ ������
	if ('.' == g_szAutoDialNumbers[newItem][1])
		g_szAutoDialNumbers[newItem][0] = 0x31 + newItem;

	strcpy(g_szAutoDialNumbers[g_SetupItem], g_szInput);

	MenuAutoDialNumbers(newItem);
}

void Menu_(unsigned char mode, unsigned char maxMenuItems)
{
	g_Mode = mode;
	g_CurMenuMaxItems = maxMenuItems;
	MenuItemsInitAndDraw(0, 0);
}

void MainMenu(void)
{
	helloCounter = 0;
	g_Mode = MD_SETUP;
	g_CurMenuMaxItems = MAX_MAIN_SETUP_ITEMS;
	MenuItemsInitAndDraw(g_oldSetupFirstLine, g_oldSetupCurItem);
}

void MenuSensors(unsigned char SetupFirstLine, unsigned char SetupCurItem)
{
	g_Mode = MD_SENSORS;
	g_CurMenuMaxItems = MAX_SENSORS_ITEMS;
	//g_GalkaPunkt = SEN_POLL;

	MenuItemsInitAndDraw(SetupFirstLine, SetupCurItem);
}

void MenuList(void)
{	//  ������ ���������( ���� 1, ���� 2) � �.�.
	Menu_(MD_SETUP_SENSORS_LIST, MAX_DEVICE_COUNT);
}

void MenuSensor(void)
{	//  ���� ���� - ���� ���� (��� ������) �������� ����������
	Menu_(MD_SETUP_SENSORS_LIST_DEVICE_SENSOR, MAX_SENSOR_COUNT);
}

void MenuDevice(unsigned char b)
{
	if (b)
	{
#ifndef 	NEWVG
		g_CurDev.adr = g_SetupItem + 3;						//  ����� ����������(�����)
#else
		g_CurDev.adr = 0;
#endif

		//  ��������� �� EEPROM (��������, ���� �������� �� ���������)
		epmrbl(g_CurDev.sensorTimeout, (const char *)(EADR_DEVICE_LIST +
			g_SetupItem*sizeof(XDEVICE) + 3), 16);

		ClearScreen();
#ifndef NEWVG
		if (!ConnectToDevice())
		{
			OutStr(szNetOtveta);
			OutData(0x31);
	#ifndef PROTEUS
			return;
	#endif
		}
#endif
		if (!GetCurDeviceInfo())
		{
			OutStr(szNetOtveta);
			OutData(0x32);
#ifndef PROTEUS
			return;
#endif
		}

	}

	Menu_(MD_SETUP_SENSORS_LIST_DEVICE, MAX_DEVICE_ITEMS);
}

void MenuListPort(void)
{
	//  ������ ������(��������) �������� ���������� - ���� 1, ���� 2 � �.�.
	Menu_(MD_SETUP_SENSORS_LIST_DEVICE_SENSLIST, MAX_SET_SENS_ITEMS);
}

void MenuTel(void)
{
	g_GalkaPunkt = STN_ADV_LINE_MODE;
	g_Galka = advancedLineMode;
	Menu_(MD_SETUP_TEL, MAX_TELEFON_ITEMS);
}

void MenuSettings(void)
{
	Menu_(MD_SETUP_SETTINGS, MAX_SETTINGS_ITEMS);
}

void MenuVolume(void)
{
	Menu_(MD_SETUP_VOLUME, MAX_VOLUME_ITEMS);
}

void MenuX232(unsigned char SetupFirstLine, unsigned char SetupCurItem)
{
	Menu_(MD_SETUP_X232, MAX_X232_ITEMS);
}

void MenuAutoDialNumbers(unsigned char SetupFirstLine)
{
	unsigned char item = 0;

	if (MAX_TEL_AUTO_DIAL_NUMBERS_ITEMS - 1 == SetupFirstLine)
	{
		SetupFirstLine = MAX_TEL_AUTO_DIAL_NUMBERS_ITEMS - 2;
		item = 1;				//  ���� ��������� ���, ��� ������ ���� ������� ���������
	}

	Menu_(MD_SETUP_TEL_AUTO_DIAL_NUMBERS, MAX_TEL_AUTO_DIAL_NUMBERS_ITEMS);
}

void MenuItemsInitAndDraw(unsigned char SetupFirstLine, unsigned char SetupCurItem)
{
	g_SetupCurItem = SetupCurItem;
	g_SetupFirstLine = SetupFirstLine;
	g_SetupItem = SetupFirstLine + SetupCurItem;
	g_checkedItem = 0xFF;		//  ��� ��������� �������

	if (MD_SETUP_TEL_LINEMODE == g_Mode)
		g_checkedItem = eeprom_read_byte_1((void*)EADR_LINE_MODE);

	DrawMenu();
}


void DrawBarInterface(void)
{
	ClearScreen();
	OutStr((char*)pMasItems[g_SetupCurItem]);	//TODO: ������������� ������
	g_curBarValue = eeprom_read_byte_1((void*)(tmp + g_SetupCurItem));

	UpdateBarInterface();
}

void DrawVolume(void)
{
	g_Mode = MD_SETUP_VOLUME_DINAMIC + g_SetupCurItem;
	tmp = EADR_VOLUME_DINAMIC;
	pMasItems = (unsigned char**)&g_szVolumeItems;

	DrawBarInterface();
}

void DrawInterval(void)
{
	g_Mode = MD_SETUP_X232_TIMEOUT + g_SetupCurItem;
	tmp = EADR_X232_TIMEOUT;
	pMasItems = (unsigned char**)&g_szX232Items;

	DrawBarInterface();
}

void UpdateBarInterface(void)
{
	char k;

	ClearInput();

	if (g_curBarValue > LINE_LEN)
		g_curBarValue = LINE_LEN;

	if ((MD_SETUP_VOLUME_TELEFON == g_Mode) ||
		(MD_SETUP_VOLUME_DINAMIC == g_Mode))
	{
		SetCaret(LINE_1_START_POS + 11);
		OutStr(itoa((100 / 16) * g_curBarValue, g_szInput, 10));
		OutData('%');
	}else
	{
		SetCaret(LINE_1_START_POS + 10);
		OutStr(itoa(g_curBarValue * 100, g_szInput, 10));
		OutData(0xBC);
		OutData(0x63);
	}

	OutData(' ');				//  ������� ���������� ��������
	if (!g_curBarValue)
		OutData(' ');			//  ����� �������� ����� 's' �� 100ms

	SetCaret(LINE_2_START_POS);

	for (k = 0; k < LINE_LEN; k++)
	{
		if (k < g_curBarValue)
			OutData(ITEM_CHECKED);
		else
			OutData(ITEM_UNCHECKED);
	}
}

void PrepareToMode(unsigned char mode)
{
	unsigned char br, br1, port;
	g_Mode = mode;
	ClearScreen();

	if (mode < MAX_MODE_ITEMS)
		OutStr((char*)g_szModes[mode]);		//TODO: ������������� ������

	ClearInput();

	if (MD_SETUP_TIME == mode)
	{	//  ��� ���� ������� ������� �������� �������, ����� ��� �������������
		UpdateTimePane();
		ReadStrFromScreen(g_szInput);
	}
	if (MD_HELLO == mode)
	{
		HideCaret();
		g_ShowTime = TRUE;
		if (g_canShowTime)
			UpdateTimePane();
	}
	if (MD_SETUP_SENSORS_NEWBAUDRATE == mode)
	{
		br = eeprom_read_byte_1((unsigned char*)EADR_USART_BAUDRATE);
		if (br >= 7)
			OutData(0x30);
		else
			OutStr(strcat(itoa(BaudRates[br] << 3, g_szInput, 10), "00"));

		OutData(')');
	}
	if (MD_SETUP_SENSORS_LIST_DEVICE_TIMEOUT == mode)
	{
		OutStr(itoa(g_CurDev.ohrTimeout, g_szInput, 10));
		OutData(')');
	}

	ShowCaret();

	SetCaret(LINE_2_START_POS);

	if (MD_SETUP_SENSORS_LIST_DEVICE_SETPORT == mode)
	{
		HideCaret();

		//  ����� ��������� ���������� ������
		br = '+';
		br1 = '-';
		port = g_CurDev.ports;

		if (1 == g_CurDev.mode)
		{	//  ����� �����������
			br = 0xFF;
			br1 = 0xEF;
			port = g_CurDev.ports_state;
		} else
		if (2 == g_CurDev.mode)
		{
			ClearScreen();

			for (mode = 0; mode < 4; mode++)
			{
				SetCaret(LINE_1_START_POS + 4*mode);
				if (g_CurDev.ports & _BV(mode))
					OutStr(itoa(dacRes[mode], g_szInput, 10));
				else
					OutStr("XXX");
			}
			for (mode = 0; mode < 4; mode++)
			{
				SetCaret(LINE_2_START_POS + 4*mode);
				if (g_CurDev.ports & _BV(mode + 4))
					OutStr(itoa(dacRes[mode + 4], g_szInput, 10));
				else
					OutStr("XXX");
			}
			return;
		}

		for (mode = 0; mode < 8; mode++)
		{
			if ((1 == g_CurDev.mode) && (!(g_CurDev.ports & _BV(mode))))
				OutData('X');
			else
			{
				if (port & (1 << mode))
					OutData(br);
				else
					OutData(br1);
			}

			OutData(32);
		}
	}//TODO: werwerwer
	if (MD_SETUP_SENSORS_LIST_DEVICE_SENS_VALUE == mode)
	{	//  ������� ��� ������� ��������

		//  �������� ����� �������� - ��� ���� ��� ����� AdjustLimits
		//  � � ��� �������� U��� � ��.
		builtinSensorsPollInterval = g_BuiltSensorsPollInterval;
		HFON;
#ifdef NEWVG
		ClearScreen();
		OutStr("\x42\x65\x70\x20");		//  ���
		OutStr(itoa(sensorsTopLimit2, g_szInput, 10));
		SetCaret(LINE_1_START_POS + 9);
		OutStr("\x48\xB8\xB7\x20");		//  ���
		OutStr(itoa(sensorsBottomLimit2, g_szInput, 10));
		SetCaret(LINE_2_START_POS);
		OutStr("\xA8\xB8\xBF\x20");		//  ���
		OutStr(itoa((unsigned short)pwr, g_szInput, 10));
		SetCaret(LINE_2_START_POS + 9);
		OutStr("\xE0\x61\xBF\x20");		//  ���
		OutStr(itoa(dacRes[g_CurDev.curSensor], g_szInput, 10));
#else
		OutStr(itoa(g_CurDev.threshold1, g_szInput, 10));
		SetCaret(LINE_2_START_POS + 7);
		OutStr(itoa(g_CurDev.threshold2, g_szInput + 7, 10));

		//  ��������� �� ����� ����� ��������
		//  ���� � ������ ���������� ������� ��� ������ ��������� �����
		//  ������ �������� ������, ������ �������� � ����� ���������, ��� ��������
		//  ����� ��������. ���� �� ������ �������� ����� ����� �����, �������
		//  �������� �� �������, ���� � ��� ����� - ���� ��������
		SetCaret(LINE_2_START_POS);
#endif
	}
	if (MD_SETUP_X232_PACKET == mode)
	{
	}else
	if ((MD_SETUP_TEL_AUTO_DIAL_NUMBER == mode) /*|| (MD_REDUCE_EDIT_ == mode)*/)
	{
		SetCaret(LINE_2_START_POS);
		if (g_szAutoDialNumbers[g_SetupItem][1] != '.')
		{
			OutStr(strcpy(g_szInput, g_szAutoDialNumbers[g_SetupItem]));
			SetCaret(LINE_2_START_POS);
		}
	}else
	if (MD_SETUP_SETTINGS_RAM == mode)
	{
	}else
	if (MD_SETUP_SENSORS_LIST_DEVICE_SENSOR_TIMEOUT1 == mode)
	{
		OutStr(ltoa(g_CurDev.sensorTimeout[g_CurDev.curSensor], g_szInput, 10));
		SetCaret(LINE_2_START_POS);
	}

	if ((MD_HELLO == mode) || (MD_OHRANA == mode))
		HideCaret();
}

//  ���� � ����� MD_SETUP_WAITPSWD
void Setup(void)
{
	g_ShowTime = FALSE;

	g_curBarValue = MD_SETUP;

	//  ����� �������� ����� ������
	PrepareToMode(MD_SETUP_WAITPSWD);
	helloCounter = 10;
	ClearInput();
}

void ClearInput(void)
{
	memset(&g_szInput, 0, sizeof(g_szInput));
	g_InputPos = 0;
}

void Backspace(void)
{
	unsigned char e = g_szInput[g_InputPos - 1];

	g_szInput[g_InputPos - 1] = 0;
	g_szLastNumber[g_InputPos - 1] = 0;

	if ((':' == e)||(' ' == e)||('.' == e))
		DeletePrevChar();						//  � ������ ��������� �������

	DeletePrevChar();
}

void Ohrana(void)
{
	g_ShowTime = FALSE;

	g_curBarValue = MD_OHRANA;					//  ����������� - ������������ �������

	if (MD_OHRANA == g_Mode)
		//  ������ � ������ ������ - ����� �� ������ ������
		g_curBarValue = MD_HELLO;

	PrepareToMode(MD_SETUP_WAITPSWD);
	ClearInput();
}

void Telephone(void)
{
	g_ShowTime = FALSE;
	PrepareToMode(MD_TEL);
	g_bSilent = 0;
}

unsigned char* GetItemName(unsigned char index)
{
	unsigned char mode = g_Mode;
	unsigned char m[2] = {0};

	//if (MD_SETUP == mode)		�����������...
	pMasItems = (unsigned char**)&g_szSetupItems;

	if (MD_SENSORS == mode)
		pMasItems = (unsigned char**)&g_szSensorsItems;
	else if (MD_SETUP_TEL == mode)
		pMasItems = (unsigned char**)&g_szTelefonItems;
	else if (MD_SETUP_SENSORS_LIST == mode)
	{
		ClearInput();
		strcat(g_szInput, szUstr);		//  ����
		return strcat(g_szInput, itoa(index, m, 10));

	}else if (MD_SETUP_SENSORS_LIST_DEVICE == mode)
		pMasItems = (unsigned char**)&g_szDeviceItems;
	else if (MD_SETUP_TEL_LINEMODE == mode)
		pMasItems = (unsigned char**)&g_szLinemodeItems;
	else if (MD_SETUP_VOLUME == mode)
		pMasItems = (unsigned char**)&g_szVolumeItems;
	else if (MD_SETUP_SENSORS_LIST_DEVICE_SENSLIST == mode)
	{
		ClearInput();
		strcat(g_szInput, szVhod);				//  ���� -

		return strcat(g_szInput, itoa(index + 1, m, 10));
	}
	else if (MD_SETUP_X232 == mode)
		pMasItems = (unsigned char**)&g_szX232Items;
	else if ((MD_SETUP_TEL_AUTO_DIAL_NUMBERS == mode) || (MD_REDUCE_EDIT == mode))
		pMasItems = (unsigned char**)&g_szAutoDialNumbers;
	else if (MD_SETUP_SETTINGS == mode)
		pMasItems = (unsigned char**)&g_szSettingsItems;
	else if (MD_SETUP_SENSORS_LIST_DEVICE_SENSOR == mode)
		pMasItems = (unsigned char**)&g_szDeviceSensorItems;

	return pMasItems[index];
}

void DrawMenuLine(unsigned char lineNo, unsigned char lineStartPos)
{
	unsigned char lineNo_ = lineNo;


	if (MD_SETUP_SENSORS_LIST == g_Mode)
#ifndef NEWVG
		lineNo = lineNo + 3;				//  � ���� ������ ��������� � 3
#else
		lineNo = lineNo + 1;
#endif

	OutStr(GetItemName(g_SetupFirstLine + lineNo)); //  ������� ������ ������ ����

	if (g_SetupCurItem == lineNo_)			//  ���� �� ������� �� ������ ������
	{
		SetCaret(lineStartPos + ARROW_START_POS);

		OutStr(szStrelka);
	}

	if ((g_SetupFirstLine + lineNo == g_GalkaPunkt) ||
		(g_checkedItem != 0xFF))
	{
		SetCaret(lineStartPos + CHECKBOX_START_POS);

		if ((g_Galka) || (g_checkedItem == g_SetupFirstLine + lineNo))
			OutData(ITEM_CHECKED);
		else
			OutData(ITEM_UNCHECKED);
	}
}

//  ����������� ���� Setup'a

//  �������� ���������� ���. ��� ���� - � ����������� ����(����� MD_SETUP_TEL_LINE_MODE)
//  � � ������������� ����. � ������ ���� ����� ����� ������� �������� �������,
//  � �� ������ ����� ������ ����� �������� � ������ ������

void DrawMenu(void)
{								//  ���� (�����) ����� �� �������
	ClearScreen();
	HideCaret();

	DrawMenuLine(0, 0);			//  ���������� ������ ������

	SetCaret(LINE_2_START_POS);

	if (g_CurMenuMaxItems > 1)
		DrawMenuLine(1, 0x40);		//  ���������� ������ ������
/*
	if (MD_SETUP_SENSORS_LIST_DEVICE == g_Mode)
	{
		if (2 == g_SetupFirstLine)
		{
			SetCaret(LINE_1_START_POS + 8);
		}else
		if (1 == g_SetupFirstLine)
		{
			SetCaret(LINE_2_START_POS + 8);
		}else
			return;

		if (g_CurDev.state & _BV(DS_OHRANA))
			OutData('1');
		else
			OutData('0');
	}
*/
}

void DeletePrevChar(void)
{
	SetCaret(--g_CaretPos);
	OutData(32);
	g_InputPos--;
}

//  ���������� ������� ������ ����� � ������
void LeftRight(unsigned char key)
{
	unsigned char mode = g_Mode;
	unsigned char e;
	unsigned char inputPos = g_InputPos;

	if ((MD_SETUP == mode) 		|| (MD_SENSORS == mode) ||
		(MD_SETUP_TEL == mode)	|| (MD_SETUP_SENSORS_LIST == mode) ||
		(MD_SETUP_SENSORS_LIST_DEVICE == mode) ||
		(MD_SETUP_TEL_LINEMODE == mode) || (MD_SETUP_VOLUME == mode) ||
		(MD_SETUP_SENSORS_LIST_DEVICE_SENSLIST == mode) ||
		(MD_SETUP_X232 == mode) ||
		(MD_SETUP_TEL_AUTO_DIAL_NUMBERS == mode) ||
		(MD_SETUP_SETTINGS == mode) ||
		(MD_SETUP_SENSORS_LIST_DEVICE_SENSOR == mode) ||
		(MD_REDUCE_EDIT == mode)
		)
	{	//  ����������� �� ����
		if (VK_UP == key)					//  �����
		{
			if (g_SetupCurItem > 0)
				g_SetupCurItem = 0;
			else
			if (g_SetupFirstLine > 0)
				g_SetupFirstLine--;
			else
			{
				g_SetupFirstLine = g_CurMenuMaxItems - 2;
				g_SetupCurItem = 1;
			}
		}

		if (VK_DOWN == key)					//  ����
		{
			if (!g_SetupCurItem)
				g_SetupCurItem = 1;
			else
			if (g_SetupFirstLine < g_CurMenuMaxItems - 2)
				g_SetupFirstLine++;
			else
			{
				g_SetupFirstLine = 0;
				g_SetupCurItem = 0;
			}
		}

		g_SetupItem = g_SetupFirstLine + g_SetupCurItem;
		if ((g_SetupItem == g_CurMenuMaxItems) ||
			(g_SetupCurItem == g_CurMenuMaxItems))
		{	//  ���� � ���� ����� ���� ���� ����� (� ��������� ������)
			g_SetupItem = 0;
			g_SetupCurItem = 0;
		}
		DrawMenu();
	}else
	if ((MD_SETUP_NEWPSWD == mode) || (MD_SETUP_WAITPSWD == mode)
		|| (MD_REDUCE_EDIT_ == mode))
	{	//  ������ ����� ��������
		if (VK_LEFT == key)
		{
			if (g_CaretPos > LINE_2_START_POS)
			{
				Backspace();
			}
		}
	}else
	if ((MD_TEL == mode) || (MD_SETUP_SENSORS_NEWBAUDRATE == mode) ||
		(MD_SETUP_SENSORS_LIST_DEVICE_TIMEOUT == mode) ||
		(MD_SETUP_X232_PACKET == mode) ||
		(MD_SETUP_TEL_AUTO_DIAL_NUMBER == mode) ||
		(MD_SETUP_SENSORS_LIST_DEVICE_SENSOR_TIMEOUT1 == mode)||
		(MD_SETUP_TIME == mode)
		)
	{
		if (VK_BACKSPACE == key)		//  � ���� ������� FUNC = Backspace
		{
			if (g_CaretPos > LINE_2_START_POS)
			{
				Backspace();

				for (e = g_InputPos; e < LINE_LEN; e++)
				{
					g_szInput[e] = g_szInput[e + 1];
					OutData(32);
				}

				SetCaret(LINE_2_START_POS);
				OutStr(g_szInput);
			}
		}else
		{
			e = 0;
			if (VK_LEFT == key)
			{
				if (g_CaretPos > LINE_2_START_POS)
					e = -1;
			}else
			if (VK_RIGHT == key)
				//  ������� ������� ������, � ����� ������ (���� ���� ��������� �������)
				if ((g_CaretPos < LINE_2_START_POS + LINE_LEN) &&
					(inputPos < strlen(g_szInput)))
					e = 1;

			if (0 != e)
			{
				mode = g_szInput[inputPos + e];

				if ((':' == mode)||(' ' == mode)||('.' == mode))
					g_InputPos = g_InputPos + e;

				g_InputPos = g_InputPos + e;
			}
		}

		g_CaretPos = LINE_2_START_POS + g_InputPos;

	}else
	if ((MD_SETUP_VOLUME_DINAMIC == mode) || (MD_SETUP_VOLUME_TELEFON == mode) ||
		(MD_SETUP_X232_TIMEOUT == mode) || (MD_SETUP_X232_INTERVAL == mode))
	{
		if (VK_DOWN == key)
		{
			if (g_curBarValue < 16)
				g_curBarValue++;
		}
		else if (g_curBarValue)
				g_curBarValue--;

		UpdateBarInterface();

		if (MD_SETUP_VOLUME_DINAMIC == mode)
			ChangeVolume();
	}else
	if ((MD_SETUP_SENSORS_LIST_DEVICE_SENS_VALUE == mode) ||
		(MD_SETUP_SETTINGS_RAM == mode) ||
		(MD_SETUP_SETTINGS_EEPROM == mode))
	{
		if (VK_LEFT == key)
		{
			if (g_CaretPos > LINE_2_START_POS)
			{
				if (inputPos == 7)
				{
					g_InputPos = strlen(g_szInput);
					SetCaret(LINE_2_START_POS + g_InputPos);
				}

				if ((MD_SETUP_SENSORS_LIST_DEVICE_SENS_VALUE == mode) ||
					(inputPos < 7))
				{
					g_szInput[inputPos - 1] = 0;
					DeletePrevChar();
				}else
				if (inputPos > 7)
				{
					g_szInput[inputPos - 1] = 0x30;
					SetCaret(--g_CaretPos);
					OutData(0x30);
					g_InputPos--;
				}
			}
		}
		else
		if (VK_RIGHT == key)
		{
			if ((g_CaretPos < LINE_2_START_POS + LINE_LEN) && (g_InputPos > 6) &&
				(MD_SETUP_SENSORS_LIST_DEVICE_SENS_VALUE != mode))
			{
				g_CaretPos++;
				g_InputPos++;
			}
		}
	}else
		return;

	SetCaret(g_CaretPos);
}

//  ���������� ������� �������� ������
void NumerKey(unsigned char key)
{
	unsigned char mode = g_Mode, k;
	unsigned char inputPos = g_InputPos, numk2 = g_numbers[key], numk = numk2 - 0x30;

	if (MD_REDUCE_EDIT == mode)				//  ���� �������� � ������ �������
		PrepareToMode(MD_HELLO);			//  ����������� ������

	if (MD_HELLO == mode)
	{	//  ����� ���������� �������
		if (!inputPos)
		{
			if (VK_6 == key)
			{	//  ����� �������������� �������
				g_ShowTime = FALSE;
				MenuAutoDialNumbers(0);
				g_Mode = MD_REDUCE_EDIT;
				return;
			}
		}else
		if (!inputPos)
		{	//  ������ ����� ����������� ������ �� �������� ������
			//  ������� � ����� ��������!!!!!!!
			Telephone();
			mode = MD_TEL;
			g_bSilent = 1;
		}
	}

	//  ������� �� ����� ��������� � ��������� �������
	if (
		(MD_SETUP_WAITPSWD != mode) && (MD_TEL != mode) &&
		(MD_SETUP_TIME != mode) && (MD_SETUP_NEWPSWD != mode)
		&& (MD_SETUP_SENSORS_NEWBAUDRATE != mode)
		&& (MD_SETUP_SENSORS_LIST_DEVICE_TIMEOUT != mode)
		&& (MD_SETUP_SENSORS_LIST_DEVICE_SETPORT != mode)
		&& (MD_SETUP_SENSORS_LIST_DEVICE_SENS_VALUE != mode)
		&& (MD_SETUP_X232_PACKET != mode)
		&& (MD_SETUP_TEL_AUTO_DIAL_NUMBER != mode)
		&& (MD_SETUP_SETTINGS_RAM != mode)
		&& (MD_SETUP_SETTINGS_EEPROM != mode)
		&& (MD_SETUP_SENSORS_LIST_DEVICE_SENSOR_TIMEOUT1 != mode)
		&& (MD_REDUCE_EDIT_ != mode)
		)
		return;

//	k = g_InputLens[mode];

	if ((MAX_INPUT_LENS_COUNT >= mode) && (g_InputLens[mode]))
		if (inputPos > g_InputLens[mode])
			return;				//  ��������� ����� ������ ��� �������� ������

	//  �������� ��� ������ ������, ���� ��������� �����
	if ((MD_SETUP_TIME != mode) && (MD_SETUP_SENSORS_LIST_DEVICE_SENS_VALUE != mode) &&
		(MD_REDUCE_EDIT != mode) && (MD_SETUP_SETTINGS_RAM != mode)
		&& (MD_SETUP_SETTINGS_EEPROM != mode)
		)
	{
		for (k = LINE_LEN - 1; k > inputPos; k--)
			g_szInput[k] = g_szInput[k - 1];

		SetCaret(LINE_2_START_POS + inputPos + 1);
		OutStr(g_szInput + inputPos + 1);
		SetCaret(LINE_2_START_POS + inputPos);
	}

	g_szInput[inputPos] = numk2;
	inputPos++;

	//TODO: ����� �� ����� ����� ���������
	if (/*(MD_TEL == mode) || */(MD_SETUP_SENSORS_NEWBAUDRATE == mode) ||
		(MD_SETUP_SENSORS_LIST_DEVICE_TIMEOUT == mode)||
		(MD_SETUP_X232_PACKET == mode) ||
		(MD_SETUP_SENSORS_LIST_DEVICE_SENSOR_TIMEOUT1 == mode))
		OutData(numk2);	//  ��� ����� ��������� �� �����
	else if ((MD_SETUP_WAITPSWD == mode)||(MD_SETUP_NEWPSWD == mode))
		OutData('*');		//  ��������� ������ ������
	else if (MD_SETUP_TIME == mode)
	{
		OutData(numk2);

		if (2 == inputPos)
			key = ':';
		else if (5 == inputPos)
			key = ' ';
		else if ((8 == inputPos) || (11 == inputPos))
			key = '.';
		else
			goto ee;

		OutData(key);
		g_szInput[inputPos] = key;
		inputPos++;
		g_CaretPos++;

	} else if (MD_SETUP_SENSORS_LIST_DEVICE_SETPORT == mode)
	{	//  g_CurDev.ports - ���������� ������ �������� ������ ��������

		if (!numk)
		{	//  ������ 0-����� ��������� 1)���������� �������� 2)���������
			//  ���������� 3)����� ��������� � ������� ���������� ������
			if (++g_CurDev.mode == 3)
				g_CurDev.mode = 0;

			//  ���� �����, �������� ����� ������ ���������� ��������
			if (g_CurDev.mode)
			{
				builtinSensorsPollInterval = g_BuiltSensorsPollInterval;
				HFON;
			}
			else
			{
				builtinSensorsPollInterval = 0;
				HFOFF;
			}

			g_CurDev.ports_state = 0;
		}

		if (!g_CurDev.mode)
			if ((numk < 9) && (numk > 0))
				g_CurDev.ports ^= 1 << (numk - 1);

		PrepareToMode(MD_SETUP_SENSORS_LIST_DEVICE_SETPORT);

	} else if (MD_SETUP_SENSORS_LIST_DEVICE_SENS_VALUE == mode)
	{
		if (inputPos == 12)
			return;	//  ��� ���� �������� ������ ������ � g_szInput

		if ((inputPos == 1) || (inputPos == 8))
		{
			if (inputPos == 1)		//  ������ ����
				if (g_CurDev.threshold1)		//  ��� ������������ ������ ��������
					//  ������� �������� ��������� ��������(������) �� �����
					numk = 0;

			if (inputPos == 8)		//  ������ ����
				if (g_CurDev.threshold2)		//  ��� ������������ ������ ��������
					//  ������� �������� ��������� ��������(�������) �� �����
					numk = 7;

			memset(g_szInput + numk, 0, sizeof(g_szInput) - numk);
			g_szInput[inputPos - 1] = numk2;
			OutStr("      ");
			SetCaret(LINE_2_START_POS + numk);
		}

		OutData(numk2);

		if (inputPos == 4)
		{	//  ������� �� ����� ������� ������
			inputPos = 7;
			g_CaretPos = LINE_2_START_POS + 6;		//  ����������� g_CaretPos++
		}
	}else
	if ((MD_SETUP_SETTINGS_RAM == mode) || (MD_SETUP_SETTINGS_EEPROM == mode))
	{
		if (4 == inputPos)
		{
			GetTetradeFromAdr();
			return;
		}
		else
			OutData(numk2);
	}

	if (MD_SETUP_SENSORS_LIST_DEVICE_SENSOR_TIMEOUT1 == mode)
	{
		if (1 == inputPos)
		{
			memset(g_szInput, 0, sizeof(g_szInput));
			g_szInput[inputPos - 1] = numk2;
			OutStr("      ");
			g_CaretPos = LINE_2_START_POS;
		}
	}

	if (MD_TEL == mode)
	{
		if (!g_bSilent)
			DialDigit(numk);

		OutData(numk2);

		if (1 == inputPos)
			//  ������� �������, ���� ������ �������� �����	�����
			memset(&g_szLastNumber, 0, sizeof(g_szInput));

		g_szLastNumber[inputPos - 1] = numk2;
	}else
	if (MD_SETUP_TEL_AUTO_DIAL_NUMBER == mode)
	{
		OutData(numk2);
	}else
	if ((MD_REDUCE_EDIT_ == mode))
	{
		if (1 == inputPos)
		{
			ClearInput();
			ClearSecondLine();
			g_szInput[0] = numk2;
		};

		OutData(numk2);
	}
ee:
	g_InputPos = inputPos;

	g_CaretPos++;
	SetCaret(g_CaretPos);
}

//  ���������� ������� ������� *
void Escape(void)
{
	unsigned char mode = g_Mode;	//  �������, ���� ���-�� mode > 15
	unsigned char _mode = g_curBarValue;

	HideCaret();
	g_GalkaPunkt = 0xFF;

	if (MD_SETUP_WAITPSWD == mode)
	{	//  ������ �������� � �����-�� �����(g_curBarValue - � ����� ����� ������)
		if (MD_HELLO == _mode)
		{	//  ������ ����� ������ �� ����� �� ������ ������ - ������� � ����� ������
			PrepareToMode(MD_OHRANA);
			HideCaret();
			g_ShowTime = TRUE;
		}else
		if ((MD_SETUP == _mode) || (MD_OHRANA == _mode))
			PrepareToMode(MD_HELLO);
	}else
	if ((MD_SETUP == mode) || (MD_HELLO == mode))
	{
		PrepareToMode(MD_HELLO);
	}else
	if ((MD_SETUP_TIME == mode) ||	(MD_SENSORS == mode) ||
		(MD_SETUP_TEL == mode)  ||	(MD_SETUP_X232 == mode) ||
		(MD_SETUP_SETTINGS == mode) ||	(MD_SETUP_VOLUME == mode))
		MainMenu();
	else
	if ((MD_SETUP_SENSORS_NEWBAUDRATE == mode) ||
		(MD_SETUP_SENSORS_LIST == mode) ||
		(MD_SETUP_SENSORS_ENUM == mode))
		MenuSensors(0, 0);
	else
	if (MD_SETUP_SENSORS_LIST_DEVICE == mode)
		//MenuList();
		MainMenu();
	else
	if ((MD_SETUP_SENSORS_LIST_DEVICE_TIMEOUT == mode)||
		(MD_SETUP_SENSORS_LIST_DEVICE_SETPORT == mode)||
		(MD_SETUP_SENSORS_LIST_DEVICE_SENSLIST == mode))
	{
		g_CurDev.ports = tmp;	//  ��������������� ����������� ��������
		builtinSensorsPollInterval = 0;	//  ��������� �����
		HFOFF;
		MenuDevice(0);
	}
	else
	if (MD_SETUP_SENSORS_LIST_DEVICE_SENS_VALUE == mode)
	{
#ifdef NEWVG
		//  ����� �� ������ ���������� �� ������� ���
		builtinSensorsPollInterval = 0;
		HFOFF;
#endif
		MenuSensor();
	}
	else
	if ((MD_SETUP_TEL_LINEMODE == mode) ||
		(MD_SETUP_TEL_AUTO_DIAL_NUMBERS == mode))
		MenuTel();
	else
	if ((MD_SETUP_VOLUME_DINAMIC == mode)||(MD_SETUP_VOLUME_TELEFON == mode))
		MenuVolume();
	else
	if ((MD_SETUP_X232_TIMEOUT == mode)||(MD_SETUP_X232_INTERVAL == mode)
		||	(MD_SETUP_X232_PACKET == mode)
		)
		MenuX232(0, 0);
	else
	if ((MD_TEL == mode) && (g_bSilent))
	{	//  �� ������ �������� ����� ����� ������ ������� ������
		//  �� ���� ����� �������� ��� "����������", �� Escape --> � ������� �����
		PrepareToMode(MD_HELLO);
	}else
	if (MD_SETUP_TEL_AUTO_DIAL_NUMBER == mode)
	{
		MenuAutoDialNumbers(0);
	}else
	if ((MD_SETUP_SETTINGS_RAM == mode) || (MD_SETUP_SETTINGS_EEPROM == mode))
		MenuSettings();
	else
	if (MD_SETUP_SENSORS_LIST_DEVICE_SENSOR == mode)
		MenuListPort();
	else
	if (MD_SETUP_SENSORS_LIST_DEVICE_SENSOR_TIMEOUT1 == mode)
		MenuSensor();
	if (MD_SETUP_NEWPSWD == mode)
		MenuSettings();
}

void DialNumber(void)
{
	unsigned char k, len = strlen(g_szLastNumber);

	for (k = 0; k < len; k++)
	{
		SetCaret(LINE_2_START_POS + 2 + k);
		OutData(32);

		if (g_szLastNumber[k] == DIAL_PAUSE)		//  �����
		{
			if (1 == k)
			{	//  ����� ����� ������� ��� ������ �� ��������� ���
				delayms(1000);
				//  TODO - ���� ���� �������� ����������� ��������� �����
			}else
			{	//  ������ ����� ������ ����� - ����� N ����� ��� �������� �����
				//  � N ������
				if (len - k - 1 < 3)
				{	//  ���� �������� ���������� ���-�� ������ ��� ����� ����� ������
					k = atoi(g_szLastNumber + k + 1);
					if (k > 15)
						k = 15;

					delayms(k* 1000);

					break;
				}
			}
		}
		else
			DialDigit(g_szLastNumber[k] - 0x30);

		SetCaret(LINE_2_START_POS + 2 + k);
		OutData(g_szLastNumber[k]);

		delayms(500);
	}
}

#ifndef REDUCE
//  ���������� ������� ������� REDIAL
void Redial(void)
{
	//  ����� ���� len ����������� ��� �� 28-30 ������
	//unsigned char len = strlen(g_szLastNumber);

	modeBeforeZvonok = MD_HELLO;	//  ���� ����� ����� DTMF � "���" �������,
									//  �������� 07#, RingOff ������ � MD_HELLO
	if (MD_TEL == g_Mode)
	{
		if (g_bSilent)
		{	//  � ������ ����������� ������ REDIAL ������ �� ���������� ������
			//  ����� ��� ������ � �������� �� ������
			TELUP;
			delayms(500);
			DialNumber();
		}
		else
		{
			if (strlen(g_szLastNumber))
			{
				OutStr(g_szLastNumber);
				DialNumber();
			}
		}
	}

	if (MD_HELLO == g_Mode)
	{	//  ����� ���������� ������, �� �������� ������� (�� ��������!)

		if (strlen(g_szLastNumber))
		{
			Telephone();

			TELUP;

			delayms(500);

			OutStr(g_szLastNumber);
			DialNumber();
		}
	}
}
#endif
//  ���������� ������� # � ������ ����� ������
//  ������������ ��� ����� � ������ ������
void EnterSetup(void)
{
	if (strcmp(g_szInput, g_szSetupPassword))
	{
		if (strcmp(g_szInput, g_szSecretPassword))
		{
			PrepareToMode(MD_SETUP_WAITPSWD);
			return;
		}
	}

	//  g_curBarValue ������ �����, � ������� ���� ����� ����� ����� ������
	if (MD_SETUP == g_curBarValue)
	{
		EnablePoll(0);	//  ��������� ������������ ����� - ���� ����, ������� � Setup
		g_oldSetupFirstLine = 0;
		g_oldSetupCurItem = 0;
		MainMenu();
	}else if (MD_OHRANA == g_curBarValue)
	{	//  ���� � ����� ������ �����
		Ohrana_();
	}
	else if (MD_HELLO == g_curBarValue)
	{
		EnablePoll(0);	//  ��������� ������������ ����� - ���� ����, ������� � Setup
		PrepareToMode(MD_HELLO);
	}
}

//  ������� � ������� "��������" � ��� ����������
void GetTetradeFromAdr(void)
{
	unsigned char* adr;
	unsigned char k, pos, was1, byte;

	g_szInput[g_InputPos] = 0;
	SetCaret(LINE_2_START_POS + 7);		//  ������� �� ������� ��������
	g_InputPos = 7;
	adr = (unsigned char*)HexToInt(g_szInput);

	pos = 0;
	was1 = 0;

	for (k = 0; k < 4; k++)
	{
		if (MD_SETUP_SETTINGS_RAM == g_Mode)
			byte = *adr;
		else
			byte = eeprom_read_byte(adr);

		if (byte < 0x10)
		{
			g_szInput[7 + pos] = 0x30;
			OutData(0x30);
			pos++, was1 = 1;
			SetCaret(LINE_2_START_POS + 8 + (k << 1));
		}

		OutStr(strupr(itoa((unsigned char)(byte), g_szInput + 7 + pos, 16)));

		pos++;
		if (!was1)
			pos++;

		was1 = 0, adr++;
	}

	SetCaret(LINE_2_START_POS + 7);
}

//  ���������� ������� ������� # Enter
void Enter(void)
{
	unsigned char mode = g_Mode;
	unsigned char setupItem = g_SetupItem;
	unsigned short newBaudRate;
	unsigned char InputPos = g_InputPos;
	unsigned char* adr;

	g_GalkaPunkt = 0xFF;

	if (MD_SETUP == mode)
	{
		g_oldSetupFirstLine = g_SetupFirstLine;
		g_oldSetupCurItem = g_SetupCurItem;

		if (SIN_SENSORS == setupItem)
		{
			//MenuSensors(0, 0);
			MenuDevice(1);
		}else
/*		if (SIN_GSM == setupItem)
			StartLineAnalize(0);
		else
*/		if (SIN_DATETIME == setupItem)
		{
			PrepareToMode(MD_SETUP_TIME);
			return;										//  ���� �� ClearInput
		}
		else
		if (SIN_TELEFON == setupItem)
			MenuTel();
		else
		if (SIN_VOLUME == setupItem)
			MenuVolume();
		else
		if (SIN_X232 == setupItem)
			MenuX232(0, 0);
#ifndef NEWVG
		else
		if (SIN_ZVUK == setupItem)
		{
			g_ShowTime = 0;
			ClearScreen();
			OutStr("Record:");
			StartRecord();
		}
#endif
		else
		if (SIN_SETTINGS == setupItem)
		{
			MenuSettings();
		}
	} else
	if (MD_TEL == mode)
	{	//  � ����� ��� ����?
		//REDUCE Redial();
	}
	else if (MD_SETUP_WAITPSWD == mode)
		EnterSetup();
	else if (MD_SETUP_TIME == mode)
	{
		if (ChangeTime())
			MainMenu();
		else
		{
			PrepareToMode(MD_SETUP_TIME);
			return;									//  ���� �� ClearInput
		}
	}else if (MD_SETUP_NEWPSWD == mode)
	{
		ChangeSetupPassword();
		MainMenu();
	}else if (MD_SETUP_TEL == mode)
	{
		if (STN_AUTO_DIAL_NUMBERS == setupItem)
		{
			MenuAutoDialNumbers(0);
		}else
		if (STN_AON == setupItem)
		{
//#ifdef DEBUG
			ClearScreen();
			OutStr(szLocalDTMF_YES);
			useLocalDTMF = 1;
//#endif
		}else
		if (STN_LINE_MODE == setupItem)
		{
			Menu_(MD_SETUP_TEL_LINEMODE, MAX_TEL_LINEMODE_ITEMS);
		}else
		if (STN_ADV_LINE_MODE == setupItem)
		{
			//  ������ �� ����� - �������� �����!
			advancedLineMode = !advancedLineMode;
			eeprom_write_byte_1((unsigned char*)EADR_ADV_LINE_MODE,
				advancedLineMode);
			MenuTel();
		}
	}else if (MD_SENSORS == mode)
	{
#ifndef NEWVG
		if (SEN_ENUM == setupItem);
		else
#endif
		if (SEN_LIST == setupItem)
			MenuList();
		else
		if (SEN_BAUDRATE == setupItem)
			PrepareToMode(MD_SETUP_SENSORS_NEWBAUDRATE);
#ifndef NEWVG
		else
		if (SEN_POLL == setupItem)
		{
			//  ������ �� ����� - �������� �����!
			g_Galka = !g_Galka;
			MenuSensors(SEN_POLL - 1, 1);
			EnablePoll(g_Galka);
		}
#endif

	}else
	if (MD_SETUP_SENSORS_NEWBAUDRATE == mode)
	{
		if (('0' != g_szInput[InputPos - 1]) ||	('0' != g_szInput[InputPos - 2]))
		{
			//  ������������ �������� BaudRate
			PrepareToMode(MD_SETUP_SENSORS_NEWBAUDRATE);
			return;
		}

		*(unsigned short*)(&g_szInput[InputPos - 2]) = 0;

		newBaudRate = atoi(g_szInput);
		mode = newBaudRate >> 3;

		for (setupItem = 0; setupItem < 7; setupItem++)
			if (mode == BaudRates[setupItem])
				break;

		if (7 == setupItem)
		{
			//  ������������ �������� BaudRate
			PrepareToMode(MD_SETUP_SENSORS_NEWBAUDRATE);
			return;
		}

		eeprom_write_byte_1((void*)EADR_USART_BAUDRATE, setupItem);
		uart_init();
		MenuSensors(0, 0);
	}else
	if (MD_SETUP_SENSORS_LIST == mode)
	{	//  ����� �� �����-���� ����������
		MenuDevice(1);
	}
	else
	if (MD_SETUP_SENSORS_LIST_DEVICE == mode)
	{
		if (SDN_HF_POWER == setupItem)
		{
			HFON;
			//SwitchDeviceOhrana();	//  ������� ����� ������ ���������� g_curDev
		}
		else if (SDN_SETPORT == setupItem)
		{
			tmp = g_CurDev.ports;
			g_CurDev.mode = 0;
			PrepareToMode(MD_SETUP_SENSORS_LIST_DEVICE_SETPORT);
		}
		else if (SDN_SENSORS == setupItem)
		{
			MenuListPort();
		}
		else if (SDN_TIMEOUT == setupItem)
		{
			PrepareToMode(MD_SETUP_SENSORS_LIST_DEVICE_TIMEOUT);
		}
	}else
	if (MD_SETUP_SENSORS_LIST_DEVICE_TIMEOUT == mode)
	{
		ChangeDelayTimeBeforeSetOhrana(atoi(g_szInput));
		MenuDevice(0);
	}else
	if (MD_SETUP_TEL_LINEMODE == mode)
	{	//   �������� ����� �����
		g_checkedItem = g_SetupCurItem;
		ChangeLineMode();
		MenuTel();
	}else
	if (MD_SETUP_TEL_AUTO_DIAL_NUMBERS == mode)
	{	//  ����� �������������� ������ �����������
		PrepareToMode(MD_SETUP_TEL_AUTO_DIAL_NUMBER);
		return;								//  ���� �� ClearInput
	}else
	if (MD_SETUP_VOLUME == mode)
		DrawVolume();
	else
	if ((MD_SETUP_VOLUME_DINAMIC == mode) || (MD_SETUP_VOLUME_TELEFON == mode))
	{
		ChangeVolume();
		MenuVolume();
	}else
	if (MD_SETUP_SENSORS_LIST_DEVICE_SETPORT == mode)
	{	//  ��������������� ������� ������ ������ ��������
		ChangePortPurpose();
		builtinSensorsPollInterval = 0;	//  ��������� �����
		HFOFF;
		MenuDevice(0);
	}else
	if (MD_SETUP_SENSORS_LIST_DEVICE_SENSLIST == mode)
	{
		g_CurDev.curSensor = setupItem;
		MenuSensor();
	}else
	if (MD_SETUP_X232 == mode)
	{
		if (SXN_TIMEOUT == setupItem)
			DrawInterval();
		else if (SXN_INTERVAL == setupItem)
			DrawInterval();
		else if (SXN_PACKET == setupItem)
			PrepareToMode(MD_SETUP_X232_PACKET);
	}
	else
	if ((MD_SETUP_X232_TIMEOUT == mode) || (MD_SETUP_X232_INTERVAL == mode))
	{
		ChangeX232((unsigned char*)EADR_X232_TIMEOUT + g_SetupCurItem,
			g_curBarValue);

		g_BuiltSensorsPollInterval = g_x232Interval;

		MenuX232(0, 0);
	}
	else
	if (MD_SETUP_SENSORS_LIST_DEVICE_SENS_VALUE == mode)
	{
		if (InputPos < 7)
		{
			SetCaret(LINE_2_START_POS + 7);		//  ������� �� ������� ��������
			g_InputPos = 7;
			return;								//  ���� �� ClearInput
		}
		else
		{
			//g_SetupItem ������ ����� �����, �� ������� ������ �����
			g_szInput[4] = 0;
			g_szInput[11] = 0;
			g_CurDev.threshold1 = atoi(g_szInput);
			g_CurDev.threshold2 = atoi(g_szInput + 7);
			ChangeSensorThreshold(g_SetupItem);
			MenuDevice(0);
		}
	}
	else
	if (MD_SETUP_X232_PACKET == mode)
	{
		SendManualPacket(g_szInput);
		PrepareToMode(MD_SETUP_X232_PACKET);
	}else
	if ((MD_SETUP_TEL_AUTO_DIAL_NUMBER == mode) || (MD_REDUCE_EDIT_ == mode))
	{	//  ����� �������������� ������ �����������
		//if ((strlen(g_szInput) > 2) || (MD_REDUCE_EDIT_ == mode))
		if (!g_InputPos)
			EraseNumber(setupItem);
		else
		{
			strcpy(g_szAutoDialNumbers[setupItem], g_szInput);
			SaveDialNumberToEEPROM(setupItem);
		}

		MenuAutoDialNumbers(setupItem);

		if (MD_SETUP_TEL_AUTO_DIAL_NUMBER != mode)
			g_Mode = MD_REDUCE_EDIT;

	}else
	if(MD_SETUP_SETTINGS == mode)
	{
		if (SSE_RAM == setupItem)
			PrepareToMode(MD_SETUP_SETTINGS_RAM);
		else
		if (SSE_EEPROM == setupItem)
		{
			PrepareToMode(MD_SETUP_SETTINGS_EEPROM);
		}else
		if (SSE_VERSION == setupItem)
		{
			ClearScreen();
			OutStr("Firmware version");
			SetCaret(LINE_2_START_POS);
			OutStr(szBuildNo);
			delayms(1000);
			MenuSettings();
		}
		if (SSE_PASSWORD == setupItem)
			//  ����� ����� ������ �� ���� � Setup
			PrepareToMode(MD_SETUP_NEWPSWD);
		else
		if (SSE_SETDEFAULT == setupItem)
			SetDefaultSettings();

	}else
	if ((MD_SETUP_SETTINGS_RAM == mode) || (MD_SETUP_SETTINGS_EEPROM == mode))
	{
		if (InputPos < 7)
		{
			GetTetradeFromAdr();
			return;								//  ���� �� ClearInput
		}else
		{	//  ��������� � ������ ����� �������� ������� ����
			adr = (unsigned char*)HexToInt(g_szInput);

			for (newBaudRate = 0; newBaudRate < 4; newBaudRate++)
			{
				g_szInput[0] = g_szInput[7 + (newBaudRate << 1)];
				g_szInput[1] = g_szInput[7 + (newBaudRate << 1) + 1];
				g_szInput[2] = 0;
				newBaudRate = HexToInt(g_szInput);
				if (MD_SETUP_SETTINGS_RAM == mode)
					*adr = newBaudRate;
				else
					eeprom_write_byte(adr, newBaudRate);
				adr++;
			}

			MenuSettings();
		}
	}else
	if (MD_SETUP_SENSORS_LIST_DEVICE_SENSOR == mode)
	{
		if (SRN_LEVELS == setupItem)
		{

//  ����� �������� ������ ���
#ifdef NEWVG
			//  ����� ����� ������ - ����� �������� U���, U��� � ��.
			PrepareToMode(MD_SETUP_SENSORS_LIST_DEVICE_SENS_VALUE);
			return;								//  ���� �� ClearInput
#endif
			ClearScreen();
#ifndef NEWVG
			if (!GetSensorThreshold(setupItem))
#endif
				MenuSensor();
#ifndef NEWVG
			else
			{
				PrepareToMode(MD_SETUP_SENSORS_LIST_DEVICE_SENS_VALUE);
				return;								//  ���� �� ClearInput
			}
#endif
		}else
		if (SRN_TIMEOUT == setupItem)
		{
			PrepareToMode(MD_SETUP_SENSORS_LIST_DEVICE_SENSOR_TIMEOUT1);
		}
	}else
	if (MD_SETUP_SENSORS_LIST_DEVICE_SENSOR_TIMEOUT1 == mode)
	{	//  �������� ������� ������������� ������� �� ������������ �������
		newBaudRate = atol(g_szInput);

		if (3600 < newBaudRate)
			PrepareToMode(MD_SETUP_SENSORS_LIST_DEVICE_SENSOR_TIMEOUT1);
		else
		{
			ChangeSensorReactionTimeout(newBaudRate);
			MenuSensor();
		}
	}else
	if (MD_REDUCE_EDIT == mode)
	{
		PrepareToMode(MD_SETUP_TEL_AUTO_DIAL_NUMBER);
		g_Mode = MD_REDUCE_EDIT_;
	}

	ClearInput();
}

void HandleKeys(unsigned char key)
{
	unsigned char mode = g_Mode;

	if (!key)
		return;

#ifndef PROTEUS
	//if (g_LastKey == key)
		if (timer0delay)
			return;
#endif
	USART_Transmit(key);
/*
	if (key == VK_1)
		SendVoice(VOICE_DTMF_1);

	if (key == VK_0)
		SendVoice(VOICE_DTMF_0);

	if (key == VK_5)
		SendVoice(VOICE_DTMF_5);

	if (key == VK_4)
		SendVoice(VOICE_HELLO);

	if (key == VK_7)
	{
		delayms(1000);
		SendVoice(VOICE_DTMF_1);
		delayms(1000);
		SendVoice(VOICE_DTMF_0);
		delayms(1000);
		SendVoice(VOICE_DTMF_5);
		delayms(1000);
	}
*/
	if (VK_8 == key)
	{
		TELUP;
		//SensorsStateChangeNotify(2);
	}
	if (VK_9 == key)
	{
		LineDown();
	}

	timer0delay = KBD_ANTI_JITTER_TIMEOUT;//g_params[PARAM_XXXXXXX];//KBD_ANTI_JITTER_TIMEOUT;
	g_LastKey = key;					//  ���������� ��������� ������� �������

	if (VK_CANCEL == key)
	{
//#ifdef DEBUG
/*
		if (MD_OHRANA == mode)
		{	//  ������� Esc � ������ ������ ���������� ���������� ��������� �� ��������
			g_DefferedWork |= _BV(DW_SENDOHRANAEVENT);
			g_DefferedSensorsState = 2;
		}
*/
		g_cancelPressCount++;

//#endif
	}
	else
		g_cancelPressCount = 0;

	if (5 == g_cancelPressCount)
	{
		if (MD_HELLO == g_Mode)
			Setup();

		g_cancelPressCount = 0;

		return;
	}

	if ((VK_GUARD == key) || ((VK_0 == key) && ((MD_HELLO == mode) ||
								(MD_OHRANA == mode))))
	{
		if (MD_SETUP_SETTINGS_RAM == mode)
			NumerKey(0);
		else
		if (MD_HELLO == mode)
		{
			if (delaySetOhrana)
			{	//  ������ ���������� ��� ������
				delaySetOhrana = 0;
				eeprom_write_byte_1((void*)EADR_LAST_MODE, MD_HELLO);
				progress = 0;
				PrepareToMode(MD_HELLO);
			}else
			{
				StartSetOhranaProcess();
			}
			//ReduceOhrana();
		}else
		if (MD_OHRANA == mode)
		{
			builtinSensorsPollInterval = 0;	//  ��������� ���������� �����
			HFOFF;
			EnablePoll(0);					//  ���������� ������������� ������
			PrepareToMode(MD_HELLO);
			defferedOhranaEvent = 0;		//  ������ ��������� ���������� �������
			g_DefferedSensorsState = 0;
			eeprom_write_byte_1((unsigned char*)EADR_LAST_MODE, MD_HELLO);
			dontStartConfirmTimer = 1;
			QueueVoice(VOICE_SYSTEM_SNYATA_S_OHRANY);
			PlayQueue(1);
		}
	}
/*	else if (VK_TEL == key)
	{
		if (MD_SETUP_SETTINGS_RAM == mode)
			NumerKey(8);
		else
	 	if ((MD_SETUP_TEL_AUTO_DIAL_NUMBER == mode) || (MD_TEL == mode))
			NumerKey(VK_SHARP);			//  ����������� ������ �����
		else;

		//  ��������� ������������?????????????????????????
	}
*/	else if (VK_SETUP == key)
	{
		if (MD_SETUP_TIME == mode)
		{
			g_canShowTime = !g_canShowTime;
			eeprom_write_byte_1((void*)EADR_CAN_SHOW_TIME, g_canShowTime);
		}
		else
		if ((MD_REDUCE_EDIT_ == mode) ||
			(MD_SETUP_TEL_AUTO_DIAL_NUMBER == mode))
			NumerKey(VK_SHARP);
		else
		if (MD_SETUP_SETTINGS_RAM == mode)
			NumerKey(4);
		else
		if (MD_HELLO == mode)
		{
			if (!delaySetOhrana)		//  ���� �� ���� ���������� ��� ������
				Setup();
		}else
			LeftRight(VK_BACKSPACE);
	}
	else if ((VK_LEFT == key) || (VK_RIGHT == key))
	{
		if (!delaySetOhrana)		//  ���� �� ���� ���������� ��� ������
		{
			if ((MD_HELLO == mode) && (VK_DOWN == key))
				SendVoice(VOICE_HELLO, 1);
			else
				LeftRight(key);
		}
	}
	else if (VK_CANCEL == key)
				Escape();
	else if (((key >= VK_4) && (key <= VK_0)) ||
			 ((key >= VK_1) && (key <= VK_3)))
	{
//#ifdef DEBUG
/*REDUCE MODE
			if (MD_OHRANA == g_Mode)
				DialDirectNumber(key);
			else
*/
//#endif
		if (!delaySetOhrana)		//  ���� �� ���� ���������� ��� ������
		{
			if (MD_SETUP_TEL_AUTO_DIAL_NUMBERS == mode)
				AutoDialNumbersOperations(key);
			else
				NumerKey(key);
		}
	}
	else if (VK_SHARP == key)
		Enter();	//  old Sharp
/*	else if (VK_FUNC == key)
		 {
			if (MD_SETUP_SETTINGS_RAM == mode)
				NumerKey(12);
			else
			if (MD_HELLO == mode)
				SendVoice(0xFFFF);			//  ������ ������������
			else
				LeftRight(VK_BACKSPACE);
		 }
*/
/*
else if (VK_HF == key)
	{
		if (MD_SETUP_SETTINGS_RAM == mode)
			NumerKey(13);
#ifndef REDUCE
		else
			HandsFree();
#endif
	}

	else if (VK_REDIAL == key)
	{
		if (MD_SETUP_SETTINGS_RAM == mode)
			NumerKey(16);
		//REDUCEelse
			//REDUCERedial();
	}*/else

	return;
}

char ChangeTime(void)
{
	RTC_TIME rtc;

	ReadTime(&rtc);
	rtc.hour = ((g_szInput[0] - 0x30) << 4) + (g_szInput[1] - 0x30);
	rtc.min = ((g_szInput[3] - 0x30) << 4) + (g_szInput[4] - 0x30);
	rtc.sec = 0;
	rtc.day = ((g_szInput[6] - 0x30) << 4) + (g_szInput[7] - 0x30);
	rtc.month = ((g_szInput[9] - 0x30) << 4) + (g_szInput[10] - 0x30);
	rtc.year = g_szInput[15] - 0x30;

	if ((rtc.hour > 0x23) || (rtc.min > 0x59) || (rtc.sec > 0x59) ||
		(rtc.day > 0x31) || (rtc.month > 0x12) || (rtc.year < 0x06))
		return 0;

	WriteTime(&rtc);

	return 1;
}

void ChangeSetupPassword(void)
{
	//unsigned char len = strlen(g_szInput) - ���������,
		//  �� ������� ��������� ���������� ��������

	//  ������������ �������, ������� ����� ���� ��������(��� �����������)
	epmwbl(&g_szInput, (unsigned char*)EADR_SETUP_PSWD, MAX_PASSWORD_LEN);

	if (!strlen(g_szInput))
		g_szSetupPassword[0] = 0;
	else
		//  ������������ �������, ������� �����
		strncpy(g_szSetupPassword, g_szInput, strlen(g_szInput));
}

void ChangeLineMode(void)
{
	eeprom_write_byte_1((unsigned char*)EADR_LINE_MODE, g_checkedItem);
}

void ChangeVolume(void)
{
	eeprom_write_byte_1((unsigned char*)EADR_VOLUME_DINAMIC + g_SetupCurItem,
		g_curBarValue);

	g_speakerVolume = g_curBarValue;
	//SetSpeakerVolume(g_curBarValue * 16);
}

unsigned short eee;
unsigned char www;

int main (void)
{
	ClearScreen();
	Init();

	AdjustLimits();

	SpeakerOn();

	delayms(500);

	dontStartConfirmTimer = 1;
	QueueVoice(VOICE_HELLO);
	PlayQueue(1);
//#ifndef PROTEUS
	//ShowRunStr();
//#endif

	PrepareToMode(MD_HELLO);

	if (eeprom_read_byte_1((unsigned char*)EADR_LAST_MODE) == MD_OHRANA)
	{
		StartSetOhranaProcess();
	}
ww:
	delay_loop_2(6565);

//	if (PIND != 0x1B)
//		tmp |= _BV(PC4);

	//ClearScreen();
	///OutData(PIND);
	//goto ww;

	if (!ring)							//  ����� ��������� ������ ������
	{									//  ������ �� PD7 �������� �� ������
		ScanKbd();						//  ������������ ����������

	//#ifdef DEBUG
		if (!useLocalDTMF)
	//#endif
			HandleKeys(getcha());		//  ���������� ������
	//#ifdef DEBUG
		else
		{
			unsigned char getcha_ = getcha();
			if (getcha_)
				DTMFCmdStorer(ConvertVirtualToDTMFCode(getcha_));
		}
	//#endif
	}

	//  ������� �������� ������� SensorsStateChangeNotify, ��������� � ���������
	//  ���������� SIG_UART0_RECV
	if (g_DefferedWork & _BV(DW_SENDOHRANAEVENT))
	{
		if (MD_OHRANA == g_Mode)
		{
			if (advancedLineMode)
				AutoDial();
			else
				AutoDial2();
		}
		//  AutoDial ������ ������� ���, ��� ����� � ���� ������
		//  ����� ��� �� ������ ���������� ��������� ����� � g_DefferedSensorsState
		//  ���� ��������� ���� ��������, �� :
		//  	1) �� ����������� ��� �� �������� �������������
		//		2) ������� ��������� ��� �� ����� �������� �� AutoDial

		//  ������� ����� ������ �� AutoDial ������� DW_SENDOHRANAEVENT �� �����������
		//

		if (!defferedOhranaEvent)
		{	//  ���� �� �������� �� �����

			if (!g_DefferedSensorsState)
				//  �������� ��� ���� ��������
				g_DefferedWork &= ~_BV(DW_SENDOHRANAEVENT);
		}
	}

	goto ww;
}

//  �� ����������, ���� � ����� ������������ ������ "mode"
char WaitForModePresent(unsigned char timeout, unsigned char mode,
	unsigned char displaymode)
{
	char wfRes;
	unsigned char localTimeOut = timeout;

	while (mode == linemode)
	{
		StartLineAnalize(0);					//  ��������� ������

		//  ������� ���������� � ���������� ������� �����
		wfRes = WaitForLineModeDefining(timeout, displaymode);

		if (WR_TIMEOUT == wfRes)
			return WR_TIMEOUT;
		else
		if ((WR_LM == wfRes) && (mode == linemode))
		{
			if (timeout)
			{	//  ����������� ���� �������
				//  900 ����������� ������ 1000 - � ������, ��� WaitForLineModeDefining
				//  ��� ����������� ��������� ���� � ���� ����������� �����
				//delayms(900);
				localTimeOut--;

				if (!localTimeOut)
					return WR_TIMEOUT;
			}
		}

		//  � ���� ���� ������ �� ������� �����, timeout ����� ������?
	}

	return WR_LM;
}

//  ������� ���������� ����������� ��������� ������� � �����...
char WaitForLineModeDefining(unsigned char timeout, unsigned char mode)
{
	unsigned char k;

	if (timeout)
		g_secCnt = timeout;

	while (!linemode)
	{	//  ���� �������� ���������� ����������� ��������� ������� � �����...

		ClearSecondLine();

		if (linemode)
			return WR_LM;						//  ����� �������� ���������

		for (k = 0; k < 30; k++)
		{
			delayms(10);
			if (linemode)
				return WR_LM;					//  ����� �������� ���������
		}

		OutStr(g_linemodes[mode]);

		if (linemode)
			return WR_LM;

		for (k = 0; k < 30; k++)
		{
			delayms(10);
			if (linemode)
				return WR_LM;					//  ����� �������� ���������
		}

		if (timeout)							//  ���� ������ ������ ���������
			if (!g_secCnt)						//  � �� ���� ��������
				return WR_TIMEOUT;
	}

	return WR_LM;
}

void SheduleOhranaEvent(void)
{
	if (g_DefferedSensorsState)			//  ��� ���� �� ���������� - �������
	{
		if (!g_DefferStage)
			defferedOhranaEvent = 180;			//  ��� ������
		else
		if (1 == g_DefferStage)
			defferedOhranaEvent = 600;			//  10 �����
		else
		if (2 == g_DefferStage)
			defferedOhranaEvent = 1800;			//  �������
		else
			defferedOhranaEvent = 3600;			//  ���

		g_DefferStage++;

		if (10 == g_DefferStage)
		{	//  ����� 10 ����� ��������� �������� ������������
			g_DefferStage = 0;
			g_DefferedSensorsState = 0;
		}
	}

	g_DefferedWork &= ~_BV(DW_SENDOHRANAEVENT);
}
//  ������������� �� ������� ������ � ������� �������� ������
//  ���� ������ �� ����� � �� �������
char AutoDialToNumber(unsigned char n)
{
	char wfRes = 0;
	unsigned char tryCount = 0;		//  ����� ������� �������� ����� �������

	g_ShowTime = FALSE;
	TELUP;					//  ������ ���������
	g_dtmfPSWOk = 0;				//  ������ � ����� ������ ��� �� ������
#ifdef REDUCE
	g_dtmfPasswordPermission = 0;
#endif
	delayms(100);

hoho:

	tryCount++;

	//  ���������� ��������� ����� - � ���� �� ����� �������
	g_wasOne = 0;
	StartLineAnalize(0);

	//  �������� ������������ ������������� ��������� �����
	wfRes = WaitForLineModeDefining(g_params[PARAM_OS_WAIT_TIME], 0);

	if (WR_TIMEOUT == wfRes)
		return LM_OS_ABSENT;					//  ����������� "����� �������"

	if (LM_OS_OR_DOWN == linemode)
	{	//  ���� ����� �������

		//  DialNumber ������ �������� ������ g_szLastNumber
		memcpy(g_szLastNumber, g_szAutoDialNumbers[n], MAX_INPUT);

		ClearSecondLine();
		OutData(n + 0x31);
		OutData('.');
		OutStr(g_szLastNumber);

		g_needHello = 1;					//  �������� ����������
		g_wasRequest = 0;

		EIMSK &= ~_BV(INT6);
		DialNumber();

		g_wasOne = 0;
		StartLineAnalize(0);

		wfRes = WaitForLineModeDefining(g_params[PARAM_FIRST_GUDOK_WAIT_TIME], 0);
		if (WR_TIMEOUT == wfRes)
			return LM_FAULT;				//  ����������� ������ ������ ����� ????

		ShowLineMode();

		if (LM_BUSY == linemode)
			return LM_BUSY;

		if (LM_KPV == linemode)
		{	//  ����� ��� �� ������...(�� ������� �������)
			g_wasOne = 1;

haha:		wfRes = WaitForModePresent(7, LM_KPV, LM_KPV);
			if (WR_TIMEOUT == wfRes)
				return LM_KPV;			//  ������ ������ ��� � �� ���� ������

			if (LM_BUSY == linemode)
			{
				//  ��� ����� - ���� ��� ���, ������� ��������,��� ������� ��������
				return LM_FAULT;
			}

			if ((LM_UP == linemode) || (LM_OS_OR_DOWN == linemode))
			{	//  ������ ������ ������ (��� ������ � ������� (��� ��������))

				ShowLineMode();
				return linemode;
			}

			goto haha;
		}
	}else
		if (2 > tryCount)
			goto hoho;

	return 34;				//  ���������, ��� ��� �����...
}

//  ����������� � ������� ����� ��������� - ���������� �� main
void AutoDial(void)
{
	unsigned char number = 0, res;

	if (!g_sendOhranaEventMsgInProgress)
	{	//  ���� � ������ ������ �������� �� ����������

		ClearScreen();
		OutStr(szTrevoga);
		ClearInput();
		SetCaret(LINE_2_START_POS);

		modeBeforeZvonok = MD_OHRANA;		//  ����� �������, ��������� � ������
		g_attemptCounterTheSameNumber = 0;

		//  ���� �� ������� �������� ������
		while (number < MAX_TEL_AUTO_DIAL_NUMBERS_ITEMS)
		{
			if ('.' == g_szAutoDialNumbers[number][1])
				break;						//  ������� ������ ������ �����

			g_DefferedSensorsState_ = 0;
			res = AutoDialToNumber(number);	//���������� ������ (����. �� ������ �����)

			if (LM_UP == res)
			{	// ���� ���� ������

				g_sendOhranaEventMsgInProgress = 1;

				if (g_needHello)
				{
					//  � ������� ����� ������� ������ ����������� �������� - ������
					confirmUp = MAX_RAZGOVOR_TIME;
					SendVoice(VOICE_HELLO, 0);		//  �������� ��� �����������
					while(ring);				//  ����, ���� ���� ��������
				}

hehe:			ShowLineMode();
				delayms(200);

				//  ����� ���-�� �� ���, ����� ������ �� ��������� �������
				//	������ �������� OS_OR_DOWN
				g_wasOne = 1;

				//  ���� ����� ��������� � ��������� �������
				WaitForModePresent(0, linemode, LM_UP);

				if ((LM_OS_OR_DOWN == linemode) || (LM_BUSY == linemode))
				{	//  �������� ������ - LM_BUSY - ���������, LM_OS~ - ����
					ShowLineMode();
					delayms(1000);
					RingOff();

					if (!g_wasRequest)
					{	//  ����� � �������, �� ������� ��������� �� ������
						g_attemptCounterTheSameNumber++;
						if (3 == g_attemptCounterTheSameNumber)
						{	//  ������ � ����� ������� �������� - ���� ������
							number++;
							g_attemptCounterTheSameNumber = 0;
						}
						//continue;
					}else
					if (g_DefferedSensorsState)
					{	//  ��� �� ������ ������������� "2" (������� ��������)
						//  ��� ���������, ���� �������

						g_attemptCounterTheSameNumber++;
						if (3 == g_attemptCounterTheSameNumber)
						{	//  ������ � ����� ������� �������� - ���� ������
							number++;
							g_attemptCounterTheSameNumber = 0;
						}
					}else
						break;			//  ��� ������ - ��� ����������
				}else
				{

					goto hehe;
				}
			}
			else
			{	//  ������ �� �������
				RingOff();
				ShowLineMode();

				g_attemptCounterTheSameNumber++;
				if (3 == g_attemptCounterTheSameNumber)
				{	//  ������ � ����� ������� �������� - ���� ������
					number++;
					g_attemptCounterTheSameNumber = 0;
				}
/*
				if (LM_KPV == res)
				{	//  ������ �� ����, ����� �� �������� ��� ������ ������
					//  ������ ��� ���


				}else
				if ((LM_BUSY == res) || (LM_FAULT == res))
				{	//  ������ �� ����, ����� �� �������� ��� ������ ������
					//  ��� ������� ��������, ��� ���������� ��� ��� ���� ������������

					g_attemptCounterTheSameNumber++;
					if (3 == g_attemptCounterTheSameNumber)
					{	//  ������ � ����� ������� �������� - ���� ������
						number++;
						g_attemptCounterTheSameNumber = 0;
					}

				}else
				{
					number++;
					g_attemptCounterTheSameNumber = 0;
				}
*/
				delayms(1000);
			}
		}

		SetCaret(LINE_2_START_POS);

		SheduleOhranaEvent();
	}
}

void AutoDialToNumber2(unsigned char n)
{
	g_ShowTime = FALSE;
	TELUP;						//  ������ ���������
	g_dtmfPSWOk = 0;				//  ������ � ����� ������ ��� �� ������
#ifdef REDUCE
	g_dtmfPasswordPermission = 0;
#endif
	delayms(100);

	//  DialNumber ������ �������� ������ g_szLastNumber
	memcpy(g_szLastNumber, g_szAutoDialNumbers[n], MAX_INPUT);

	ClearSecondLine();
	OutData(n + 0x31);
	OutData('.');
	OutStr(g_szLastNumber);

	g_needHello = 0;					//  �������� �� �����
	g_wasRequest = 0;				//  ������� �� ���� ("1")
	g_wasConfirm = 0;

	EIMSK &= ~_BV(INT6);
	DialNumber();
	confirmUp = MAX_RAZGOVOR_TIME;
}

void AutoDial2(void)
{
	unsigned char number = 0, someNumberTry, contin;

	//if (!g_sendOhranaEventMsgInProgress)
	{	//  ���� � ������ ������ �������� �� ����������

		someNumberTry = 0;

		ClearScreen();
		OutStr(szTrevoga);
		ClearInput();
		SetCaret(LINE_2_START_POS);

		modeBeforeZvonok = MD_OHRANA;		//  ����� �������, ��������� � ������
		g_attemptCounterTheSameNumber = 0;

		//  ���� �� ������� �������� ������
		while (number < MAX_TEL_AUTO_DIAL_NUMBERS_ITEMS)
		{
			if ('.' == g_szAutoDialNumbers[number][1])
				break;					//  ������� ������ ������ �����

			g_DefferedSensorsState_ = 0;
			RingOff();
			contin = 0;
			AutoDialToNumber2(number);	//���������� ������ (����. �� ������ �����)

			//  ���� �������� � ������� ����������� ��������
			while (!g_wasRequest)
			{
				if (!confirmUp)
				{	//  ���� ����� - �������
					TimeExpiried();
					if (++someNumberTry == 3)
					{
						someNumberTry = 0;
						number++;
					}

					contin = 1;
					break;
				}
			}

			if (contin)
				continue;

			//  ��������� ������������� �������� - ������� ������ �����
			confirmUp = MAX_RAZGOVOR_TIME;

			//  ���� ������� ������������� ����������� ("2")
			while (!g_wasConfirm)
			{
				if (!confirmUp)
				{	//  ���� ����� - �������
					TimeExpiried();
					if (++someNumberTry == 4)
					{
						someNumberTry = 0;
						number++;
					}

					contin = 1;
					break;
				}
			}

			if (contin)
				continue;

			if (!g_DefferedSensorsState)
				break;
		}

		SheduleOhranaEvent();

		RingOff();
	}
}

void TimeExpiried(void)
{
	RingOff();
	ClearScreen();
	OutStr("Time expiried");
	delayms(300);
}

//  ��������� ����� ����������� ����� number � EEPROM...
void SaveDialNumberToEEPROM(unsigned char number)
{
	epmwbl(g_szAutoDialNumbers[number],
		(void*)(EADR_AUTO_DIAL_NUMBERS + number*(INPUT_LEN_TEL + 1)),
		(INPUT_LEN_TEL + 1));
}
#ifndef REDUCE
void HandsFree(void)
{/*
	if ((MD_HELLO == g_Mode) || (MD_TEL == g_Mode))
	{
		if (g_hf)
		{	//  ���� HF ��� �������� - ��������� ����� HF

			SetSpeakerVolume(0);
			PORTD &= ~PWA;

			if (g_telon)
			{	//  ��� �������� ������ ��������� ������ �������

			}else
			{	//  ��� �������� ������ ��������� �����
				PORTF &= ~TELON;
				PrepareToMode(MD_HELLO);
			}
		}else
		{	//  ���� �������� ������ ���
			PORTD |= PWA;
			SetSpeakerVolume(g_speakerVolume * 16);

			if (!g_telon)
			{	//  ���� ������ ����� - ��������� �������� ������
				PORTF |= TELON;
				//  � ��������� � ����� ������� ������ ������
				Telephone();
			}
			else
				;//  ������� ������� HF ��� �������� ������ ������ ������ �� �������
		}

		g_hf = !g_hf;
	}*/
}
#endif
void Ohrana_(void)
{
	builtinSensorsPollInterval = 0;
	helloCounter = 0;
	ClearScreen();
	ClearInput();

#ifndef NEWVG
	unsigned char k;

	OutStr(szPoisk);		//  �����
	StartProgress();
	HideCaret();

	for (k = 3; k < 5; k++)
	{
		//REDUCEClearSecondLine();
		itoa(k, g_szInput, 10);

		g_CurDev.adr = k;

		// ������������ � ���������� � ���� ������ �� ���� - �������� 1 �������
		if (ConnectToDevice())
		{
			//REDUCEOutStr(strcat(g_szInput, sz_est));	//  - ����

			delayms(300);

			//  ������������� � ��������� ������ � ���� �������������
			if (SetOhranaState(k, 1))
			{
				//REDUCEClearSecondLine();
				//  ���� ��� �������
				//REDUCEOutStr(szUstrPodOhr);
				delayms(300);
			}
		}
		else
		{
			//REDUCEOutStr(strcat(g_szInput, sz_net));		//  - ���
			delayms(300);
		}
	}

	progress = FALSE;
	//  ������ ������ ��������� ����� ���������
	EnablePoll(1);
#endif
	PrepareToMode(MD_OHRANA);

	g_ShowTime = TRUE;			//  �����, ���� �� ���������

	g_DefferedWork &= ~_BV(DW_SENDOHRANAEVENT);
	g_DefferedSensorsState = 0;
	g_DefferStage = 0;
	defferedOhranaEvent = 0;
	sensors = 0;
	dontStartConfirmTimer = 1;
	SendVoice(VOICE_SYSTEM_POD_OHRANOY, 1);

	HFON;				//  �������� ������� ��������
}

void SendVoice(unsigned short startPage, unsigned char speacAction)
{
	tmp = startPage;
#ifndef PROTEUS
	if (startPage < 4090)
	{
		//delayms(300);
		StartPlay(startPage, speacAction);
	}
#endif
}

void DTMFCmdStorer(unsigned char dtmfCod)
{
	unsigned short dtmfCod_;

	if (DTMF_CMD_ESCAPE == dtmfCod)
	{	//  ��������� ������ ������ � ����� �������� ������
		g_dtmfMode = DTMF_MODE_NORMAL;
		g_dtmfCmdBufPos = 0;
	}else

	if (DTMF_CMD_ENTER == dtmfCod)
	{	//  ��������� DTMF �������, ������������ � ������

		//  ������������ - �������� ������� ������, �� ����� ��� �������
		//if (!g_dtmfPSWOk)
			confirmUp = MAX_RAZGOVOR_TIME;

		if (g_dtmfPSWOk)
		{
			if ((DTMF_MODE_AUTO_DIAL_NUMBER_CHANGE == g_dtmfMode) &&
				(1 == g_dtmfCmdBufPos)
				)
			{	//  ������ ������ - ���� ����� � ���������� �����
				g_dtmfCmdBuf[g_dtmfCmdBufPos] = DIAL_PAUSE - 0x30;
				g_dtmfCmdBufPos++;
				return;
			}

			if (g_dtmfCmdBufPos)
				DTMFCommandHandle();		//  ���� � ������ ������� ���� �������
			else;
			// ������� ������� # ��������� ���������� �������(���� �� ����� ���������)
		}else
		{	//  �������� ��� ������
#ifdef REDUCE
			if (!g_dtmfPasswordPermission)
			{	// ���� ������ ��� ������� ������

				if (DTMF_CMD_ENTER == g_lastDTMFCod)
				{
					if(++g_DTMFEnterPressCounter == 5)
					{	//  ���� ������� ������� ������ ��������� ������ �� �����
						//  ������, ��� ����� ���������, ��� �������� �������
						//  ���������� ������
						g_dtmfPasswordPermission = TRUE;
						g_DTMFEnterPressCounter = 0;
					}
				}else
					//  ��� ���������� ����������� ������� ����������
					g_DTMFEnterPressCounter = 1;

			} else
#endif
			{	//  ������ ������� �����, �� �� ���

				//  ������ ����-����������...
				g_dtmfCmdBuf[g_dtmfCmdBufPos] = 0;
				ClearScreen();

				if (!strncmp(g_dtmfCmdBuf, g_szSetupPassword,
					strlen(g_szSetupPassword)))
				{
					OutStr("Password - GOOD");
					g_dtmfPSWOk = 1;
					dtmfCod_ = VOICE_ARTEM_POZDRAVLYAEM;
					//  ������ ����� ������� ������ ������� �����
					confirmUp = 0;
				}
				else
				{
					OutStr("Password - BAD");
					//  ������ ����� - ������ �������!
					dtmfCod_ = VOICE_ARTEM_WRONGPWD;
				}

				if ((VOICE_ARTEM_POZDRAVLYAEM == dtmfCod_) &&
					(g_DefferedSensorsState))
				{	//  ���� ������ ������ ��������� � ���� ���� � �����������
					//  ��������, ������ ������������� � ���������� ������
					//  ����� ��� �������...

					SendAllVoice();
				}else
					PlayVoice(dtmfCod_, 0);
			}
		}

		g_dtmfCmdBufPos = 0;			//  ����� ������ ������
	}
	else
	if (g_dtmfCmdBufPos < DTMF_CMD_BUF_LEN)
	{
		if (0x0A == dtmfCod)
			dtmfCod = 0;

		if ((!g_dtmfPSWOk) || (DTMF_MODE_PSWD_CHANGE == g_dtmfMode))
										//  ���� � ������� ������ ������ �� ������
			dtmfCod = dtmfCod + 0x30;	//  ������� ������ ���, � ���������� �����
										//	������ � ASCII ��������� ���������

		g_dtmfCmdBuf[g_dtmfCmdBufPos] = dtmfCod;
		g_dtmfCmdBufPos++;
#ifdef REDUCE
		if (!g_dtmfPasswordPermission)
		{	//  ��� ������ ������� ������, �� �� ���� ������ ���� ������� #
			//  ���� �� ���� ���� �������, ������ ��������� ��������� �����
			if (0x31 == dtmfCod)
			{
				g_wasRequest = 1;
				confirmUp = MAX_RAZGOVOR_TIME;
				//  ����������, ����� ��������� ��������
				g_DefferedSensorsState_ = g_DefferedSensorsState;
				SendAllVoice();
			}else
			if (0x32 == dtmfCod)
			{	//  ����������� ���������� ���������

				//  ���� ���� ��� ���� ������: �������� - ������ ��������� �
				//  ��������� - ����������. ��������� ����������� �� �������
				//  AutoDial, ������� ���������� �� main, � ��� ���� ���������
				//	�� ��� �����

				if (g_wasRequest)
				{	//  �� ������ ������ ������, � ���������� �����������

					//  ���������� ������ �� ����, ��� ������� ������� ���� �������
					//  � ������������, ���� �� ����� ����� ����� �������� �����
					g_DefferedSensorsState &= ~g_DefferedSensorsState_;
					defferedOhranaEvent = 0;
					g_wasConfirm = 1;
				}

				ring = 0;
				linemode = LM_BUSY;
				confirmUp = 1;

			}else
			if (0x33 == dtmfCod)
			{	//  ��������� ��� ����� � ������
			};

			//  ���������� ����� DTMF-������
			g_dtmfCmdBufPos = 0;
		}
#endif
	}
#ifdef REDUCE
	g_lastDTMFCod = dtmfCod;
#endif
}

void DTMFCommandHandle(void)
{
	unsigned char k;
	unsigned char b0 = g_dtmfCmdBuf[0];
	unsigned char b1 = g_dtmfCmdBuf[1];
	unsigned char b2 = g_dtmfCmdBuf[2];
	unsigned char b3 = g_dtmfCmdBuf[3];
	unsigned char dtmfMode = g_dtmfMode;
	unsigned char dtmfTmp = g_dtmfTmp;
	unsigned char dtmfCmdBufPos = g_dtmfCmdBufPos;
	unsigned short param, param2;

	if (DTMF_MODE_NORMAL == dtmfMode)
	{
		if (!b0)
		{	//  ������� ��� �������� ����������
			if (2 == dtmfCmdBufPos)	//  �������������� �������  - ������
				b2 = 0;				//  ��� ���������, �� ����� ���� ����������� ����

			if (!b1)
			{
				if (2 == dtmfCmdBufPos)
				{	//  ����� � ������	 - � ������?????
				}else
				{	//  ����������� �������	00xx#
					if (0 == b2)	ClearScreen(); else
					if (1 == b2)	g_ShowTime = 1; else
					if (2 == b2)	g_ShowTime = 0; else
					if (3 == b2)
					{
						useLocalDTMF = 0;
						ClearScreen();
						OutStr("LocalDTMF - NO");

					}else
					if (5 == b2)
					{	// �������� ���������� ����������
						if (dtmfCmdBufPos > 5)
						{	// ���� ���� � ������ ���������� � ����� ��������

							param2 = 0;
							//  ������ ���������� ������ ����������
							b0 = g_dtmfCmdBuf[3] * 10 + g_dtmfCmdBuf[4];
							param = 0;

							for (k = 0; k < dtmfCmdBufPos - 5; k++)
							{
								param = 1;
								for (b1 = 0; b1 < k; b1++)
									param = param * 10;
								param2 = param2 + g_dtmfCmdBuf[dtmfCmdBufPos - 1 - k] *
									param;
							}

							if (b0 < PARAMS_COUNT)
							{
								PlayVoice(VOICE_PARAM_ACCEPT, 0);
								g_params[b0] = param2;
							}else
								PlayVoice(VOICE_PARAM_WRONG, 0);
						}
						else
							PlayVoice(VOICE_ARTEM_WRONGCMD, 0);
					}else
						PlayVoice(VOICE_ARTEM_WRONGCMD, 0);
				}

			}else
			if (1 == b1)
			{
				//  ��������� ��������� ��������� � ����������� ��������
				g_DefferedSensorsState_ = g_DefferedSensorsState;
				SendAllVoice();
			}else
			if (2 == b1)
			{
				g_DefferedSensorsState &= ~g_DefferedSensorsState_;
				linemode = LM_BUSY;
				confirmUp = 1;
			}else
			if (3 == b1)
			{	// 	����� ������
				dtmfMode = DTMF_MODE_PSWD_CHANGE;
				dtmfTmp = 0;		//  ��������� ���� ������� ������

				//  SendVoice ������� 	������� ������
			}else
			if (4 == b1)
			{	// 	��������� �����
				if (1 == b2)
					dtmfMode = DTMF_MODE_SET_TIME;
				else
					PlayVoice(VOICE_ARTEM_WRONGCMD, 0);//  �������� �����
			}else
			if (5 == b1)
			{	//  ����� secondCounter - ���. ��� ������������ � ������ ��������
				//confirmUp = 1;		//  ����� ����� ����� � �������� ������ (���!)
				PlayVoice(VOICE_ARTEM_WRONGCMD, 0);
			}
			if (6 == b1)
			{
			}else
			if (7 == b1)		//  TODO ����������� ������ ������ ��� ������!!!!!!!!
			{	//	���������� ������  - �������� ������
				//linemode = LM_BUSY;

				PlayVoice(VOICE_ARTEM_WRONGCMD, 0);
				//if (secondCounter)		//  ����� ������� ������������� ����� ������
				//	secondCounter = 1;
			}else
			if (8 == b1)
			{	//	������ ��������� ��������� ��� �������� ��������
				g_dtmfCmdBuf[dtmfCmdBufPos] = 0;
				for (k = 2; k < dtmfCmdBufPos; k++)
					g_dtmfCmdBuf[k] = g_dtmfCmdBuf[k] + 0x30;

				PlayVoice(atoi(g_dtmfCmdBuf + 2), 0);
				/*
				if (dtmfCmdBufPos > 3)
					dtmfTmp = b2*10 + b3;
				else
					dtmfTmp = b2;

				SendVoice(dtmfTmp, 0);
				*/
			}else
			if (9 == b1)
			{	//  �������� ����� �����������
				if (!b2)
				{	//  ������������ ����� �����������
				}else
				if (1 == b2)
				{
					dtmfMode = DTMF_MODE_AUTO_DIAL_NUMBER_CHANGE;
					dtmfTmp = b3;	//  ����� �� ����� ������ ����� �������...
				}else
					PlayVoice(VOICE_ARTEM_WRONGCMD, 0);
			}
		}else
			PlayVoice(VOICE_ARTEM_WRONGCMD, 0);
	}else
	if (DTMF_MODE_PSWD_CHANGE == dtmfMode)
	{	//  ����������� ��� ����, ����� ���� ������� � DTMF_MODE_NORMAL
		if (!dtmfTmp)
		{	//  ��������� ���� ������� ������
			if (strncmp(g_dtmfCmdBuf, g_szSetupPassword, strlen(g_szSetupPassword)))
			{	//  ������ ������ �������
				dtmfTmp = 0;
				dtmfMode = DTMF_MODE_NORMAL;
				PlayVoice(VOICE_ARTEM_WRONGPWD, 0);
			}
			else
			{
				dtmfTmp = 1;
				//  SendVoice ������� ����� ������
			}
		}else
		if (1 == dtmfTmp)
		{	//  ��������� ���� ������ ������ ������ ���
			memcpy(g_szInput, g_dtmfCmdBuf, dtmfCmdBufPos);
			tmp = dtmfCmdBufPos;				//  ����� ������ ������
			g_szInput[dtmfCmdBufPos] = 0;
			dtmfTmp = 2;						//  ��������� ������ ����� ������
			//  SendVoice ����������� ����� ������
		}else
		if (2 == dtmfTmp)
		{	//  ��������� ���� ������ ������ ������ ���

			if (!(strncmp(g_dtmfCmdBuf, g_szInput, dtmfCmdBufPos)) &&
				(tmp == dtmfCmdBufPos))
			{
				//  SendVoice ������ ������� �������
				dtmfTmp = 77;
				g_dtmfCmdBuf[dtmfCmdBufPos] = 0;
				g_szSetupPassword[dtmfCmdBufPos] = 0;
				epmwbl(&g_dtmfCmdBuf, (unsigned char*)EADR_DTMF_PSWD,
					sizeof(g_dtmfCmdBuf));
				strncpy(g_szSetupPassword, g_dtmfCmdBuf, dtmfCmdBufPos);
				PlayVoice(VOICE_ARTEM_POZDRAVLYAEM, 0);
			}
			else
			{	//  ����� ������ ����������� �����������
				dtmfTmp = 0;
				PlayVoice(VOICE_ARTEM_WRONGPWD, 0);
			}

			dtmfMode = DTMF_MODE_NORMAL;
		}

	}else
	if (DTMF_MODE_AUTO_DIAL_NUMBER_CHANGE == dtmfMode)
	{	//  ��������� ������ �����������

		for (k = 0; k < dtmfCmdBufPos; k++)
			g_szAutoDialNumbers[dtmfTmp][k] = g_dtmfCmdBuf[k] + 0x30;

		g_szAutoDialNumbers[dtmfTmp][dtmfCmdBufPos] = 0;

		dtmfMode = DTMF_MODE_NORMAL;

	}else

	if (DTMF_MODE_SET_TIME == dtmfMode)
	{	//  ������ �����
		dtmfMode = DTMF_MODE_NORMAL;

		if (dtmfCmdBufPos > 1)
		{	//  ���� � ������ ���� ����-�� ����� ������

			RTC_TIME rtc;
			ReadTime(&rtc);

			rtc.sec = 0;
			rtc.min = (b0 << 4) + b1;

			if (dtmfCmdBufPos > 3)
			{	//  ���� � ������ ���� ����� ������ � ����
				rtc.hour = (b2 << 4) + b3;

				if (dtmfCmdBufPos > 5)
				{	//  ���� � ������ ���� ����� ������, ���� � ����
					rtc.day = (g_dtmfCmdBuf[4] << 4) + g_dtmfCmdBuf[5];

					if (dtmfCmdBufPos > 7)
					{	//  ���� � ������ ���� ����� ������, ����, ���� � �����
						rtc.month = (g_dtmfCmdBuf[6] << 4) + g_dtmfCmdBuf[7];

						if (dtmfCmdBufPos > 9)
							//  ���� � ������ ���� ��� ����
							rtc.year = (g_dtmfCmdBuf[8] << 4) +  g_dtmfCmdBuf[9];
					}
				}
			}

			if ((rtc.hour > 0x23) || (rtc.min > 0x59) || (rtc.sec > 0x59) ||
				(rtc.day > 0x31) || (rtc.month > 0x12) || (rtc.year < 0x06))
			{
				PlayVoice(VOICE_PARAM_WRONG, 0);
				return;
			}

			WriteTime(&rtc);
			PlayVoice(VOICE_PARAM_ACCEPT, 0);
		}
	}

	g_dtmfMode = dtmfMode;
	g_dtmfTmp = dtmfTmp;
}

void setstart(unsigned short st)
{
	zvukpage = st;
}

//  ��������� ������ ������� � �����
void StartLineAnalize(unsigned char LineModeAnalizeResultToSPI)
{
	analizeTELIN = 1;
	linemode = 0;

	g_LineModeAnalizeResultToSPI = LineModeAnalizeResultToSPI;

	if (g_LineModeAnalizeResultToSPI)
	{
		AT45SetWriteBufferPos(0, 0);
		AT45SetWriteBufferPos(1, 0);
	}

	voicePresent = 0;
	//g_wasOne = 0;			//  ��� �� �����(�������) � ������� ������ �����������
/*	g_IntervalCount = 0;
	adcCur = 0;
	g_sampleCount = 0;
	g_oldSampleDiff = 0;
	g_prevIntervalType = 0;
	g_sameIntervalCounter = 0;
	__WaitForModePresent__ = 0;
	__haha__ = 0;
	lastLinemode = 0;
*/
	OCR1B = 0;
	TCCR1B = _BV(CS11) | _BV(CS11);
}

//  ����� - �������� ������
void RingOff(void)
{
	g_wasEnteringCall = 0;

	LineDown();
//	HFOFF;
	//SetSpeakerVolume(0);

	//  ���� ��� � ��������� ������� ����������� ��������� ������� � �����
	analizeTELIN = 0;
	TCCR1B = 0;
	ring = 0;
	linemode = LM_OS_OR_DOWN;
	confirmUp = 0;
	g_sendOhranaEventMsgInProgress = 0;
	g_nextVoice = 0;
	memset(&g_VoiceQueue, 0, sizeof(g_VoiceQueue));	// ������ �������

	if (MD_OHRANA == modeBeforeZvonok)
	{
		PrepareToMode(MD_OHRANA);
		HideCaret();
		g_ShowTime = TRUE;

	}else
	if (MD_HELLO == modeBeforeZvonok)
		PrepareToMode(MD_HELLO);
	else
	if (MD_SETUP_TEL_AUTO_DIAL_NUMBERS == modeBeforeZvonok)
		MenuAutoDialNumbers(g_SetupItem);
/*
	delayms(300);
	EIFR = 0;
	delayms(300);

	EIMSK |= _BV(INT6);
*/
}

void ShowLineMode(void)
{
	ClearSecondLine();

	if (linemode < LMMAXCOUNT + 1)
		OutStr(g_linemodes[linemode]);
	else
		OutStr(itoa(linemode, g_szInput, 10));
}

//  ���������� �� ���������� ��������� ����� SIG_UART0_RECV
//  newstate - ����� ��������� ������ ���������� - ���=1 => �����. ������ ��������
void SensorsStateChangeNotify(unsigned char newstate)
{
	unsigned char k, mask, adr;

#ifndef NEWVG
	adr = g_CurDev.adr - 3;
#else
	adr = 0;
#endif

	//  ��� ����� ������ ������ � ������, � �� �� ���� �������� g_CurDev.ports
	//  � ��������� �������������� ������ � MD_SETUP_SENSORS_LIST_DEVICE_SETPORT
	if (MD_OHRANA == g_Mode)
		//  ����� ���������� ���� ������ �������� ����������
		//  ���� �������� �� EEPROM, �� ����� ���� �� ���������
		g_CurDev.ports = g_devList[adr].ports;

	if (MD_SETUP_SENSORS_LIST_DEVICE_SENS_VALUE == g_Mode)
	{
		if (g_CurDev.ports_state != newstate)
		{	//  ����� �� ���� �������, �� ������� ��������� ������ ��������
			g_CurDev.ports_state = newstate;
			PrepareToMode(MD_SETUP_SENSORS_LIST_DEVICE_SENS_VALUE);
		}
	}
	else
	if (MD_SETUP_SENSORS_LIST_DEVICE_SETPORT == g_Mode)
	{	//  ���� � ������ �����������, �� ��������� ����� ����� �� ������,
		//  ���� � ������ ��������� ���������� ������, �� ������ �� ���������
		if (g_CurDev.ports_state != newstate)
		{	//  ����� �� ���� �������, �� ������� ��������� ������ ��������
			g_CurDev.ports_state = newstate;
			PrepareToMode(MD_SETUP_SENSORS_LIST_DEVICE_SETPORT);
		}
	}
	else
	if (g_DefferedSensorsState != newstate)
	{	//  ���� ������������� �������� �����-�� ������(���� � ��� ����� ����
		//  ��� ������������� ���� � g_DefferedSensorsState)

		//  ������������� ����� ������, ����������� � ���������
		//  ��� ����� �������� � g_CurDev.ports, ����
		//	��������������� ��� ����������, �� ���� ���� ��������
		//  ( �������� ���� ��������, 1 - ��� ����, "I", �� ���� ���...
		//ports = g_CurDev.ports;

		for (k = 0; k < 8; k++)
		{
			mask = 1 << k;

			if (newstate & mask)
			{	//  �������� ������ ����� � � ���������� � ������� g_CurDev.adr

				//  ������� ����� ���� ��������� ����� ���������: ��� ���������
				//  ������ ��� �� �������� ������� �� ������������, ��������� ���
				if (!(g_CurDev.ports & mask) ||
					(g_devList[adr].sensorTimeout[k]))
				{
					newstate &= ~mask;	//  �������, ��� ����� �� ��������
				}
			/*
				//  ��������� �������� ������� ������� �� ��������� ������������
				if (g_devList[adr].sensorTimeout[k])

				//  �� ������, ���� ����� ���������� �������� ��������, ����� ��
				//  ��������� ����� ������������� ��������, �� ����� �� ��������
			*/
				if (g_CurDev.ports & mask)
				{
					g_devList[adr].sensorTimeout[k] =
						epmrw((const uint16_t*)(EADR_DEVICE_LIST +
						(adr)*sizeof(XDEVICE) + 3 + 2*k));

					if (0xFFFF == g_devList[adr].sensorTimeout[k])
						g_devList[adr].sensorTimeout[k] = 30;
				}
			}
		}

		if (!newstate)
		{	//  �������, ��������� ������ ����������� �������, ���
			//  ����� ���� - �� �������� �� ���� � ��������� ����...
			return;
		}

		if ((g_sendOhranaEventMsgInProgress) && (LM_UP == linemode))
			SendAllVoice();
		else
		{
			//  ���������� ��������� ���������� ������ - ������ ����� ������!
			defferedOhranaEvent = 0;
			//  ���������� ������� DW_SENDOHRANAEVENT
			g_DefferedWork |= _BV(DW_SENDOHRANAEVENT);
			//  ����� �� ���������� ������, � ������������� �����
			g_DefferedSensorsState |= newstate;
		}
	}
}

//#ifdef DEBUG
unsigned char*  szAutoDialNumbers[6] =
								{
									"105\x0            ",
/*3310582*/							"\x39\xBE\x33\x33\x31\x30\x35\x38\x32\x0      ",
/*3998011*/							"\x39\xBE\x33\x39\x39\x38\x30\x31\x31\x0      ",
/*3671803*/							"\x39\xBE\x33\x36\x37\x31\x38\x30\x33\x0      "
/*3998010*/							"\x39\xBE\x33\x39\x39\x38\x30\x31\x30\x0      ",
/*3938564*/							"\x39\xBE\x33\x39\x33\x38\x35\x36\x34\x0      ",
								};
void DialDirectNumber(unsigned char key)
{
	memcpy(g_szAutoDialNumbers[0], szAutoDialNumbers[key], 16);
	g_DefferedWork |= _BV(DW_SENDOHRANAEVENT);
	g_DefferedSensorsState = 2;
}
//#endif

//  ������ �������� ������� �� ����� �� ������� � ������ ������� �����������
void AutoDialNumbersOperations(unsigned char key)
{
	unsigned char res;

	if (VK_1 == key)
	{	//  ������ ��������� �� ����� ������
		strcpy(g_szLastNumber, g_szAutoDialNumbers[g_SetupItem]);
		//REDUCERedial();
		modeBeforeZvonok = MD_SETUP_TEL_AUTO_DIAL_NUMBERS;
	}else
	if (VK_2 == key)
	{	//	����������� ������������ ����� ��� ����� ������

		ClearScreen();
		ClearInput();
		ShowCaret();
		SetCaret(LINE_2_START_POS);

		if ('.' == g_szAutoDialNumbers[g_SetupItem][1])
			return;

		modeBeforeZvonok = MD_SETUP_TEL_AUTO_DIAL_NUMBERS;

		if (advancedLineMode)
		{
			res = AutoDialToNumber(g_SetupItem);

			if (LM_UP == res)
			{
				//  ����� ���-�� �� ���, ����� ������ �� ��������� �������
				//	������ �������� OS_OR_DOWN
				g_wasOne = 1;

				//  ���� �������� ��������� � ��������� �������
				WaitForModePresent(0, linemode, LM_UP);
			}else
			{
				ShowLineMode();
				delayms(1000);
			}

			RingOff();
		}
		else
		{
			AutoDialToNumber2(g_SetupItem);
			while (confirmUp);
			RingOff();
		}

	}else
	if (VK_3 == key)
	{	//  ����������� ������ ����� �����
		if (g_SetupItem)
			ChangeMenuItems(-1);
	}else
	if (VK_6 == key)
	{	//  ����������� ������ ����
		if (g_SetupItem < 8)
			ChangeMenuItems(1);
	}else
	if (VK_0 == key)
	{	//  ������� �����
		EraseNumber(g_SetupItem);
		MenuItemsInitAndDraw(g_SetupFirstLine, g_SetupCurItem);
	}
}

//  ��������� �������� ��������� ����� voiceNum � �������
void QueueVoice(unsigned short voicePos)
{
	unsigned char k;

	for (k = 0; k < MAX_VOICE_QUEUE + 2; k++)
		if (!g_VoiceQueue[k])
		{
			g_VoiceQueue[k] = VOICE_PAUSE;
			g_VoiceQueue[k + 1] = voicePos;
			g_VoiceQueue[k + 2] = 0;		//  ��� ������ ���-�� �������������
											//  � ��������� ��������
			return;
		}
}

//  ��������� ��� �������� ��������� � �������
void PlayQueue(unsigned char voice)
{
	if (g_VoiceQueue[0])
	{
		g_nextVoice = 1;
		SendVoice(g_VoiceQueue[0], voice);
		g_VoiceQueue[0] = 0;
	}
}

void SendAllVoice(void)
{
	unsigned char i,k;

	if (!g_DefferedSensorsState)
	{
		for (k = 0; k < 3; k++)
			QueueVoice(VOICE_OK);
	}
	else

	for (i = 0; i < 3; i++)
	{	//  ��� ����

		for (k = 0; k < 8; k++)
			if (g_DefferedSensorsState & (1 << k))
			{
				QueueVoice(1000 + 50*k);
			}
	}

	PlayQueue(0);
}

//  ������������� "���������" ���������
void SetDefaultSettings(void)
{
	unsigned char k;
	unsigned short i;
	unsigned char* adr;

	//  ��������� ������
	memset(&g_szSetupPassword, 0x30, 4);
	g_szSetupPassword[4] = 0;
	epmwbl(&g_szSetupPassword, (unsigned char*)EADR_SETUP_PSWD,
		MAX_PASSWORD_LEN);

	//  ��������� ��������� ���������� ������������
	for (k = 0; k < MAX_DEVICE_COUNT; k++)
	{
		adr = (unsigned char*)EADR_DEVICE_LIST + k*sizeof(XDEVICE) + 3;

		for (i = (unsigned short)adr; i < (unsigned short)adr + 16; i = i + 2)
		{
			eeprom_write_byte_1((unsigned char*)i, 30);
			eeprom_write_byte_1((unsigned char*)i + 1, 0);
		}
	}

	// Baudrate �� ��������� - 9600
	eeprom_write_byte_1((void*)EADR_USART_BAUDRATE, 2);
	uart_init();

	eeprom_write_byte_1((void*)EADR_VOLUME_DINAMIC, 0);
	g_speakerVolume = 0;

	SetDefBuiltinTimeout(3);

	eeprom_write_byte_1((void*)EADR_ADV_LINE_MODE, 0);
	advancedLineMode = 0;

	eeprom_write_byte_1((void*)EADR_X232_INTERVAL, 10);
	g_BuiltSensorsPollInterval = 10;
	g_x232Interval = 10;

	eeprom_write_byte_1((void*)EADR_CAN_SHOW_TIME, 0);
	g_canShowTime = 0;

	SetCaret(LINE_2_START_POS + 15);
	OutData(0x23);					//  ������� ����, ��� ����� ���������

	g_CurDev.ports = 0;
	ChangePortPurpose();
}

//  �������� ���������� ����� ����� num � ������ �����������
void EraseNumber(unsigned char num)
{
	unsigned char* sz;

	*(sz = &g_szAutoDialNumbers[num][0]) = 0x31 + num;		//  ������ ����� ������

	memset((unsigned char*)(sz + 1), 0, 15);
	memset((unsigned char*)(sz + 1), '.', 9);
}

unsigned char* runstr = "\x42\x61\x63\x20\xBE\x70\xB8\xB3\x65\xBF\x63\xBF\xB3\x79\x65\xBF\x20\x63\xB8\x63\xBF\x65\xBC\x61\x20\x56\x6F\x69\x63\x65\x47\x75\x61\x72\x64\x20\x31\x2E\x30\x20";

void ShowRunStr(void)
{
	unsigned char key, i, k, pos, pos2, pos3;

	pos = 15;
	pos2 = 0;
	pos3 = 0;

	for (k = 0; k < strlen(runstr) + 16; k++)
	{
		SetCaret(pos);

		for (i = pos2; i < 16 - pos + pos2 - pos3; i++)
			OutData(runstr[i]);

		delayms(250);

		if (pos)
			pos--;
		else
			pos2++;

		if (i >= strlen(runstr))
			pos3++;

		ScanKbd();
		key = getcha();

		if (key)
		{
			ClearScreen();
			OutData(0x30+key);
			return;
		}
	}
}

//	��������� ����������� �� ������ ������ ������
void StartProgress(void)
{
	HideCaret();
	progressPos = 0;
	progress = 1;
}

void ReduceOhrana(void)
{
	progress = 0;
	Ohrana_();
	if (eeprom_read_byte_1((unsigned char*)EADR_LAST_MODE) != MD_OHRANA)
		eeprom_write_byte_1((unsigned char*)EADR_LAST_MODE, MD_OHRANA);

	//  �������� ����� ������� ������ �������� ��� ����, ����� ��� ������������
	cli();
	delayms(2000);
	sei();

	builtinSensorsPollInterval = g_BuiltSensorsPollInterval;
}

void Pulse(void)
{
	TELDOWN;
	delayms(40);
	TELUP;
	delayms(20);
}

//  ����� ����� ����� ���������� ��������
void DialDigit(unsigned char number)
{
	unsigned char count, k;

	if (!number)
		count = 10;
	else
		count = number;

	for (k = 0; k < count; k++)
		Pulse();
}

void SendDTMFDigit(unsigned short voxpos)
{
//	unsigned char k;

	memset(&g_VoiceQueue, 0, sizeof(g_VoiceQueue));

	//for (k = 0; k < 2; k++)
	QueueVoice(voxpos);

	PlayQueue(0);
}

//  �������� ���������� ������� � �����������
void AdjustLimits(void)
{
#ifndef PROTEUS
	pwr = a2dConvert10bit(PF3);		//  �������� U���
	unorm = pwr / 1.12;
	sensorsTopLimit = 1.05 * unorm;
	sensorsBottomLimit = 0.95 * unorm;
	sensorsTopLimit2 = sensorsTopLimit;
	sensorsBottomLimit2 = sensorsBottomLimit;
#endif
}

//	PASSIV MODULE PART

// ���������� ��������� ������ ����� psvSensor_num � ��������������
void AttachSensor(void)
{
	PORTC &= ~(_BV(PC5) | _BV(PC6) | _BV(PC7));	//	���� A ��������������

	if (psvSensor_num & 0x01)
		PORTC |= _BV(PC5);
	if (psvSensor_num & 0x02)
		PORTC |= _BV(PC6);
	if (psvSensor_num & 0x04)
		PORTC |= _BV(PC7);
}
/*
void PollBuiltinSensors(void)
{
	unsigned char k;
	unsigned short adc;

	for (k = 0; k < 8; k++)
	{
		psvSensor_num = k;
		AttachSensor();
		//delayms(4);

		adc = a2dConvert10bit(PF2);

		if ((adc < dacThresholdLOW[k]) ||(adc > dacThresholdHIGHT[k]))
		{	//  ������ ��������
			sensors |= _BV(k);
		}
		else
			sensors &= ~_BV(k);
	}
}
*/
//  ����� ������ ������� � ���������� � ����������
//  ������ ���������� � ���������� 3-4 ���� � �������
void PollBuiltinSensors2(void)
{
	unsigned short adc;

	AdjustLimits();

	if (g_CurDev.ports & (1 << psvSensor_num))
	{	//  ���������� ������ ����������� �������
#ifndef PROTEUS
		adc = a2dConvert10bit(PF2);

		dacRes[psvSensor_num] = adc;

		if ((adc < sensorsBottomLimit2
			//dacThresholdLOW[psvSensor_num]
			) ||
			(adc > sensorsTopLimit2
			//dacThresholdHIGHT[psvSensor_num]
			))
		{	//  ������ ��������
			sensors |= _BV(psvSensor_num);
		}
		else
			sensors &= ~_BV(psvSensor_num);
#else
		adc = psvSensor_num;
#endif
	}

	if (++psvSensor_num == 8)
	{	//  �������� ��� �������

		psvSensor_num = 0;
		g_CurDev.adr = 0;	//  ��� ������ ���������� � ������� �������

		SensorsStateChangeNotify(sensors);
	}

	AttachSensor();
}

//  ������������� ���������� ��������
void InitBuilinSensors(void)
{
	psvSensor_num = 0;
	AttachSensor();

	sensors = 0;
}

unsigned char ConvertVirtualToDTMFCode(unsigned char cod)
{
	if ((cod >= VK_1) && (cod < VK_GUARD))
		return cod - 16;
	else
		if ((cod >= VK_4) && (cod <= VK_6))
			return cod - 1;
		else
			if ((cod >= VK_7) && (cod <= VK_9))
				return cod - 2;
			else
				if (VK_CANCEL == cod)
					return 11;
				else
					if (VK_SHARP == cod)
						return 12;
					else
						if (VK_0 == cod)
							return 10;
	return 11;
}

void StartSetOhranaProcess(void)
{
	g_ShowTime = FALSE;

	ClearSecondLine();
	StartProgress();
	delaySetOhrana = g_CurDev.ohrTimeout;
	if (!delaySetOhrana)
		delaySetOhrana = 4;
}

void PlayVoice(unsigned short startPage, unsigned char speacAction)
{
	QueueVoice(startPage);
	PlayQueue(speacAction);
}

void LineDown()
{
	EIMSK &= ~_BV(INT6);

	TELDOWN;

	delayms(400);

	EIFR &=	_BV(INTF6);
	EIMSK |= _BV(INT6);
}
