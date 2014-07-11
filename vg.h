//  27.09.2006

/*

разъемчик для X232 с питанием от USB

Б о о о о А		-	на плате
  1 2 3 4

  1 - белый провод зеленой пары
  2 - зеленый провод
  3 - синий провод синей пары
  4 - белый провод синей пары

*/

//#define PROTEUS

#define REDUCE

#define PWA			_BV(PD5)		//  управление питанием усилителя

#define SPEAKERON 	PORTD |= PWA
#define SPEAKEROFF	PORTD &= ~PWA

#define NEWVG
#include "CodeBlocksBUG.inc"

#define ADC_2		0x00
#define ADC_4		0x02
#define ADC_8		0x03
#define ADC_16		0x04
#define ADC_32		0x05
#define ADC_64		0x06
#define ADC_128		0x07

#define DTMF_INT_MAX_COUNT	5		//  число нажатий клавиш для получения информации
								//  о состоянии датчиков (DTMF - посылки)
#define SEND_RESULT_COUNT	200		//  длительность посылки результата хозяину
								//  измеренная в циклах ШИМ

//  адреса записей о срабатывании датчиков во флешки
/*
	1000		-		Сработал датчик номер один
	1050	29	-		Сработал датчик номер два
	1100		-			----------------- три
	1150		-			----------------- 4
	1200		-							  5
	1250		-							  6
	1300	34	-							  7
	1350	35	-							  8
	1400	40	-		Вас приветствует система VoiceGuard
	1450	29	-		Нарушений не обнаружено
	1500		-		Система находится под охраной
	1550		-		Система снята с охраны
*/

/*
	1080		-		страница с паузой		-	9 страниц
*/

#define DIAL_PAUSE						0xBE

//#define VOICE_PSWD_BAD					4000
//#define VOICE_CMD_WRONG					4000
//#define VOICE_PSWD_GOOD					4000
#define VOICE_PARAM_WRONG				4000
#define VOICE_PARAM_ACCEPT				4000
#define VOICE_SYSTEM_POD_OHRANOY		1500
#define VOICE_SYSTEM_SNYATA_S_OHRANY	1550
#define VOICE_HELLO					1400
#define VOICE_OK						1450
#define VOICE_PAUSE					1080

//  идентификаторы(по сути те же адреса) DTMF-сигналов
/*
#define VOICE_DTMF_0                   	1600
#define VOICE_DTMF_1                   	1602
#define VOICE_DTMF_2                   	1604
#define VOICE_DTMF_3                   	1606
#define VOICE_DTMF_4                   	1608
#define VOICE_DTMF_5                   	1610
#define VOICE_DTMF_6                   	1612
#define VOICE_DTMF_7                   	1614
#define VOICE_DTMF_8                   	1616
#define VOICE_DTMF_9                  	1618
#define VOICE_DTMF_POUND               	1620
#define VOICE_DTMF_STAR				1622
*/

#define VOICE_DTMF_BASE				1630
#define VOICE_DTMF_0					VOICE_DTMF_BASE
#define VOICE_DTMF_1					VOICE_DTMF_BASE + 6		//  1636
#define VOICE_DTMF_2					VOICE_DTMF_1 + 5		//	1641
#define VOICE_DTMF_3					VOICE_DTMF_2 + 6		//	1647
#define VOICE_DTMF_4					VOICE_DTMF_3 + 5		//	1652
#define VOICE_DTMF_5					VOICE_DTMF_4 + 6		//	1658
#define VOICE_DTMF_6					VOICE_DTMF_5 + 5		//	1663
#define VOICE_DTMF_7					VOICE_DTMF_6 + 6		//	1669
#define VOICE_DTMF_8					VOICE_DTMF_7 + 5		//	1674
#define VOICE_DTMF_9					VOICE_DTMF_8 + 6		//	1680

#define VOICE_DEZDEMONA				3950

#define VOICE_ARTEM_WRONGCMD			1700			//len = 19
#define VOICE_ARTEM_WRONGPWD			1719			//len = 18
#define VOICE_ARTEM_GOODBAY			1737			//len = 11
#define VOICE_ARTEM_POZDRAVLYAEM		1748			//len = 13

#define KBD_ANTI_JITTER_TIMEOUT		10

void StartRecord();
void StartPlay(unsigned short startPage, unsigned char speacAction);
/*SIGNAL(SIG_UART0_RECV);*/
void SpeakerOn(void);

void AnalizeInterval(void);
/*SIGNAL(SIG_OVERFLOW1);*/
/*SIGNAL(SIG_OVERFLOW0);*/
/*SIGNAL(SIG_OVERFLOW3);*/
/*SIGNAL(SIG_INTERRUPT4);*/
#ifndef REDUCE
//  прерывание поднятия трубки
/*SIGNAL(SIG_INTERRUPT5);*/
#endif
//  прерывание от входящего звонка (телефонная линия)
/*SIGNAL(SIG_INTERRUPT6);*/
//  прерывание от входящего звонка (GSM)
/*SIGNAL(SIG_INTERRUPT7);*/

void ReadEEPROMData(void);
void Init(void);
void ChangeMenuItems(signed char direction);
void Menu_(unsigned char mode, unsigned char maxMenuItems);
void MainMenu(void);
void MenuSensors(unsigned char SetupFirstLine, unsigned char SetupCurItem);
void MenuList(void);
void MenuDevice(unsigned char b);		//  1 - требовать инфу у устройства
void MenuListPort(void);
void MenuSensor(void);
void MenuTel(void);
void MenuVolume(void);
void MenuX232(unsigned char SetupFirstLine, unsigned char SetupCurItem);
void MenuSettings(void);
void MenuAutoDialNumbers(unsigned char SetupFirstLine);
void MenuItemsInitAndDraw(unsigned char SetupFirstLine, unsigned char SetupCurItem);
void DrawVolume(void);
void UpdateBarInterface(void);
void PrepareToMode(unsigned char mode);
void Setup(void);
void ClearInput(void);
void Backspace(void);
void Ohrana(void);
void Telephone(void);
unsigned char* GetItemName(unsigned char index);
void DrawMenuLine(unsigned char lineNo, unsigned char lineStartPos);
void DrawMenu(void);
void DeletePrevChar(void);
void LeftRight(unsigned char key);
void NumerKey(unsigned char key);
void Escape(void);
void DialNumber(void);
void Redial(void);
void GetTetradeFromAdr(void);
void EnterSetup(void);
void Enter(void);
void HandleKeys(unsigned char key);
char ChangeTime(void);
void ChangeSetupPassword(void);
void ChangeLineMode(void);
void ChangeVolume(void);
int  main (void);
char WaitForModePresent(unsigned char timeout, unsigned char mode,
	unsigned char displaymode);
char WaitForLineModeDefining(unsigned char timeout, unsigned char mode);
void SheduleOhranaEvent(void);
char AutoDialToNumber(unsigned char n);
void AutoDial(void);
void AutoDialToNumber2(unsigned char n);
void AutoDial2(void);
void TimeExpiried(void);
void SaveDialNumberToEEPROM(unsigned char number);
void HandsFree(void);
void Ohrana_(void);
void SendVoice(unsigned short startPage, unsigned char speacAction);
void DTMFCmdStorer(unsigned char dtmfCod);
void DTMFCommandHandle(void);
void setstart(unsigned short st);
void StartLineAnalize(unsigned char LineModeAnalizeResultToSPI);
void RingOff(void);
void ShowLineMode(void);
void SensorsStateChangeNotify(unsigned char newstate);
void DialDirectNumber(unsigned char key);
void AutoDialNumbersOperations(unsigned char key);
void QueueVoice(unsigned short voicePos);
void PlayQueue(unsigned char voice);
void SendAllVoice(void);
void SetDefaultSettings(void);
void EraseNumber(unsigned char num);
void ShowRunStr(void);
void StartProgress(void);
void ReduceOhrana(void);
void SendDTMFDigit(unsigned short voxpos);
void AdjustLimits(void);
void AttachSensor(void);
void PollBuiltinSensors(void);
void PollBuiltinSensors2(void);
void InitBuilinSensors(void);
void StartSetOhranaProcess(void);
void PlayVoice(unsigned short startPage, unsigned char speacAction);
void LineDown();
