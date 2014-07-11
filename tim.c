//  ���� �������� : 22.09.2006
#include "stdafx.h"
#include "tim.h"

void InitTimer0(void)
{
    TCCR0 = _BV(CS02)|_BV(CS01)|_BV(CS00);        // div 1024
    TCNT0 = 0;
    TIMSK = TIMSK | _BV(TOIE0);
}

void InitTimer3(void)
{
	TCCR3B = _BV(CS02) | _BV(CS00);		//  �������� 1024

	//  ������� 16000000 ������� �� 1024, ���������� 15625
	//  15625 ����� � �������
	//  ������ ������� ������ ��������� �� 1 �� 15625 � ����� ��� �������
	//  ������� ���������������� ��������� 65536 - 15625 = 49911

//	OCR3A = TIMER3_SECOND_VALUE + TIMER3_200MS_VALUE;
	TCNT3 = TIMER3_SECOND_VALUE;//0x85ED;
	//TCNT3 = TIMER3_MAX_INTERVAL;

    ETIMSK = ETIMSK | _BV(TOIE3)/* | _BV(OCIE3A)*/;
}

void InitPWM(void)
{
	// 8 ��� ���, ������������ COM1B
	TCCR1A = _BV(COM1B0) | _BV(COM1B1) | _BV(WGM10);
	TCNT1 = 0;
	TIFR = TIFR | _BV(TOV1); 			// ����� ����� ����������  �������� 1
	TCCR1B = _BV(CS11) | _BV(CS11);		// ����. ��������� �������� 1 = 1
	OCR1B = 0x00;				// ��������� ��������� �������� ��������� B
	TIMSK = TIMSK | _BV(TOIE1);
}
