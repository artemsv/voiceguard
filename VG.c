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
3156619		сим карта джамшида

1047669		Азиз-ука
1249494		Эмине-гуль
1543790		Акмаль-ука
1835055		Неля Шамильевна
2217801		Vligor
3028877		Акром-ака
3036703		Араббой-ака
3052553		Баходыр-ука

3062536		Тимурка
3310582		Артем-ака
3355574		Джасур-ака

1600158		домо-апа - врач компьютерщик
3344637		ее сотка

3622128		Битлз	дом 3 кв 3

митя - 24 декабря
4059961		Мелс-ака
4062609		Аваз-ака

3681450		Людмила Николаевна Перепелюк(рабочий)
2970112		JK
малика		15.03.1987

Михаил Хегай, сотрудник Аваз-ака
180-57-88	сотка
150-16-47	рабочий

191-93-23	Козинец А.
222-08-92	Митин новый дом
223-89-93
*/

#include "DTMF.h"

//  закомментировать в коммерческом релизе
//#define DEBUG

#define HFON		PORTB |= HF
#define HFOFF	PORTB &= ~HF

unsigned char	g_szSetupPassword[MAX_PASSWORD_LEN + 1];

//  режим работы, в котором система находится в данный момент
unsigned char	g_Mode;

//  в некоторых пунктах при выводе пункта с таким индексом выводится галка
//  и вообще, если g_checkedItem != FF, то какой то пункт отмечен
unsigned char	g_checkedItem = 1;

unsigned char	g_ShowTime;		//  показывать во второй строке время
unsigned char 	g_canShowTime;	//  а вообще, разрешено ли время

unsigned char	g_numbers[] = {'A', '1', '2', '3', 'B', '4', '5', '6', 'C',
								'7', '8', '9', 'D', 'E', '0', DIAL_PAUSE, 'F',
								'1', '2', '3'};
unsigned char* 	g_szSecretPassword = "1188564";

//  буфер с вводимыми с клавиатуры цифрами
unsigned char	g_szInput[MAX_INPUT + 1] =
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned char	g_InputPos = 0;	//  след. свободная позиция в буфере g_szInput

#define MAX_MODE_ITEMS	33

const unsigned char*	g_szModes[MAX_MODE_ITEMS] =
			{
				//"\x54\x65\xBB\x65\xE4\x6F\xBD",	//Телефон

				//  Наберите номер
				"\x48\x61\xB2\x65\x70\xB8\xBF\x65\x20\xBD\x6F\xBC\x65\x70",
				//"Введите время",
				"\x42\xB3\x65\xE3\xB8\xBF\x65\x20\xB3\x70\x65\xBC\xC7",
				//Введите пароль
				"\x42\xB3\x65\xE3\xB8\xBF\x65\x20\xBE\x61\x70\x6F\xBB\xC2",
				"VoiceGuard 1.0",

				//Под охраной
				"\xA8\x6F\xE3\x20\x6F\x78\x70\x61\xBD\x6F\xB9",
				"\x48\x6F\xB3\xC3\xB9\x20\xBE\x61\x70\x6F\xBB\xC2",//Новый пароль",
				//Текущее
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

//  пункты главного меню Setup
unsigned char*	g_szSetupItems[MAX_MAIN_SETUP_ITEMS] = {
								"\xE0\x61\xBF\xC0\xB8\xBA\xB8", //  Датчики
//								"GSM",
								"\x42\x70\x65\xBC\xC7",		//  Время
								"\x54\x65\xBB\x65\xE4\x6F\xBD",	//  Телефон
								"\xA1\x70\x6F\xBC\xBA\x6F\x63\xBF\xC2",	//Громкость
								"\xA8\x70\x6F\xBF\x6F\xBA\x6F\xBB",
#ifndef NEWVG
								"\xA4\xB3\x79\xBA",			//  Звук
#endif
								"\x48\x61\x63\xBF\x70\x6F\xB9\xBA\x61"	//  настройка

								};

//  пункты подменю Sensors главного меню Setup
unsigned char*	g_szSensorsItems[MAX_SENSORS_ITEMS] = {
								//  Список
								"\x43\xBE\xB8\x63\x6F\xBA",
#ifndef NEWVG
								//  Поиск
								"\xA8\x6F\xB8\x63\xBA",
								//  Скорость
#endif
								"\x43\xBA\x6F\x70\x6F\x63\xBF\xC2"
#ifndef NEWVG

								//  Опрос
								"\x4F\xBE\x70\x6F\x63"
#endif
								};

unsigned char* g_szDeviceItems[MAX_DEVICE_ITEMS] = {
								"\xA8\x6F\x70\xBF\xC3",		//  Порты
								"\x42\x78\x6F\xE3\xC3",		//  Входы
								//  Задержка
								"\xA4\x61\xE3\x65\x70\xB6\xBA\x61",
								"\xA8\xB8\xBF\x61\xBD\xB8\x65"//  Питание
								};

unsigned char* g_szDeviceSensorItems[MAX_SENSOR_COUNT] = {
								"\xA9\x70\x6F\xB3\xBD\xB8",
								"\x54\x61\xB9\xBC\x61\x79\xBF"
								};

//  пункты подменю Telefon главного меню Setup
unsigned char*	g_szTelefonItems[MAX_TELEFON_ITEMS] = {
								//  Номера
								"\x48\x6F\xBC\x65\x70\x61",
								//  Аон
								"\x41\x6F\xBD",
								//  Режим линии
								"\x50\x65\xB6\xB8\xBC\x20\xBB\xB8\xBD\xB8\xB8",
								//  Определитель
								"\x4F\xBE\x70\x65\xE3\x65\xBB\x65\xBD\xB8\x65"
								};

unsigned char*	g_szVolumeItems[MAX_VOLUME_ITEMS] = {
								"\xE0\xB8\xBD\x61\xBC\xB8\xBA", //Динамик
								"\x54\x65\xBB\x65\xE4\x6F\xBD" //Телефон
								};

//  пункты подменю Line mode меню Telefon
unsigned char*	g_szLinemodeItems[MAX_TEL_LINEMODE_ITEMS] = {
								//  Импульсный
								"\xA5\xBc\xBe\x79\xBB\xC2\x63\xBD\xC3\xA6",
								//  Тоновый
								"\x54\x6F\xBD\x6F\xB3\xC3\xA6"
								};

unsigned char* g_szX232Items[MAX_X232_ITEMS] = {
								"\x54\x61\xB9\xBC\x61\x79\xBF",			//  TimeOut
								"\xA5\xBD\xBF\x65\x70\xB3\x61\xBB"
#ifndef NEWVG
								,		//  Interval
								"\xA8\x61\xBA\x65\xBF"					//  Пакет
#endif
								};

unsigned char* g_szSettingsItems[MAX_SETTINGS_ITEMS] = {
								"RAM",
								"EEPROM",
								"Version",
								"\xA8\x61\x70\x6F\xBB\xC2",	  //  Пароль
								"\x43\xB2\x70\x6F\x63"

								};

#define MAX_INPUT_LENS_COUNT	33

unsigned char	g_InputLens[MAX_INPUT_LENS_COUNT] = {
									INPUT_LEN_TEL + 1, 	// единица нужна для проверки,
									INPUT_LEN_TIME, // ведь g_InputPos начинается
									INPUT_LEN_PSWD + 1,	// с нуля
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
									//  Определение
									"\x4F\xBE\x70\x65\xE3\x65\xBB\x65\xBD\xB8\x65",
									//  Вызов
									"\x42\xC3\xB7\x6F\xB3",
									//  Занято
									"\xA4\x61\xBD\xC7\xBF\x6F",
									//  ОС или положил
									"\x4F\x43\x20\xB8\xBB\xB8\x20\xBE\x6F\xBB\x6F\xB6\xB8\xBB",
									//  Поднял
									"\xA8\x6F\xE3\xBD\xC7\xBB",
									//  Выключен
									"\x42\xc3\xBA\xBB\xC6\xC0\x65\xBD"
								};

unsigned char 	g_LastKey;
unsigned char	g_SetupCurItem = 0;
unsigned char	g_SetupFirstLine = 0;
unsigned char	g_SetupItem;
unsigned char	g_CurMenuMaxItems;		//  количество пунктов текущего меню
unsigned char	g_curBarValue;			//  уровень
unsigned char	g_telon;				//  1 - поднята телефонная трубка
unsigned char 	g_hf;					//  1 - режим HandsFree включен
unsigned char 	g_speakerVolume;		//  текущая громкость динамика
unsigned char	g_szLastNumber[MAX_INPUT + 1];		//  последний набранный номер
unsigned char 	g_bSilent;				//  1 - не посылать в линию набираемый номер

unsigned char 	g_GalkaPunkt;			//  пункт текущего меню с галкой
unsigned char 	g_Galka;				//  тип галки у этого пункта
unsigned char	g_needHello;			//  выдавать приветствие

//  экспортируемые переменные
extern unsigned char g_CaretPos;		//  положение курсора
extern CUR_DEVICE g_CurDev;
extern XDEVICE g_devList[MAX_DEVICE_COUNT];
extern unsigned short wBufPos[2];

//  строки
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
unsigned char** pMasItems;	//  адрес массива с пунктами
extern unsigned char g_x232Interval;
extern unsigned char g_StreamMode;
unsigned char BaudRates[7] = {3, 6, 12, 24, 48, 72, 144};

unsigned char ring = 0;				//  1 - звонит звонок

unsigned short zvukpos = 0;			//  позиция в буфере для очередного байта звука
unsigned short zvukpage;			//  номер текущей страницы
unsigned char activeBuf = 0;

unsigned char modeBeforeZvonok;		//  какой режим был во время звонка?

unsigned char analizeTELIN;
unsigned char telinDivider;
unsigned char linemode;
unsigned char lastLinemode;
unsigned char g_sendOhranaEventMsgInProgress;	// 	1 - происходит автодозвон
unsigned char g_DefferedWork;					//  каждый бит  - какая-то работа
unsigned char g_DefferedSensorsState;			//  новое состояние датчиков
unsigned char g_DefferedSensorsState_;			//  временное состояние
unsigned char g_DefferStage = 0;				//  стадия попыток оповестить хозяина
unsigned short defferedOhranaEvent;
unsigned char g_LineModeAnalizeResultToSPI;

//  индексы переменных - параметров
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

//  кол-во режимов телефонной линии
#define LMMAXCOUNT		6
//  режимы телефонной линии
#define LM_KPV			1
#define LM_BUSY			2
#define LM_OS_OR_DOWN	3
#define LM_UP			4
#define LM_FAULT		5
#define LM_OS_ABSENT	6

//  константы, возвращаемые WaitXXX функциями
#define WR_LM			1
#define WR_TIMEOUT		2

/*	Определения DTMF декодера	*/

#define DTMF_CMD_BUF_LEN	10					//  длина буфера команд DTMF

//  режимы DTMF
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
unsigned char g_dtmfTmp;			//  временная переменная, исп. при DTMF сеансе
unsigned char g_dtmfPSWOk;			//  1 - в текущем сеансе пароль введен правильно
unsigned char useLocalDTMF;		//	1 - клава используется як локальная DTMF-клава
unsigned char g_wasRequest;		//  состояние датчиков было запрошено (кнопка "1")
unsigned char g_wasConfirm;		//  уведомление было подтверждено(кнопка "2")

unsigned short	g_sampleCount = 0;
unsigned char g_fallCounter;	//  количество спадов на 100-сэмпловом промежутке
unsigned short g_sameIntervalCounter;	//  счетчик однотипных 100-сэмпловых интервалов
unsigned char g_prevIntervalType;		//  знак предыдущего интервала
signed char   g_sampleDiff;				//  разность последних двух сэмплов
unsigned char voicePresent;
//  был ли единичный интервал в последнем измерении(после команды начать измерение)?
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
unsigned char g_wasEnteringCall;		//  был входящий звонок

unsigned char g_cancelPressCount;		//  счетчик последующих нажатий Escape
unsigned char g_oldSetupFirstLine;		//  какой пункт главного высветить при Escape
unsigned char g_oldSetupCurItem;		//  куда поставить стрелку
unsigned char helloCounter;
unsigned char progress;
unsigned char progressPos;

unsigned char timer0delay;
//  кол-во попыток уведомить один и тот-же номер, который берет трубку и не жмет "1"
//  (или не берет, за него девушкаговорит)
unsigned char g_attemptCounterTheSameNumber;
//  true - работает новый звук
//unsigned char mc14lc;

#define MAX_VOICE_QUEUE	30 + 2
//  номер очередной позиции для воспр.
unsigned char g_nextVoice;
//  очередь звуков на проигрывание
unsigned short g_VoiceQueue[MAX_VOICE_QUEUE];
/*
unsigned char g_genDTMF;
unsigned short g_cntDTMF;
*/

/// Passiv module declarations

//  номер датчика, который подключен через мультиплексор к АЦП

#define MAX_ADC_PORT_COUNT					8
//  Интервал опроса встроенных датчиков
#define BUILTIN_SENSORS_POLL_INTERVAL_		20

unsigned char psvSensor_num;
unsigned char sensors;
unsigned char builtinSensorsPollInterval;		//  интервал опроса встроенных
double sensorsTopLimit;
double sensorsBottomLimit;
unsigned short sensorsTopLimit2;
unsigned short sensorsBottomLimit2;

//  пороги чувствительности для встроенных датчиков
unsigned short dacThresholdLOW[MAX_ADC_PORT_COUNT];
unsigned short dacThresholdHIGHT[MAX_ADC_PORT_COUNT];

unsigned short dacRes[MAX_ADC_PORT_COUNT];
unsigned short pwr;
double unorm;
unsigned short delaySetOhrana;
unsigned char g_BuiltSensorsPollInterval;

//  максимальное время удержания поднятой трубки без активности хозяина
#define MAX_RAZGOVOR_TIME	20

//  запускает запись с микрофонного входа трубки во
//  флешку по адресу: страница zvPage, после записи zvPages страниц запись прекратить
void StartRecord()
{
	record = 1;
	zvukpos = 0;
	TCCR1B = _BV(CS11) | _BV(CS11);		// коэф. Пересчёта счётчика 1 = 1
	AT45SetWriteBufferPos(0, 0);
}

//  проиграть zvPages страниц звука, начиная со страницы zvPage
void StartPlay(unsigned short startPage, unsigned char speacAction)
{
	OCR1B = 0;
	ring = 1;
	zvukpos = 0;
	if (0xFFFF != startPage)			//  параметр
		zvukpage = startPage;
	TCCR1B = _BV(CS11) | _BV(CS11);		// коэф. Пересчёта счётчика 1 = 1
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

//  анализирует 100-сэмпловый интервал и определяет мгновенное состояние линии
//  зная мгновенное состояние и предысторию, пытается вычислить текущее состояние
void AnalizeInterval(void)
{
	adc_1 = adcCur;						//  предыдущее значение
	adcCur = a2dConvert8bit(1);			//  текущее значение

	g_sampleCount++;					//  счетчик отсчетов

	if (g_sampleCount > 3)
	{
		g_sampleDiff = adcCur - adc_1;

		if (g_sampleDiff < 0)
			g_sampleDiff = 0;
		else
			g_sampleDiff = 1;

		if (g_sampleDiff != g_oldSampleDiff)
		{	//  если произошла смена знака
			g_oldSampleDiff = g_sampleDiff;
			if (!g_oldSampleDiff)
				//  если не просто смена знака, а спад
				g_fallCounter++;		//  счетчик спадов
		}
	}

	if (g_sampleCount > 103)
	{	//  набрался целый интервал пичков - анализируем его

		g_IntervalCount++;					//  счетчик интервалов

		//  g_fallCounter - количество пичков, для 425 Гц оно должно быть 11-13
		if ((g_fallCounter > 10) && (g_fallCounter < 14))
			g_fallCounter = 1;				//	интервал сигнала 425 Гц
		else
			g_fallCounter = 0;				//	интервал молчания, голоса или шума

		if (g_fallCounter == g_prevIntervalType)
			g_sameIntervalCounter++;					//  еще один такой интервал
		else
		{	//  этот интервал не таков, как предыдущий
			if (g_sameIntervalCounter > 0)
				g_sameIntervalCounter--;

			g_prevIntervalType = g_fallCounter;
		}

		if (g_sameIntervalCounter > g_params[PARAM_SAME_INTERVAL_CNT])
		{	//  если количество однотипных интервалов более нормы

			if (voicePresent != g_fallCounter)
			{	//  если это количество интервалов не такое,як предыдущее состояние
				//  или просто это первое зафиксированное изменение состояния линии...

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
					g_wasOne = 1;		//  пришла первая единица в текущем сеансе

				if (g_wasOne)
				{
					if (!g_fallCounter)
					{	//  если сейчас закончился нулевой интервал(пауза)
						// 	и, значит, предыдущий был единичный интервал

						if ((g_IntervalCount << 1) < g_lastIntervalCount)
						//  если предыдущий единичный более чем в два раза меньше
						//	текущего нулевого
						{
							linemode = LM_KPV;
							lastLinemode = 0;
						}
						else
						{	//  "Занято" - хитрый сигнал, надо удостоверится, что
							//	это настоящее "Занято" - для этого ждем два раза подряд
							//  (вообще-то достаточно одного,но это для нашей мини-АТС)
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
			{	//  если однотипные интервалы все продолжаются и продолжаются

				if ((g_IntervalCount > g_params[PARAM_GUDOK_LEN]) &&
						g_wasOne &&	voicePresent)
				{ 	//	механизм ускоренного определения  сигнала "Ответ станции"

					linemode = LM_OS_OR_DOWN;
					g_IntervalCount = 0;
				}else

				if (g_IntervalCount > g_params[PARAM_MAXPAUSE_LEN])
				{	//  если ну очень долгое молчание или гудок

					//  но нельзя реагировать на длинную паузу перед первым гудком

					if (g_wasOne)
					{	//  если первый гудок все-таки был
						//	(то есть это не начальная пауза перед первым гудком)

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
	{	//  если сейчас записываем звук
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
	{	//  если сейчас проигрываем звук
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
				TCCR1B = 0;		//  отключать таймер только при отсутствии измерения
			ring = 0;
			//TCNT1 = 0;

			if (g_nextVoice < MAX_VOICE_QUEUE)
			{	//  не достигнут ли физический конец очереди звуков?

				if (g_VoiceQueue[g_nextVoice])
				{	//  а есть ли задание в очереди?

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
	{	//  если сейчас измеряем звук

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

//  защита от повторных нажатий кнопок
//  таймер инкрементируется с частотой 15625 (16Mhz / 1024)
//  переполняется 15625/256 = раз в секунду ( каждые 16.38 мс )
SIGNAL(SIG_OVERFLOW0)
{
	TCNT0 = 61;
	if (timer0delay)
		timer0delay--;

	if (builtinSensorsPollInterval)
		if (!(--builtinSensorsPollInterval))
		{	//  пришло время опрашивать внутренние датчики
			if (!ring)
				PollBuiltinSensors2();
			builtinSensorsPollInterval = g_BuiltSensorsPollInterval;
		}
}

/*
char ew = 32, hhh, zvb, gotov;
//  прерывание обслуживания MC14LC5480
//  частота 16000000 делится на 32 и получается 512000 тиков в секунду
//  вывод MC_2048 устанавливается в единицу каждый второй тик => на MC_2048 256 kHz
//  MC_8K устанавливается в единицу каждый 64 вызов прерывания => там 8 kHz
SIGNAL(SIG_OVERFLOW0)
{
	if (!(PORTE & MC_2048))
	{	//  сейчас пока низкий уровень MCLK, BCLKT и BCLKR

		//  устанавливаем высокий уровень тактовой синхронизации
		PORTE |= MC_2048;
	}else
	{	//  сейчас высокий уровень MCLK, BCLKT и BCLKR

		//  устанавливаем низкий уровень тактовой синхронизации
		PORTE &= ~MC_2048;

		//ACSR = PIND & MC_8K;
		if ((unsigned char)(PIND & MC_8K))
		{	//  если стоит 8килогерцовый импульс

			//w = (PORTG & MC_DR);			//  читаем бит DT

			if (PING & MC_DR)
				zvb |= 1 << hhh;

			hhh++;								//  счетчик битов

			//if (hhh == 8)
			//{
			//	gotov = 1;
			//}
		}

		if (!--ew)
		{
			PORTD |= MC_8K;		//  фронт кадрового синхроимпульса

			ew = 32;			//  делитель частоты 256 kHz для выхода 8К
		//	hhh = 0;
		}
	}


	//PORTE ^= MC_2048;		//  инвертируем MCLK, BLCKT, BLCKR
}
*/

//  second interrupt
SIGNAL(SIG_OVERFLOW3)
{
	unsigned char k;

   	TCNT3 = TIMER3_SECOND_VALUE;
	//TCNT3 = TIMER3_MAX_INTERVAL + (16 - g_x232Interval) * (TIMER3_200MS_VALUE/2);

	if (g_canShowTime && g_ShowTime && (!ring))		//  может мешать звуку
		UpdateTimePane();

	//  обновляем показания АЦП-метров
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
			{	//  не подтвердил поднятие - отбой
				if ((advancedLineMode) || (g_wasEnteringCall))
					RingOff();
			}
		}
	}

	//  простой счетчик секунд, используется при ожидании поднятия
	if (g_secCnt)
		g_secCnt--;

	if (helloCounter)			//  ограничитель времени набора пароля
	{
		if (!--helloCounter)
		{
			PrepareToMode(MD_HELLO);
		}
	}

	//  прорисовка строки-прогресса
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
	{	//  отложенное событие о сработке датчиков
		if (!--defferedOhranaEvent)
		{	//  таймаут на повтор истек
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
	if(!ring)				//  мешает новому звуку(в старом звуке не было)
		HandleProtocol();	//  находит очередной пакет(если готов) и посылает его
#else
	for (k = 0; k < 8; k++)
	{
		//  если таймаут еще не кончился, уменьшаем его на число циклов, которые
		//  это устройство пропустило из-за наличия других устройств
		//  так сделано из-за накладности уменьшения таймаутов всех устройств
		//  в одном цикле сразу, поэтому при каждом вызове - одно устройство
		if (g_devList[0].sensorTimeout[k])
			g_devList[0].sensorTimeout[k]--;
	}
#endif
}

//  прерывание от DTMF - декодера MT8870
SIGNAL(SIG_INTERRUPT4)
{
	OCR1B = 0;					//  когда DTMF посылка вклинивается в звук,
									//  слышен писк(delay в SendVoice)

	//  если он поднял, и не дожидаясь приветствия, выдал команду
	linemode = LM_UP;
	g_needHello = 0;				//  приветствие тогда выдавать и не надо

	delayms(50);
	DTMFCmdStorer((PINF & 0xF0) >> 4);
	SetCaret(LINE_1_START_POS + 12);
	OutData(0x30 + ((PINF & 0xF0) >> 4));

//	OutStr(itoa((PINF & 0xF0) >> 4, g_szInput, 10));

}

//  от клавиатуры
SIGNAL(SIG_INTERRUPT5)
{
	//ClearScreen();
//	OutData('1');

	delayms(50);
	ScanKbd();
	HandleKeys(getcha());
}

#ifndef REDUCE
//  прерывание поднятия трубки
/*
SIGNAL(SIG_INTERRUPT5)
{
	unsigned char e3, e4;

	g_dtmfPSWOk = 0;		//  кто-бы не поднял трубку, DTMF сеанс запрещен
#ifdef REDUCE
	g_dtmfPasswordPermission = 0;
#endif

	if ((MD_HELLO == g_Mode) || (MD_TEL == g_Mode))
	{	//  поднятие трубки обрабатывается только в режиме 0 и режиме набора номера
		EIMSK &= ~_BV(INT5); 		//  отключаем прерывание INT5

		//  задержка для устранения дребезга
		delay_loop_2(60663);

		e3 = PINE & _BV(PE5);
		e4 = EICRB & _BV(ISC50);

		if ((e3 && e4) || ((!e3) && (!e4)))
		{	//  подтвердилось
			EICRB ^= _BV(ISC50); 		//  инвертируем бит фронта или спада

			if (g_telon)
			{	//  положили трубку
				PORTF &= ~TELON;
				PrepareToMode(MD_HELLO);
				g_telon = 0;
				//SetSpeakerVolume(0);
				PORTD &= ~PWA;			//	выключаем усилитель
				g_hf = 0;
			}else
			{	//  подняли трубку - подключаемся к линии и зажигаем светодиод
				//  если же включен режим HandsFree, ничего не изменяется
				g_telon = 1;

				if (!g_hf)
				{
					PORTF |= TELON;
					if (!g_hf)
						Telephone();
				}
			}
		}

		EIMSK |= _BV(INT5); 		//	включаем прерывание INT5
	}
}
*/
#endif

//  прерывание от входящего звонка (телефонная линия)
SIGNAL(SIG_INTERRUPT6)
{
	if (g_StreamMode) 		//  потоковую запись прерывать никак нельзя
		return;

	g_DefferedSensorsState_ = 0;
	g_dtmfPSWOk = 0;		//  кто-бы не поднял трубку, DTMF сеанс запрещен
#ifdef REDUCE
	g_dtmfPasswordPermission = 0;
#endif

	if ((MD_HELLO != g_Mode) && (MD_OHRANA != g_Mode))
		return;

	TELUP;								//  подняли трубку
	g_wasEnteringCall = 1;

	if (MD_HELLO == g_Mode)
	{
		ClearScreen();

	//	SetSpeakerVolume(g_speakerVolume * 16);

		g_ShowTime = 0;
		OutStr(szZvonok);				//  Звонок

		modeBeforeZvonok = MD_HELLO;	//  чтобы положив, оказались в 0-вом режиме
		delayms(500);
		PlayVoice(VOICE_HELLO, 0);
	}else
	if (MD_OHRANA == g_Mode)
	{	//  автоподнятие в режиме охраны
		g_dtmfPSWOk = 0;				//  пароль не введен
#ifdef REDUCE
		g_dtmfPasswordPermission = 0;
#endif
		modeBeforeZvonok = MD_OHRANA;	//  чтобы положив, оказались в охране
		delayms(500);
		PlayVoice(VOICE_SYSTEM_POD_OHRANOY, 0);
	}
	//  в течении xx-ти секунд должен подтвердить командой
	confirmUp = MAX_RAZGOVOR_TIME;

//	delayms(1000);			//  чтобы успел до уха донести...
}

//  прерывание от входящего звонка (GSM)
SIGNAL(SIG_INTERRUPT7)
{
}

/*
//  возникает каждые 200 миллисекунд
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
	{	//  та к бывает после прошивки - пароль по умолчанию - "0000"
		memset(&g_szSetupPassword, 0x30, 4);
		g_szSetupPassword[4] = 0;
	}
	g_szSetupPassword[MAX_PASSWORD_LEN] = 0;	//  терминатор

	//  сохраняем время старта системы
	ReadTime(&rtc);
	//epmwbl(&rtc, (void*)EADR_START_TIME, sizeof(rtc));

	//  громкость динамика
	g_speakerVolume = eeprom_read_byte_1((unsigned char*)EADR_VOLUME_DINAMIC);

	//  читаем номера телефонов
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
	PORTD = PWR;					//  PD4 - для GSM

	PORTG = 0x00;
	PORTB = 0x66;
	PORTE = 0x04;


	EIFR = 0;				//  очищаем на всякий

	//  настройка внешних прерываний
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

	InitLCD();			//  экранчик

	InitRTC();			//  инициализация DS1307

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

//  меняет местами два пункта меню (используется в списке номеров автодозвона)
void ChangeMenuItems(signed char direction)
{
	unsigned char newItem = g_SetupItem + direction;

	strcpy(g_szInput, g_szAutoDialNumbers[newItem]);

	//  если этот пункт списка содержит пустой номер, то корректируем номер номера...
	if ('.' == g_szInput[1])
		g_szInput[0] = 0x31 + g_SetupItem;

	strcpy(g_szAutoDialNumbers[newItem],
		g_szAutoDialNumbers[g_SetupItem]);

	//  без этих двух ифов была возможна ситуация, когда путем передвига и
	//  редактирования номеров получались одинаковые строки 2..... и 2......,
	//  находящиеся в разных местах списка
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
{	//  список устройств( Устр 1, Устр 2) и т.д.
	Menu_(MD_SETUP_SENSORS_LIST, MAX_DEVICE_COUNT);
}

void MenuSensor(void)
{	//  меню вход - один вход (или датчик) текущего устройства
	Menu_(MD_SETUP_SENSORS_LIST_DEVICE_SENSOR, MAX_SENSOR_COUNT);
}

void MenuDevice(unsigned char b)
{
	if (b)
	{
#ifndef 	NEWVG
		g_CurDev.adr = g_SetupItem + 3;						//  номер устройства(адрес)
#else
		g_CurDev.adr = 0;
#endif

		//  считываем из EEPROM (временно, надо получать от устройств)
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
	//  список входов(датчиков) текущего утсройства - Вход 1, Вход 2 и т.д.
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
		item = 1;				//  если получится так, что первым надо сделать последний
	}

	Menu_(MD_SETUP_TEL_AUTO_DIAL_NUMBERS, MAX_TEL_AUTO_DIAL_NUMBERS_ITEMS);
}

void MenuItemsInitAndDraw(unsigned char SetupFirstLine, unsigned char SetupCurItem)
{
	g_SetupCurItem = SetupCurItem;
	g_SetupFirstLine = SetupFirstLine;
	g_SetupItem = SetupFirstLine + SetupCurItem;
	g_checkedItem = 0xFF;		//  нет отмеченых пунктов

	if (MD_SETUP_TEL_LINEMODE == g_Mode)
		g_checkedItem = eeprom_read_byte_1((void*)EADR_LINE_MODE);

	DrawMenu();
}


void DrawBarInterface(void)
{
	ClearScreen();
	OutStr((char*)pMasItems[g_SetupCurItem]);	//TODO: потенциальная ошибка
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

	OutData(' ');				//  стирает предыдущее значение
	if (!g_curBarValue)
		OutData(' ');			//  иначе остается буква 's' от 100ms

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
		OutStr((char*)g_szModes[mode]);		//TODO: потенциальная ошибка

	ClearInput();

	if (MD_SETUP_TIME == mode)
	{	//  тут надо считать текущее значение времени, чтобы его редактировать
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

		//  режим установки назначений портов
		br = '+';
		br1 = '-';
		port = g_CurDev.ports;

		if (1 == g_CurDev.mode)
		{	//  режим мониторинга
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
	{	//  выводим два текущих значения

		//  включаем опрос датчиков - это даст нам вызов AdjustLimits
		//  и в нем значения Uпит и пр.
		builtinSensorsPollInterval = g_BuiltSensorsPollInterval;
		HFON;
#ifdef NEWVG
		ClearScreen();
		OutStr("\x42\x65\x70\x20");		//  Вер
		OutStr(itoa(sensorsTopLimit2, g_szInput, 10));
		SetCaret(LINE_1_START_POS + 9);
		OutStr("\x48\xB8\xB7\x20");		//  Низ
		OutStr(itoa(sensorsBottomLimit2, g_szInput, 10));
		SetCaret(LINE_2_START_POS);
		OutStr("\xA8\xB8\xBF\x20");		//  Пит
		OutStr(itoa((unsigned short)pwr, g_szInput, 10));
		SetCaret(LINE_2_START_POS + 9);
		OutStr("\xE0\x61\xBF\x20");		//  Дат
		OutStr(itoa(dacRes[g_CurDev.curSensor], g_szInput, 10));
#else
		OutStr(itoa(g_CurDev.threshold1, g_szInput, 10));
		SetCaret(LINE_2_START_POS + 7);
		OutStr(itoa(g_CurDev.threshold2, g_szInput + 7, 10));

		//  готовимся ко вводу новых значений
		//  если в момент нахождения каретки под старым значением будет
		//  нажата цифровая кнопка, старое исчезнет и будет считаться, что вводится
		//  новое значение. Если на первом значении будет нажат ентер, каретка
		//  перейдет ко второму, если и там ентер - ввод закончен
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

//  вход в режим MD_SETUP_WAITPSWD
void Setup(void)
{
	g_ShowTime = FALSE;

	g_curBarValue = MD_SETUP;

	//  режим ожидания ввода пароля
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
		DeletePrevChar();						//  в режиме изменения времени

	DeletePrevChar();
}

void Ohrana(void)
{
	g_ShowTime = FALSE;

	g_curBarValue = MD_OHRANA;					//  оптимизация - присваивание заранее

	if (MD_OHRANA == g_Mode)
		//  нажали в режиме охраны - выход из режима охраны
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

	//if (MD_SETUP == mode)		оптимизация...
	pMasItems = (unsigned char**)&g_szSetupItems;

	if (MD_SENSORS == mode)
		pMasItems = (unsigned char**)&g_szSensorsItems;
	else if (MD_SETUP_TEL == mode)
		pMasItems = (unsigned char**)&g_szTelefonItems;
	else if (MD_SETUP_SENSORS_LIST == mode)
	{
		ClearInput();
		strcat(g_szInput, szUstr);		//  Устр
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
		strcat(g_szInput, szVhod);				//  Вход -

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
		lineNo = lineNo + 3;				//  в этом режиме нумерация с 3
#else
		lineNo = lineNo + 1;
#endif

	OutStr(GetItemName(g_SetupFirstLine + lineNo)); //  выводим первую строку меню

	if (g_SetupCurItem == lineNo_)			//  есть ли стрелка на первой строке
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

//  перерисовка меню Setup'a

//  галочных интерфейса сущ. два типа - в двустрочном меню(режим MD_SETUP_TEL_LINE_MODE)
//  и в многострочном меню. В первом типе галки возле пунктов меняются местами,
//  а во втором после ентера галка меняется у одного пункта

void DrawMenu(void)
{								//  цикл (любой) здесь не выгоден
	ClearScreen();
	HideCaret();

	DrawMenuLine(0, 0);			//  прорисовка первой строки

	SetCaret(LINE_2_START_POS);

	if (g_CurMenuMaxItems > 1)
		DrawMenuLine(1, 0x40);		//  прорисовка второй строки
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

//  обработчик нажатия кнопок влево и вправо
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
	{	//  перемещения по меню
		if (VK_UP == key)					//  вверх
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

		if (VK_DOWN == key)					//  вниз
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
		{	//  если в меню всего лишь один пункт (в несетевой версии)
			g_SetupItem = 0;
			g_SetupCurItem = 0;
		}
		DrawMenu();
	}else
	if ((MD_SETUP_NEWPSWD == mode) || (MD_SETUP_WAITPSWD == mode)
		|| (MD_REDUCE_EDIT_ == mode))
	{	//  старый режим стирания
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
		if (VK_BACKSPACE == key)		//  в этих режимах FUNC = Backspace
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
				//  перевод каретки вправо, к концу строки (если есть введенные символы)
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

//  обработчик нажатия цифровых кнопок
void NumerKey(unsigned char key)
{
	unsigned char mode = g_Mode, k;
	unsigned char inputPos = g_InputPos, numk2 = g_numbers[key], numk = numk2 - 0x30;

	if (MD_REDUCE_EDIT == mode)				//  если находимя в списке номеров
		PrepareToMode(MD_HELLO);			//  упрощенного режима

	if (MD_HELLO == mode)
	{	//  новое упрощенное видение
		if (!inputPos)
		{
			if (VK_6 == key)
			{	//  режим редактирования номеров
				g_ShowTime = FALSE;
				MenuAutoDialNumbers(0);
				g_Mode = MD_REDUCE_EDIT;
				return;
			}
		}else
		if (!inputPos)
		{	//  первая цифра набираемого номера из нулевого режима
			//  переход в режим телефона!!!!!!!
			Telephone();
			mode = MD_TEL;
			g_bSilent = 1;
		}
	}

	//  реакция на цифры разрешена в следующих режимах
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
			return;				//  превышена длина строки для текущего режима

	//  сдвигаем всю строку вправо, если позволяет режим
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

	//TODO: какую то ветку можно упростить
	if (/*(MD_TEL == mode) || */(MD_SETUP_SENSORS_NEWBAUDRATE == mode) ||
		(MD_SETUP_SENSORS_LIST_DEVICE_TIMEOUT == mode)||
		(MD_SETUP_X232_PACKET == mode) ||
		(MD_SETUP_SENSORS_LIST_DEVICE_SENSOR_TIMEOUT1 == mode))
		OutData(numk2);	//  все цифры выводятся на экран
	else if ((MD_SETUP_WAITPSWD == mode)||(MD_SETUP_NEWPSWD == mode))
		OutData('*');		//  звездочки вместо пароля
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
	{	//  g_CurDev.ports - назначение портов текущего модуля датчиков

		if (!numk)
		{	//  нажали 0-смена подрежима 1)мониторинг сработки 2)детальный
			//  мониторинг 3)режим установки и обратно назначений портов
			if (++g_CurDev.mode == 3)
				g_CurDev.mode = 0;

			//  если нужно, включаем режим опроса встроенных датчиков
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
			return;	//  при этом остается лишний символ в g_szInput

		if ((inputPos == 1) || (inputPos == 8))
		{
			if (inputPos == 1)		//  первый знак
				if (g_CurDev.threshold1)		//  еще присутствует старое значение
					//  попытка поменять имеющееся значение(нижнее) на новое
					numk = 0;

			if (inputPos == 8)		//  первый знак
				if (g_CurDev.threshold2)		//  еще присутствует старое значение
					//  попытка поменять имеющееся значение(верхнее) на новое
					numk = 7;

			memset(g_szInput + numk, 0, sizeof(g_szInput) - numk);
			g_szInput[inputPos - 1] = numk2;
			OutStr("      ");
			SetCaret(LINE_2_START_POS + numk);
		}

		OutData(numk2);

		if (inputPos == 4)
		{	//  перейти ко вводу второго порога
			inputPos = 7;
			g_CaretPos = LINE_2_START_POS + 6;		//  учитывается g_CaretPos++
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
			//  очистка старого, если начали набирать новый	номер
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

//  обработчик нажатия клавиши *
void Escape(void)
{
	unsigned char mode = g_Mode;	//  выгодно, если кол-во mode > 15
	unsigned char _mode = g_curBarValue;

	HideCaret();
	g_GalkaPunkt = 0xFF;

	if (MD_SETUP_WAITPSWD == mode)
	{	//  отмена перехода в какой-то режим(g_curBarValue - в какой режим хотели)
		if (MD_HELLO == _mode)
		{	//  отмена ввода пароля на выход из режима охраны - возврат в режим охраны
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
		g_CurDev.ports = tmp;	//  восстанавливаем сохраненное значение
		builtinSensorsPollInterval = 0;	//  отключаем опрос
		HFOFF;
		MenuDevice(0);
	}
	else
	if (MD_SETUP_SENSORS_LIST_DEVICE_SENS_VALUE == mode)
	{
#ifdef NEWVG
		//  выход из режима наблюдения за работой АЦП
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
	{	//  из режима телефона можно выйти только положив трубку
		//  но если режим телефона был "молчаливый", то Escape --> в нулевой режим
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

		if (g_szLastNumber[k] == DIAL_PAUSE)		//  пауза
		{
			if (1 == k)
			{	//  пауза после девятки для выхода на городскую АТС
				delayms(1000);
				//  TODO - сюда надо воткнуть определение состояния линии
			}else
			{	//  скорее всего вторая пауза - число N после нее означает паузу
				//  в N секунд
				if (len - k - 1 < 3)
				{	//  если осталось допустимое кол-во секунд для паузы после номера
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
//  обработчик нажатия клавиши REDIAL
void Redial(void)
{
	//  здесь тоже len увеличивает код на 28-30 байтов
	//unsigned char len = strlen(g_szLastNumber);

	modeBeforeZvonok = MD_HELLO;	//  если будет сеанс DTMF и "нас" вырубят,
									//  командой 07#, RingOff вернет в MD_HELLO
	if (MD_TEL == g_Mode)
	{
		if (g_bSilent)
		{	//  в режиме молчаливого набора REDIAL звонит по набранному номеру
			//  номер уже набран и высвечен на экране
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
	{	//  набор последнего номера, по которому звонили (не набирали!)

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
//  обработчик нажатия # в режиме ввода пароля
//  используется при входе в разные режимы
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

	//  g_curBarValue хранит режим, в который надо войти после ввода пароля
	if (MD_SETUP == g_curBarValue)
	{
		EnablePoll(0);	//  отключаем протокольный опрос - если надо, включат в Setup
		g_oldSetupFirstLine = 0;
		g_oldSetupCurItem = 0;
		MainMenu();
	}else if (MD_OHRANA == g_curBarValue)
	{	//  вход в режим охраны здесь
		Ohrana_();
	}
	else if (MD_HELLO == g_curBarValue)
	{
		EnablePoll(0);	//  отключаем протокольный опрос - если надо, включат в Setup
		PrepareToMode(MD_HELLO);
	}
}

//  переход к позиции "Значение" и его вычисление
void GetTetradeFromAdr(void)
{
	unsigned char* adr;
	unsigned char k, pos, was1, byte;

	g_szInput[g_InputPos] = 0;
	SetCaret(LINE_2_START_POS + 7);		//  переход ко второму значению
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

//  обработчик нажатия клавиши # Enter
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
			return;										//  уход от ClearInput
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
	{	//  а зачем это надо?
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
			return;									//  уход от ClearInput
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
			//  щелчек по галке - изменить галку!
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
			//  щелчек по галке - изменить галку!
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
			//  неправильное значение BaudRate
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
			//  неправильное знаечние BaudRate
			PrepareToMode(MD_SETUP_SENSORS_NEWBAUDRATE);
			return;
		}

		eeprom_write_byte_1((void*)EADR_USART_BAUDRATE, setupItem);
		uart_init();
		MenuSensors(0, 0);
	}else
	if (MD_SETUP_SENSORS_LIST == mode)
	{	//  ентер на каком-либо устройстве
		MenuDevice(1);
	}
	else
	if (MD_SETUP_SENSORS_LIST_DEVICE == mode)
	{
		if (SDN_HF_POWER == setupItem)
		{
			HFON;
			//SwitchDeviceOhrana();	//  сменить режим охраны устройства g_curDev
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
	{	//   изменить режим линии
		g_checkedItem = g_SetupCurItem;
		ChangeLineMode();
		MenuTel();
	}else
	if (MD_SETUP_TEL_AUTO_DIAL_NUMBERS == mode)
	{	//  режим редактирования номера автодозвона
		PrepareToMode(MD_SETUP_TEL_AUTO_DIAL_NUMBER);
		return;								//  уход от ClearInput
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
	{	//  переназначенние функций портов модуля датчиков
		ChangePortPurpose();
		builtinSensorsPollInterval = 0;	//  отключаем опрос
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
			SetCaret(LINE_2_START_POS + 7);		//  переход ко второму значению
			g_InputPos = 7;
			return;								//  уход от ClearInput
		}
		else
		{
			//g_SetupItem хранит номер входа, на котором нажали ентер
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
	{	//  конец редактирования номера автодозвона
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
			//  режим смены пароля на вход в Setup
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
			return;								//  уход от ClearInput
		}else
		{	//  перенести в память новое значение тетрады байт
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

//  режим проверки работы АЦП
#ifdef NEWVG
			//  новый смысл режима - показ значений Uпит, Uдат и пр.
			PrepareToMode(MD_SETUP_SENSORS_LIST_DEVICE_SENS_VALUE);
			return;								//  уход от ClearInput
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
				return;								//  уход от ClearInput
			}
#endif
		}else
		if (SRN_TIMEOUT == setupItem)
		{
			PrepareToMode(MD_SETUP_SENSORS_LIST_DEVICE_SENSOR_TIMEOUT1);
		}
	}else
	if (MD_SETUP_SENSORS_LIST_DEVICE_SENSOR_TIMEOUT1 == mode)
	{	//  изменили таймаут возобновления реакции на срабатывание датчика
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
	g_LastKey = key;					//  запоминаем последнюю нажатую клавишу

	if (VK_CANCEL == key)
	{
//#ifdef DEBUG
/*
		if (MD_OHRANA == mode)
		{	//  нажатие Esc в режиме охраны генерирует отладочное сообщение от датчиков
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
			{	//  отмена постановки под охрану
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
			builtinSensorsPollInterval = 0;	//  остановим внутренний опрос
			HFOFF;
			EnablePoll(0);					//  отключение протокольного опроса
			PrepareToMode(MD_HELLO);
			defferedOhranaEvent = 0;		//  отмена возможных отложенных реакций
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
			NumerKey(VK_SHARP);			//  впечатывает символ паузы
		else;

		//  случайное срабатывание?????????????????????????
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
			if (!delaySetOhrana)		//  если не ждем постановки под охрану
				Setup();
		}else
			LeftRight(VK_BACKSPACE);
	}
	else if ((VK_LEFT == key) || (VK_RIGHT == key))
	{
		if (!delaySetOhrana)		//  если не ждем постановки под охрану
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
		if (!delaySetOhrana)		//  если не ждем постановки под охрану
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
				SendVoice(0xFFFF);			//  начать проигрывание
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
	//unsigned char len = strlen(g_szInput) - интересно,
		//  но вводить отдельную переменную накладно

	//  переписываем столько, сколько может быть символов(без терминатора)
	epmwbl(&g_szInput, (unsigned char*)EADR_SETUP_PSWD, MAX_PASSWORD_LEN);

	if (!strlen(g_szInput))
		g_szSetupPassword[0] = 0;
	else
		//  переписываем столько, сколько ввели
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

	if (!ring)							//  после перекидки первой строки
	{									//  кнопок на PD7 проблемы со звуком
		ScanKbd();						//  сканирование клавиатуры

	//#ifdef DEBUG
		if (!useLocalDTMF)
	//#endif
			HandleKeys(getcha());		//  выполнение команд
	//#ifdef DEBUG
		else
		{
			unsigned char getcha_ = getcha();
			if (getcha_)
				DTMFCmdStorer(ConvertVirtualToDTMFCode(getcha_));
		}
	//#endif
	}

	//  событие оставила функция SensorsStateChangeNotify, вызванная в контекста
	//  прерывания SIG_UART0_RECV
	if (g_DefferedWork & _BV(DW_SENDOHRANAEVENT))
	{
		if (MD_OHRANA == g_Mode)
		{
			if (advancedLineMode)
				AutoDial();
			else
				AutoDial2();
		}
		//  AutoDial должна сделать все, что может и даже больше
		//  после нее не должно оставаться единичных битов в g_DefferedSensorsState
		//  если единичные биты остались, то :
		//  	1) не дозвонились или не получили подтверждения
		//		2) датчики сработали уже во время возврата из AutoDial

		//  Поэтому после выхода из AutoDial событие DW_SENDOHRANAEVENT не установлено
		//

		if (!defferedOhranaEvent)
		{	//  если не отложили на потом

			if (!g_DefferedSensorsState)
				//  сообщили обо всех датчиках
				g_DefferedWork &= ~_BV(DW_SENDOHRANAEVENT);
		}
	}

	goto ww;
}

//  не возвращает, пока в линии присутствует сигнал "mode"
char WaitForModePresent(unsigned char timeout, unsigned char mode,
	unsigned char displaymode)
{
	char wfRes;
	unsigned char localTimeOut = timeout;

	while (mode == linemode)
	{
		StartLineAnalize(0);					//  запустить анализ

		//  ожидаем завершения и показываем текущий режим
		wfRes = WaitForLineModeDefining(timeout, displaymode);

		if (WR_TIMEOUT == wfRes)
			return WR_TIMEOUT;
		else
		if ((WR_LM == wfRes) && (mode == linemode))
		{
			if (timeout)
			{	//  отсчитываем свой таймаут
				//  900 миллисекунд вместо 1000 - а потому, что WaitForLineModeDefining
				//  для определения состояния сама в себе затрачивает время
				//delayms(900);
				localTimeOut--;

				if (!localTimeOut)
					return WR_TIMEOUT;
			}
		}

		//  а если цикл пойдет по второму кругу, timeout ждать заново?
	}

	return WR_LM;
}

//  ожидает завершение определения состояния сигнала а линии...
char WaitForLineModeDefining(unsigned char timeout, unsigned char mode)
{
	unsigned char k;

	if (timeout)
		g_secCnt = timeout;

	while (!linemode)
	{	//  цикл ожидания завершения определения состояния сигнала а линии...

		ClearSecondLine();

		if (linemode)
			return WR_LM;						//  чтобы пораньше вернуться

		for (k = 0; k < 30; k++)
		{
			delayms(10);
			if (linemode)
				return WR_LM;					//  чтобы пораньше вернуться
		}

		OutStr(g_linemodes[mode]);

		if (linemode)
			return WR_LM;

		for (k = 0; k < 30; k++)
		{
			delayms(10);
			if (linemode)
				return WR_LM;					//  чтобы пораньше вернуться
		}

		if (timeout)							//  если таймер вообще запускали
			if (!g_secCnt)						//  и он весь протикал
				return WR_TIMEOUT;
	}

	return WR_LM;
}

void SheduleOhranaEvent(void)
{
	if (g_DefferedSensorsState)			//  все таки не получилось - отложим
	{
		if (!g_DefferStage)
			defferedOhranaEvent = 180;			//  три минуты
		else
		if (1 == g_DefferStage)
			defferedOhranaEvent = 600;			//  10 минут
		else
		if (2 == g_DefferStage)
			defferedOhranaEvent = 1800;			//  полчаса
		else
			defferedOhranaEvent = 3600;			//  час

		g_DefferStage++;

		if (10 == g_DefferStage)
		{	//  через 10 часов нарушение потеряет актуальность
			g_DefferStage = 0;
			g_DefferedSensorsState = 0;
		}
	}

	g_DefferedWork &= ~_BV(DW_SENDOHRANAEVENT);
}
//  дозванивается до нужного номера и ожидает поднятия трубки
//  если трубку за время Х не подняли
char AutoDialToNumber(unsigned char n)
{
	char wfRes = 0;
	unsigned char tryCount = 0;		//  число попыток услышать ответ станции

	g_ShowTime = FALSE;
	TELUP;					//  трубку поднимаем
	g_dtmfPSWOk = 0;				//  пароль в новом сеансе еще не введен
#ifdef REDUCE
	g_dtmfPasswordPermission = 0;
#endif
	delayms(100);

hoho:

	tryCount++;

	//  определяем состояние линии - а есть ли ответ станции
	g_wasOne = 0;
	StartLineAnalize(0);

	//  ожидание установления определенного состояния линии
	wfRes = WaitForLineModeDefining(g_params[PARAM_OS_WAIT_TIME], 0);

	if (WR_TIMEOUT == wfRes)
		return LM_OS_ABSENT;					//  недождались "Ответ станции"

	if (LM_OS_OR_DOWN == linemode)
	{	//  есть ответ станции

		//  DialNumber всегда набирает только g_szLastNumber
		memcpy(g_szLastNumber, g_szAutoDialNumbers[n], MAX_INPUT);

		ClearSecondLine();
		OutData(n + 0x31);
		OutData('.');
		OutStr(g_szLastNumber);

		g_needHello = 1;					//  выдавать необходимо
		g_wasRequest = 0;

		EIMSK &= ~_BV(INT6);
		DialNumber();

		g_wasOne = 0;
		StartLineAnalize(0);

		wfRes = WaitForLineModeDefining(g_params[PARAM_FIRST_GUDOK_WAIT_TIME], 0);
		if (WR_TIMEOUT == wfRes)
			return LM_FAULT;				//  недождались начала любого гудка ????

		ShowLineMode();

		if (LM_BUSY == linemode)
			return LM_BUSY;

		if (LM_KPV == linemode)
		{	//  пошел КПВ по трубам...(из Гоблина Руслана)
			g_wasOne = 1;

haha:		wfRes = WaitForModePresent(7, LM_KPV, LM_KPV);
			if (WR_TIMEOUT == wfRes)
				return LM_KPV;			//  значит хужаин так и не взял трубку

			if (LM_BUSY == linemode)
			{
				//  для соток - пока шел КПВ, девушка говорила,что телефон выключен
				return LM_FAULT;
			}

			if ((LM_UP == linemode) || (LM_OS_OR_DOWN == linemode))
			{	//  хозяин поднял трубку (или поднял и положил (или отключил))

				ShowLineMode();
				return linemode;
			}

			goto haha;
		}
	}else
		if (2 > tryCount)
			goto hoho;

	return 34;				//  придумать, что это такое...
}

//  выполняется в главном цикле программы - вызывается из main
void AutoDial(void)
{
	unsigned char number = 0, res;

	if (!g_sendOhranaEventMsgInProgress)
	{	//  если в данный момент разговор не происходит

		ClearScreen();
		OutStr(szTrevoga);
		ClearInput();
		SetCaret(LINE_2_START_POS);

		modeBeforeZvonok = MD_OHRANA;		//  чтобы положив, оказались в охране
		g_attemptCounterTheSameNumber = 0;

		//  цикл по номерам записной книжки
		while (number < MAX_TEL_AUTO_DIAL_NUMBERS_ITEMS)
		{
			if ('.' == g_szAutoDialNumbers[number][1])
				break;						//  попался первый пустой номер

			g_DefferedSensorsState_ = 0;
			res = AutoDialToNumber(number);	//производим дозвон (удар. на первом слоге)

			if (LM_UP == res)
			{	// взял таки трубку

				g_sendOhranaEventMsgInProgress = 1;

				if (g_needHello)
				{
					//  в течении этого времени должен подтвердить поднятие - пароль
					confirmUp = MAX_RAZGOVOR_TIME;
					SendVoice(VOICE_HELLO, 0);		//  посылаем ему приветствие
					while(ring);				//  ждем, пока звук кончится
				}

hehe:			ShowLineMode();
				delayms(200);

				//  гудок буд-то бы был, чтобы первый же следующий длинный
				//	служил сигналом OS_OR_DOWN
				g_wasOne = 1;

				//  весь сеанс протекает в следующей функции
				WaitForModePresent(0, linemode, LM_UP);

				if ((LM_OS_OR_DOWN == linemode) || (LM_BUSY == linemode))
				{	//  положили трубку - LM_BUSY - городская, LM_OS~ - наша
					ShowLineMode();
					delayms(1000);
					RingOff();

					if (!g_wasRequest)
					{	//  может и подняли, но запроса состояния не давали
						g_attemptCounterTheSameNumber++;
						if (3 == g_attemptCounterTheSameNumber)
						{	//  хватит с одним номером возиться - есть другие
							number++;
							g_attemptCounterTheSameNumber = 0;
						}
						//continue;
					}else
					if (g_DefferedSensorsState)
					{	//  или не нажали подтверждение "2" (которое обнуляет)
						//  или сработало, пока болтали

						g_attemptCounterTheSameNumber++;
						if (3 == g_attemptCounterTheSameNumber)
						{	//  хватит с одним номером возиться - есть другие
							number++;
							g_attemptCounterTheSameNumber = 0;
						}
					}else
						break;			//  все удачно - все получилось
				}else
				{

					goto hehe;
				}
			}
			else
			{	//  трубку не подняли
				RingOff();
				ShowLineMode();

				g_attemptCounterTheSameNumber++;
				if (3 == g_attemptCounterTheSameNumber)
				{	//  хватит с одним номером возиться - есть другие
					number++;
					g_attemptCounterTheSameNumber = 0;
				}
/*
				if (LM_KPV == res)
				{	//  трубку не взял, вышли по таймауту или просто занято
					//  звоним еще раз


				}else
				if ((LM_BUSY == res) || (LM_FAULT == res))
				{	//  трубку не взял, вышли по таймауту или просто занято
					//  или аппарат выключен, или недоступен или вне зоны обслуживания

					g_attemptCounterTheSameNumber++;
					if (3 == g_attemptCounterTheSameNumber)
					{	//  хватит с одним номером возиться - есть другие
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
	TELUP;						//  трубку поднимаем
	g_dtmfPSWOk = 0;				//  пароль в новом сеансе еще не введен
#ifdef REDUCE
	g_dtmfPasswordPermission = 0;
#endif
	delayms(100);

	//  DialNumber всегда набирает только g_szLastNumber
	memcpy(g_szLastNumber, g_szAutoDialNumbers[n], MAX_INPUT);

	ClearSecondLine();
	OutData(n + 0x31);
	OutData('.');
	OutStr(g_szLastNumber);

	g_needHello = 0;					//  выдавать не нужно
	g_wasRequest = 0;				//  запроса не было ("1")
	g_wasConfirm = 0;

	EIMSK &= ~_BV(INT6);
	DialNumber();
	confirmUp = MAX_RAZGOVOR_TIME;
}

void AutoDial2(void)
{
	unsigned char number = 0, someNumberTry, contin;

	//if (!g_sendOhranaEventMsgInProgress)
	{	//  если в данный момент разговор не происходит

		someNumberTry = 0;

		ClearScreen();
		OutStr(szTrevoga);
		ClearInput();
		SetCaret(LINE_2_START_POS);

		modeBeforeZvonok = MD_OHRANA;		//  чтобы положив, оказались в охране
		g_attemptCounterTheSameNumber = 0;

		//  цикл по номерам записной книжки
		while (number < MAX_TEL_AUTO_DIAL_NUMBERS_ITEMS)
		{
			if ('.' == g_szAutoDialNumbers[number][1])
				break;					//  попался первый пустой номер

			g_DefferedSensorsState_ = 0;
			RingOff();
			contin = 0;
			AutoDialToNumber2(number);	//производим дозвон (удар. на первом слоге)

			//  ждем поднятия и запроса результатов сработки
			while (!g_wasRequest)
			{
				if (!confirmUp)
				{	//  пока ждали - таймаут
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

			//  дождались подтверждения поднятия - заводим таймер снова
			confirmUp = MAX_RAZGOVOR_TIME;

			//  ждем нажатия подтверждения уведомления ("2")
			while (!g_wasConfirm)
			{
				if (!confirmUp)
				{	//  пока ждали - таймаут
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

//  сохраняет номер автодозвона номер number в EEPROM...
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
		{	//  если HF уже нажимали - отключаем режим HF

			SetSpeakerVolume(0);
			PORTD &= ~PWA;

			if (g_telon)
			{	//  при поднятой трубке отключаем только динамик

			}else
			{	//  при опущеной трубке разрываем линию
				PORTF &= ~TELON;
				PrepareToMode(MD_HELLO);
			}
		}else
		{	//  если нажимаем первый раз
			PORTD |= PWA;
			SetSpeakerVolume(g_speakerVolume * 16);

			if (!g_telon)
			{	//  если трубка лежит - имитируем поднятие трубки
				PORTF |= TELON;
				//  и переходим в режим прямого набора номера
				Telephone();
			}
			else
				;//  нажатие клавиши HF при поднятой трубке влияет только на динамик
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

	OutStr(szPoisk);		//  Поиск
	StartProgress();
	HideCaret();

	for (k = 3; k < 5; k++)
	{
		//REDUCEClearSecondLine();
		itoa(k, g_szInput, 10);

		g_CurDev.adr = k;

		// подключаемся к устройству и ждем ответа от него - максимум 1 секунда
		if (ConnectToDevice())
		{
			//REDUCEOutStr(strcat(g_szInput, sz_est));	//  - есть

			delayms(300);

			//  устанавливает в состояние охраны и ждет подтверждения
			if (SetOhranaState(k, 1))
			{
				//REDUCEClearSecondLine();
				//  Устр под охраной
				//REDUCEOutStr(szUstrPodOhr);
				delayms(300);
			}
		}
		else
		{
			//REDUCEOutStr(strcat(g_szInput, sz_net));		//  - нет
			delayms(300);
		}
	}

	progress = FALSE;
	//  только теперь разрешаем опрос устройств
	EnablePoll(1);
#endif
	PrepareToMode(MD_OHRANA);

	g_ShowTime = TRUE;			//  можно, если не запрещено

	g_DefferedWork &= ~_BV(DW_SENDOHRANAEVENT);
	g_DefferedSensorsState = 0;
	g_DefferStage = 0;
	defferedOhranaEvent = 0;
	sensors = 0;
	dontStartConfirmTimer = 1;
	SendVoice(VOICE_SYSTEM_POD_OHRANOY, 1);

	HFON;				//  включаем питание датчиков
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
	{	//  обнуление буфера команд и сброс текущего режима
		g_dtmfMode = DTMF_MODE_NORMAL;
		g_dtmfCmdBufPos = 0;
	}else

	if (DTMF_CMD_ENTER == dtmfCod)
	{	//  выполнить DTMF команду, содержащуюся в буфере

		//  перезапустим - пытается набрать пароль, не будем ему вредить
		//if (!g_dtmfPSWOk)
			confirmUp = MAX_RAZGOVOR_TIME;

		if (g_dtmfPSWOk)
		{
			if ((DTMF_MODE_AUTO_DIAL_NUMBER_CHANGE == g_dtmfMode) &&
				(1 == g_dtmfCmdBufPos)
				)
			{	//  особый случай - ввод паузы в телефонный номер
				g_dtmfCmdBuf[g_dtmfCmdBufPos] = DIAL_PAUSE - 0x30;
				g_dtmfCmdBufPos++;
				return;
			}

			if (g_dtmfCmdBufPos)
				DTMFCommandHandle();		//  если в буфере команды есть команда
			else;
			// простое нажатие # повторяет предыдущую команду(если ее можно повторить)
		}else
		{	//  наверное это пароль
#ifdef REDUCE
			if (!g_dtmfPasswordPermission)
			{	// если пароль еще вводить нельзя

				if (DTMF_CMD_ENTER == g_lastDTMFCod)
				{
					if(++g_DTMFEnterPressCounter == 5)
					{	//  пять нажатий решетки подряд открывают дорогу ко вводу
						//  пароля, без этого считается, что вводятся команды
						//  усеченного режима
						g_dtmfPasswordPermission = TRUE;
						g_DTMFEnterPressCounter = 0;
					}
				}else
					//  все предыдущие накопленные нажатия обнуляются
					g_DTMFEnterPressCounter = 1;

			} else
#endif
			{	//  пароль вводить можно, он ли это

				//  ставим ноль-терминатор...
				g_dtmfCmdBuf[g_dtmfCmdBufPos] = 0;
				ClearScreen();

				if (!strncmp(g_dtmfCmdBuf, g_szSetupPassword,
					strlen(g_szSetupPassword)))
				{
					OutStr("Password - GOOD");
					g_dtmfPSWOk = 1;
					dtmfCod_ = VOICE_ARTEM_POZDRAVLYAEM;
					//  теперь может держать трубку сколько хочет
					confirmUp = 0;
				}
				else
				{
					OutStr("Password - BAD");
					//  выдать фразу - пароль неверен!
					dtmfCod_ = VOICE_ARTEM_WRONGPWD;
				}

				if ((VOICE_ARTEM_POZDRAVLYAEM == dtmfCod_) &&
					(g_DefferedSensorsState))
				{	//  если пароль введен правильно и есть инфа о сработавших
					//  датчиках, вместо подтверждения о правильном пароле
					//  дадим ему датчики...

					SendAllVoice();
				}else
					PlayVoice(dtmfCod_, 0);
			}
		}

		g_dtmfCmdBufPos = 0;			//  сброс буфера команд
	}
	else
	if (g_dtmfCmdBufPos < DTMF_CMD_BUF_LEN)
	{
		if (0x0A == dtmfCod)
			dtmfCod = 0;

		if ((!g_dtmfPSWOk) || (DTMF_MODE_PSWD_CHANGE == g_dtmfMode))
										//  если в текущем сеансе пароль не введен
			dtmfCod = dtmfCod + 0x30;	//  ожидаем только его, а сравнивать лучше
										//	строки с ASCII цифровыми символами

		g_dtmfCmdBuf[g_dtmfCmdBufPos] = dtmfCod;
		g_dtmfCmdBufPos++;
#ifdef REDUCE
		if (!g_dtmfPasswordPermission)
		{	//  раз пароль вводить нельзя, то не было подряд пяти нажатий #
			//  если не было пяти нажатий, сейчас действует усеченный режим
			if (0x31 == dtmfCod)
			{
				g_wasRequest = 1;
				confirmUp = MAX_RAZGOVOR_TIME;
				//  запоминаем, какие сообщения посылаем
				g_DefferedSensorsState_ = g_DefferedSensorsState;
				SendAllVoice();
			}else
			if (0x32 == dtmfCod)
			{	//  подтвердить услышанное сообщение

				//  ведь есть два типа звонка: входящий - запрос состояния и
				//  исходящий - автодозвон. Исходящий выполняется из функции
				//  AutoDial, которая вызывается из main, и нам надо корректно
				//	из нее выйти

				if (g_wasRequest)
				{	//  не просто нажали двойку, а осмысленно подтвердили

					//  сбрасываем только те биты, про датчики которых было сказано
					//  и подтверждено, ведь во время звука могли появится новые
					g_DefferedSensorsState &= ~g_DefferedSensorsState_;
					defferedOhranaEvent = 0;
					g_wasConfirm = 1;
				}

				ring = 0;
				linemode = LM_BUSY;
				confirmUp = 1;

			}else
			if (0x33 == dtmfCod)
			{	//  поставить или снять с охраны
			};

			//  сбрасываем буфер DTMF-команд
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
		{	//  команда для главного устройства
			if (2 == dtmfCmdBufPos)	//  двухсимвольные команды  - только
				b2 = 0;				//  для краткости, на самом деле добавляется ноль

			if (!b1)
			{
				if (2 == dtmfCmdBufPos)
				{	//  снять с охраны	 - а пароль?????
				}else
				{	//  специальные функции	00xx#
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
					{	// изменить внутреннюю переменную
						if (dtmfCmdBufPos > 5)
						{	// если есть и индекс переменной и новое значение

							param2 = 0;
							//  индекс переменной всегда двузначный
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
				//  повторить последнее сообщение о сработавших датчиках
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
			{	// 	смена пароля
				dtmfMode = DTMF_MODE_PSWD_CHANGE;
				dtmfTmp = 0;		//  ожидается ввод старого пароля

				//  SendVoice введите 	прежний пароль
			}else
			if (4 == b1)
			{	// 	системное время
				if (1 == b2)
					dtmfMode = DTMF_MODE_SET_TIME;
				else
					PlayVoice(VOICE_ARTEM_WRONGCMD, 0);//  сообщить время
			}else
			if (5 == b1)
			{	//  сброс secondCounter - исп. при автоподнятии в режиме телефона
				//confirmUp = 1;		//  чтобы стало нулем и повесило трубку (оно!)
				PlayVoice(VOICE_ARTEM_WRONGCMD, 0);
			}
			if (6 == b1)
			{
			}else
			if (7 == b1)		//  TODO возможность класть трубку без пароля!!!!!!!!
			{	//	завершение сеанса  - положить трубку
				//linemode = LM_BUSY;

				PlayVoice(VOICE_ARTEM_WRONGCMD, 0);
				//if (secondCounter)		//  когда система автоматически взяла трубку
				//	secondCounter = 1;
			}else
			if (8 == b1)
			{	//	выдать голосовое сообщения для проверки звучания
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
			{	//  изменить номер автодозвона
				if (!b2)
				{	//  продиктовать номер автодозвона
				}else
				if (1 == b2)
				{
					dtmfMode = DTMF_MODE_AUTO_DIAL_NUMBER_CHANGE;
					dtmfTmp = b3;	//  номер по этому номеру будет изменен...
				}else
					PlayVoice(VOICE_ARTEM_WRONGCMD, 0);
			}
		}else
			PlayVoice(VOICE_ARTEM_WRONGCMD, 0);
	}else
	if (DTMF_MODE_PSWD_CHANGE == dtmfMode)
	{	//  срабатывает три раза, после чего переход в DTMF_MODE_NORMAL
		if (!dtmfTmp)
		{	//  ожидается ввод старого пароля
			if (strncmp(g_dtmfCmdBuf, g_szSetupPassword, strlen(g_szSetupPassword)))
			{	//  старый пароль неверен
				dtmfTmp = 0;
				dtmfMode = DTMF_MODE_NORMAL;
				PlayVoice(VOICE_ARTEM_WRONGPWD, 0);
			}
			else
			{
				dtmfTmp = 1;
				//  SendVoice введите новый пароль
			}
		}else
		if (1 == dtmfTmp)
		{	//  ожидается ввод нового пароля первый раз
			memcpy(g_szInput, g_dtmfCmdBuf, dtmfCmdBufPos);
			tmp = dtmfCmdBufPos;				//  длина нового пароля
			g_szInput[dtmfCmdBufPos] = 0;
			dtmfTmp = 2;						//  последняя стадия смены пароля
			//  SendVoice подтвердите новый пароль
		}else
		if (2 == dtmfTmp)
		{	//  ожидается ввод нового пароля второй раз

			if (!(strncmp(g_dtmfCmdBuf, g_szInput, dtmfCmdBufPos)) &&
				(tmp == dtmfCmdBufPos))
			{
				//  SendVoice пароль изменен успешно
				dtmfTmp = 77;
				g_dtmfCmdBuf[dtmfCmdBufPos] = 0;
				g_szSetupPassword[dtmfCmdBufPos] = 0;
				epmwbl(&g_dtmfCmdBuf, (unsigned char*)EADR_DTMF_PSWD,
					sizeof(g_dtmfCmdBuf));
				strncpy(g_szSetupPassword, g_dtmfCmdBuf, dtmfCmdBufPos);
				PlayVoice(VOICE_ARTEM_POZDRAVLYAEM, 0);
			}
			else
			{	//  новый пароль подтвержден неправильно
				dtmfTmp = 0;
				PlayVoice(VOICE_ARTEM_WRONGPWD, 0);
			}

			dtmfMode = DTMF_MODE_NORMAL;
		}

	}else
	if (DTMF_MODE_AUTO_DIAL_NUMBER_CHANGE == dtmfMode)
	{	//  изменение номера автодозвона

		for (k = 0; k < dtmfCmdBufPos; k++)
			g_szAutoDialNumbers[dtmfTmp][k] = g_dtmfCmdBuf[k] + 0x30;

		g_szAutoDialNumbers[dtmfTmp][dtmfCmdBufPos] = 0;

		dtmfMode = DTMF_MODE_NORMAL;

	}else

	if (DTMF_MODE_SET_TIME == dtmfMode)
	{	//  меняет время
		dtmfMode = DTMF_MODE_NORMAL;

		if (dtmfCmdBufPos > 1)
		{	//  если в буфере есть хотя-бы новые минуты

			RTC_TIME rtc;
			ReadTime(&rtc);

			rtc.sec = 0;
			rtc.min = (b0 << 4) + b1;

			if (dtmfCmdBufPos > 3)
			{	//  если в буфере есть новые минуты И часы
				rtc.hour = (b2 << 4) + b3;

				if (dtmfCmdBufPos > 5)
				{	//  если в буфере есть новые минуты, часы и день
					rtc.day = (g_dtmfCmdBuf[4] << 4) + g_dtmfCmdBuf[5];

					if (dtmfCmdBufPos > 7)
					{	//  если в буфере есть новые минуты, часы, день и месяц
						rtc.month = (g_dtmfCmdBuf[6] << 4) + g_dtmfCmdBuf[7];

						if (dtmfCmdBufPos > 9)
							//  если в буфере есть вся дата
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

//  запустить анализ сигнала в линии
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
	//g_wasOne = 0;			//  был ли гудок(единица) в текущем сеансе определения
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

//  отбой - повесить трубку
void RingOff(void)
{
	g_wasEnteringCall = 0;

	LineDown();
//	HFOFF;
	//SetSpeakerVolume(0);

	//  если был в прогрессе процесс определения состояния сигнала в линии
	analizeTELIN = 0;
	TCCR1B = 0;
	ring = 0;
	linemode = LM_OS_OR_DOWN;
	confirmUp = 0;
	g_sendOhranaEventMsgInProgress = 0;
	g_nextVoice = 0;
	memset(&g_VoiceQueue, 0, sizeof(g_VoiceQueue));	// чистим очередь

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

//  вызывается из прерывания получения байта SIG_UART0_RECV
//  newstate - новое состояние портов устройства - бит=1 => соотв. датчик сработал
void SensorsStateChangeNotify(unsigned char newstate)
{
	unsigned char k, mask, adr;

#ifndef NEWVG
	adr = g_CurDev.adr - 3;
#else
	adr = 0;
#endif

	//  это нужно делать только в охране, а то не даст изменить g_CurDev.ports
	//  в состоянии редактирования портов в MD_SETUP_SENSORS_LIST_DEVICE_SETPORT
	if (MD_OHRANA == g_Mode)
		//  флаги разрешения всех восьми датчиков устройства
		//  пока получаем из EEPROM, но потом надо от устройств
		g_CurDev.ports = g_devList[adr].ports;

	if (MD_SETUP_SENSORS_LIST_DEVICE_SENS_VALUE == g_Mode)
	{
		if (g_CurDev.ports_state != newstate)
		{	//  когда не было условия, на больших скоростях опроса мерцание
			g_CurDev.ports_state = newstate;
			PrepareToMode(MD_SETUP_SENSORS_LIST_DEVICE_SENS_VALUE);
		}
	}
	else
	if (MD_SETUP_SENSORS_LIST_DEVICE_SETPORT == g_Mode)
	{	//  если в режиме мониторинга, то изменения будут видны на экране,
		//  если в режиме установки назначения портов, но ничего не изменится
		if (g_CurDev.ports_state != newstate)
		{	//  когда не было условия, на больших скоростях опроса мерцание
			g_CurDev.ports_state = newstate;
			PrepareToMode(MD_SETUP_SENSORS_LIST_DEVICE_SETPORT);
		}
	}
	else
	if (g_DefferedSensorsState != newstate)
	{	//  если дополнительно сработал какой-то датчик(ведь у нас могут быть
		//  уже установленные биты в g_DefferedSensorsState)

		//  просматриваем маску портов, разрешенных к обработке
		//  эта маска хранится в g_CurDev.ports, если
		//	соответствующий бит установлен, то этот порт запрещен
		//  ( вообщето надо наоборот, 1 - это вход, "I", но пока так...
		//ports = g_CurDev.ports;

		for (k = 0; k < 8; k++)
		{
			mask = 1 << k;

			if (newstate & mask)
			{	//  сработал датчик номер к в устройстве с адресом g_CurDev.adr

				//  реакция может быть запрещена двумя способами: или запрещена
				//  совсем или не кончился таймаут от срабатывания, проверяем оба
				if (!(g_CurDev.ports & mask) ||
					(g_devList[adr].sensorTimeout[k]))
				{
					newstate &= ~mask;	//  сбросим, как будто не сработал
				}
			/*
				//  обновляем значения времени реакции на повторное срабатывание
				if (g_devList[adr].sensorTimeout[k])

				//  но только, если истек предыдущий интервал задержки, иначе мы
				//  постоянно будем устанавливать задержку, не давая ей кончится
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
		{	//  выходит, сработали только запрещенные датчики, раз
			//  здесь ноль - мы сбросили их биты и получился ноль...
			return;
		}

		if ((g_sendOhranaEventMsgInProgress) && (LM_UP == linemode))
			SendAllVoice();
		else
		{
			//  прекращаем возможные отложенные звонки - звоним прямо сейчас!
			defferedOhranaEvent = 0;
			//  обработать событие DW_SENDOHRANAEVENT
			g_DefferedWork |= _BV(DW_SENDOHRANAEVENT);
			//  чтобы не уничтожить старое, а присовокупить новое
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

//  нажали цифровую клавишу на одном из номеров в списке номеров автодозвона
void AutoDialNumbersOperations(unsigned char key)
{
	unsigned char res;

	if (VK_1 == key)
	{	//  просто позвонить по этому номеру
		strcpy(g_szLastNumber, g_szAutoDialNumbers[g_SetupItem]);
		//REDUCERedial();
		modeBeforeZvonok = MD_SETUP_TEL_AUTO_DIAL_NUMBERS;
	}else
	if (VK_2 == key)
	{	//	опеределить максимальную паузу для этого номера

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
				//  гудок буд-то бы был, чтобы первый же следующий длинный
				//	служил сигналом OS_OR_DOWN
				g_wasOne = 1;

				//  весь разговор протекает в следующей функции
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
	{	//  передвинуть данный номер вверх
		if (g_SetupItem)
			ChangeMenuItems(-1);
	}else
	if (VK_6 == key)
	{	//  передвинуть данный вниз
		if (g_SetupItem < 8)
			ChangeMenuItems(1);
	}else
	if (VK_0 == key)
	{	//  стереть номер
		EraseNumber(g_SetupItem);
		MenuItemsInitAndDraw(g_SetupFirstLine, g_SetupCurItem);
	}
}

//  поставить звуковое сообщение номер voiceNum в очередь
void QueueVoice(unsigned short voicePos)
{
	unsigned char k;

	for (k = 0; k < MAX_VOICE_QUEUE + 2; k++)
		if (!g_VoiceQueue[k])
		{
			g_VoiceQueue[k] = VOICE_PAUSE;
			g_VoiceQueue[k + 1] = voicePos;
			g_VoiceQueue[k + 2] = 0;		//  там бывало что-то недосказанное
											//  и возникали накладки
			return;
		}
}

//  проиграть все звуковые сообщения в очереди
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
	{	//  три раза

		for (k = 0; k < 8; k++)
			if (g_DefferedSensorsState & (1 << k))
			{
				QueueVoice(1000 + 50*k);
			}
	}

	PlayQueue(0);
}

//  устанавливает "заводские" настройки
void SetDefaultSettings(void)
{
	unsigned char k;
	unsigned short i;
	unsigned char* adr;

	//  обнуление пароля
	memset(&g_szSetupPassword, 0x30, 4);
	g_szSetupPassword[4] = 0;
	epmwbl(&g_szSetupPassword, (unsigned char*)EADR_SETUP_PSWD,
		MAX_PASSWORD_LEN);

	//  обнуление таймаутов повторного срабатывания
	for (k = 0; k < MAX_DEVICE_COUNT; k++)
	{
		adr = (unsigned char*)EADR_DEVICE_LIST + k*sizeof(XDEVICE) + 3;

		for (i = (unsigned short)adr; i < (unsigned short)adr + 16; i = i + 2)
		{
			eeprom_write_byte_1((unsigned char*)i, 30);
			eeprom_write_byte_1((unsigned char*)i + 1, 0);
		}
	}

	// Baudrate По умолчанию - 9600
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
	OutData(0x23);					//  признак того, что сброс отработал

	g_CurDev.ports = 0;
	ChangePortPurpose();
}

//  очистить телефонный номер номер num в списке автодозвона
void EraseNumber(unsigned char num)
{
	unsigned char* sz;

	*(sz = &g_szAutoDialNumbers[num][0]) = 0x31 + num;		//  ставим номер номера

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

//	запустить прогрессбар на второй строке диспея
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

	//  задержка перед началом опроса датчиков для того, чтобы все устаканилось
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

//  набор одной цифры импульсным способом
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

//  измеряем напряжение питания в зависимости
void AdjustLimits(void)
{
#ifndef PROTEUS
	pwr = a2dConvert10bit(PF3);		//  измеряет Uпит
	unorm = pwr / 1.12;
	sensorsTopLimit = 1.05 * unorm;
	sensorsBottomLimit = 0.95 * unorm;
	sensorsTopLimit2 = sensorsTopLimit;
	sensorsBottomLimit2 = sensorsBottomLimit;
#endif
}

//	PASSIV MODULE PART

// подключает очередной датчик номер psvSensor_num к мультиплексору
void AttachSensor(void)
{
	PORTC &= ~(_BV(PC5) | _BV(PC6) | _BV(PC7));	//	вход A мультиплектора

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
		{	//  датчик сработал
			sensors |= _BV(k);
		}
		else
			sensors &= ~_BV(k);
	}
}
*/
//  опрос одного датчика и подготовка к следующему
//  должна вызываться в прерывании 3-4 раза в секунду
void PollBuiltinSensors2(void)
{
	unsigned short adc;

	AdjustLimits();

	if (g_CurDev.ports & (1 << psvSensor_num))
	{	//  опрашиваем только разрешенные датчики
#ifndef PROTEUS
		adc = a2dConvert10bit(PF2);

		dacRes[psvSensor_num] = adc;

		if ((adc < sensorsBottomLimit2
			//dacThresholdLOW[psvSensor_num]
			) ||
			(adc > sensorsTopLimit2
			//dacThresholdHIGHT[psvSensor_num]
			))
		{	//  датчик сработал
			sensors |= _BV(psvSensor_num);
		}
		else
			sensors &= ~_BV(psvSensor_num);
#else
		adc = psvSensor_num;
#endif
	}

	if (++psvSensor_num == 8)
	{	//  опросили все датчики

		psvSensor_num = 0;
		g_CurDev.adr = 0;	//  это первое устройство с нулевым адресом

		SensorsStateChangeNotify(sensors);
	}

	AttachSensor();
}

//  инициализация встроенных датчиков
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
