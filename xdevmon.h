//  28.09.2006

/*
	Все устройства на шине имеют свой адрес. Нулевой адрес не используется,
	адрес 1 присваивается компьютеру, если он есть, адрес 2 у самого устройства,
	а адреса 3 - N присваиваются блокам УВДМ. При подключении блока он получает
	самый младший незанятый адрес, то есть, если на шине один блок УВДМ, он всегда
	имеет адрес 3.
*/

// 	маркерный байт каждого пакета
#define PACKET_MARKER		0xA5
#define PACKET_SIZE			16
#define PACKET_DATA_SIZE	PACKET_SIZE - 3 * sizeof(unsigned char)

#ifdef NEWVG
	#define MAX_DEVICE_COUNT	1
#else
	#define MAX_DEVICE_COUNT	5
#endif

typedef struct tagPACKET
{
	unsigned char marker;			//	маркер A5
	unsigned char adr;				//  адрес утстройства
	unsigned char type;				//  тип пакета(команда)
	unsigned char data[PACKET_SIZE - 3 * sizeof(unsigned char)];

//	unsigned char crc16;			//  контрольная сумма

} PACKET, *PPACKET;

typedef struct
{
	unsigned char adr;
	unsigned char state;			//  состояние устройства
	unsigned char ports;			//  конфигурация портов

	//  таймаут повторной реакции на срабатывание
	unsigned short sensorTimeout[8];
#ifdef NEWVG
	unsigned short ohrTimeout;		//  таймаут при постановке на охрану
#endif
} XDEVICE, *PXDEVICE;

typedef struct
{
	unsigned char g_xDevCount;
	unsigned char g_tryCount;
	unsigned char g_linkStatus;
	//  номер очередного опрашиваемого устройства
	unsigned char g_x232CurPollDev;

} LINK_STATE, *PLINK_STATE;

typedef struct
{
	unsigned char adr;
	unsigned char state;			//  состояние устройства
	unsigned char ports;			//  конфигурация портов
	unsigned char ports_state;		//  текущее состояние портов
	unsigned short threshold1;
	unsigned short threshold2;
	unsigned char mode;
	unsigned short sensorTimeout[8];
	unsigned char curSensor;
	unsigned short ohrTimeout;

} CUR_DEVICE, *PCUR_DEVICE;

// биты байта devStatus

#define	DS_OHRANA		0x00	// 1 - под охраной
#define	DS_STATUS_LED	0x01	// 1 - горит светодиод статуса
#define	DS_MARKER		0x02	// 1 - ожидается прием маркерного байта
#define	DS_CONNECTED	0x03	// 1 - устройство подсоединено
#define	DS_PACKET		0x04	// 1 - есть пакет для обработки
#define	DS_ADC			0x05	// 1 - работа ЦАП разрешена
#define	DS_SETOHRANA	0x06	// 1 - идет отсчет таймера постановки под охрану

void InitDevices(void);
void SendPacket_(void);
void SendPacket(PPACKET packet);
void SendCommand(unsigned char adr, unsigned char type);
void HandleProtocol(void);
void HandleReply(void);
char ConnectToDevice(void);
void SendAll(unsigned char cmd);
void SwitchDeviceOhrana(void);
char SetOhranaState(unsigned char num, unsigned char action);
char GetCurDeviceInfo();
void ChangeDelayTimeBeforeSetOhrana(unsigned short timeout);
void ChangePortPurpose(void);
char GetRespond(PPACKET packet);
char GetSensorThreshold(unsigned char idex);
char ChangeSensorThreshold(unsigned char index);
void ChangeX232(unsigned char* valueType, unsigned char value);
void HandlePrivatePacket(void);
void SendManualPacket(unsigned char* sz);
void EnablePoll(unsigned char pollValue);
void ChangeSensorReactionTimeout(unsigned short newValue);
void SetDefBuiltinTimeout(unsigned short timeout);
