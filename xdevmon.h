//  28.09.2006

/*
	��� ���������� �� ���� ����� ���� �����. ������� ����� �� ������������,
	����� 1 ������������� ����������, ���� �� ����, ����� 2 � ������ ����������,
	� ������ 3 - N ������������� ������ ����. ��� ����������� ����� �� ��������
	����� ������� ��������� �����, �� ����, ���� �� ���� ���� ���� ����, �� ������
	����� ����� 3.
*/

// 	��������� ���� ������� ������
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
	unsigned char marker;			//	������ A5
	unsigned char adr;				//  ����� �����������
	unsigned char type;				//  ��� ������(�������)
	unsigned char data[PACKET_SIZE - 3 * sizeof(unsigned char)];

//	unsigned char crc16;			//  ����������� �����

} PACKET, *PPACKET;

typedef struct
{
	unsigned char adr;
	unsigned char state;			//  ��������� ����������
	unsigned char ports;			//  ������������ ������

	//  ������� ��������� ������� �� ������������
	unsigned short sensorTimeout[8];
#ifdef NEWVG
	unsigned short ohrTimeout;		//  ������� ��� ���������� �� ������
#endif
} XDEVICE, *PXDEVICE;

typedef struct
{
	unsigned char g_xDevCount;
	unsigned char g_tryCount;
	unsigned char g_linkStatus;
	//  ����� ���������� ������������� ����������
	unsigned char g_x232CurPollDev;

} LINK_STATE, *PLINK_STATE;

typedef struct
{
	unsigned char adr;
	unsigned char state;			//  ��������� ����������
	unsigned char ports;			//  ������������ ������
	unsigned char ports_state;		//  ������� ��������� ������
	unsigned short threshold1;
	unsigned short threshold2;
	unsigned char mode;
	unsigned short sensorTimeout[8];
	unsigned char curSensor;
	unsigned short ohrTimeout;

} CUR_DEVICE, *PCUR_DEVICE;

// ���� ����� devStatus

#define	DS_OHRANA		0x00	// 1 - ��� �������
#define	DS_STATUS_LED	0x01	// 1 - ����� ��������� �������
#define	DS_MARKER		0x02	// 1 - ��������� ����� ���������� �����
#define	DS_CONNECTED	0x03	// 1 - ���������� ������������
#define	DS_PACKET		0x04	// 1 - ���� ����� ��� ���������
#define	DS_ADC			0x05	// 1 - ������ ��� ���������
#define	DS_SETOHRANA	0x06	// 1 - ���� ������ ������� ���������� ��� ������

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
