//  12.12.2006

#define DTMF_CMD_1			1
#define DTMF_CMD_2			2
#define DTMF_CMD_3			3
#define DTMF_CMD_4			4
#define DTMF_CMD_5			5
#define DTMF_CMD_6			6
#define DTMF_CMD_7			7
#define DTMF_CMD_8			8
#define DTMF_CMD_9			9
#define DTMF_CMD_0			0
#define DTMF_CMD_ESCAPE		11
#define DTMF_CMD_ENTER		12

void ISR_T1_Overflow(void);

void DialDigit(unsigned char number);
