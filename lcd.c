//  дата создания : 22.09.2006

#include "stdafx.h"
#include <stdio.h>
#include "utils.h"

#include "lcd.h"

unsigned char g_CaretPos = 0;

void Strobe()
{
	PORTG = PORTG | SET_E;
	delay_loop_2(1000);
	PORTG = PORTG & CLEAR_E;
}

void OutData(unsigned char b)
{
	PORTG = PORTG | SET_RS;
	PORTA = b;
	Strobe();
	//PORTA = 0xFF;
}

void OutCmd(unsigned char b)
{
	PORTG = PORTG & CLEAR_RS;
	PORTA = b;
	Strobe();
	//PORTA = 0xFF;
}

void OutStr(unsigned char* sz)
{
	while(*sz)
		OutData(*(sz++));
}

void SetCaret(unsigned char pos)
{
	//  1 line - address from 0x00 to 0x27
	//	2 line - address from 0x40 to 0x67
	OutCmd(pos | 0x80);		//  set DRAM Adress
	g_CaretPos = pos;
}

unsigned char GetCaretPos(void)
{
	return g_CaretPos;
}

void InitLCD()
{
	//OutCmd(0x06);
	OutCmd(0x3C);
	OutCmd(0x0C);
}
/*
void ClearScreen2()
{
	SetCaret(0x00);
	OutStr(EMPTY_LCD_STRING);
	SetCaret(0x40);
	OutStr(EMPTY_LCD_STRING);
	SetCaret(0x00);
}
*/
void ClearScreen(void)
{
	OutCmd(1);
	//OutCmd(3);
	delayms(4);				//  без этой паузы не работает
	//SetCaret(0x00);
}

void ShowCaret()
{
	OutCmd(0x0E);
}

void HideCaret()
{
	OutCmd(0x0C);
}

void ClearSecondLine()
{
	unsigned char k;

	SetCaret(LINE_2_START_POS);

	for (k = 0; k < MAX_INPUT; k++)
		OutData(0x20);

	SetCaret(LINE_2_START_POS);
}

unsigned char ReadByte(unsigned char adr)
{
	unsigned char res;

	OutCmd(adr | 0x80);

	DDRA = 0;
	PORTA = 0;

	PORTG = PORTG | SET_RS;
	PORTG = PORTG | SET_RW;
	delay_loop_2(1000);
	PORTG = PORTG | SET_E;
	delay_loop_2(1000);

	res = PINA;

	PORTG = PORTG & CLEAR_E;
	PORTG = PORTG & CLEAR_RW;

	DDRA = 0xFF;
	PORTA = 0xFF;

	return res;
}

void ReadStrFromScreen(unsigned char* sz)
{
	unsigned char k;

	for (k = LINE_2_START_POS; k < LINE_2_START_POS + LINE_LEN; k++)
		*sz++ = ReadByte(k);
}
