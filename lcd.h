//	26.09.2006

//  16 пробелов дл€ очистки экрана
#define EMPTY_LCD_STRING	"                "

#define SET_RS 		0x04
#define CLEAR_RS	0xFB

#define SET_E		2
#define CLEAR_E		~2

#define SET_RW		1
#define CLEAR_RW	~1

//#define ShowCaret() 	OutCmd(0x0E)

void Strobe(void);
void OutData(unsigned char b);
void OutCmd(unsigned char b);
void OutStr(unsigned char* sz);
void SetCaret(unsigned char pos);
unsigned char GetCaretPos(void);
void InitLCD(void);
//  обновл€ет экранчик, если есть чего обновл€ть
//void UpdateLCD(void);
//  вывод приветстви€
void SayHello(void);
void ClearScreen(void);
void ClearScreen2(void);
void ShowCaret(void);
void HideCaret(void);
void ClearSecondLine(void);
void ReadStrFromScreen(unsigned char* sz);
