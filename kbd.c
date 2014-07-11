//  22.09.2006
#include "stdafx.h"
#include "kbd.h"

#define KBD_BUFFER_SIZE 15
#define KBD_BUFFER_FULL	0xFF	//  ��������� �� ����������� �����
#define KBD_BUFFER_EMPTY	0xFF	//  ��������� �� ������ �����
#define NUM_KEYS		20		//  ���������� ������ �� �����������

//  ������ �������� ��������� ����. ����. ��� ���������� ������
unsigned char keys_state[NUM_KEYS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//  ����� ���������� - �������� ���� ��������� 16 ������� ������
unsigned char kbd_buf[KBD_BUFFER_SIZE] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//  ��������� �� ������ � ����� ������
int keyHead, keyTail;

//  ���������� ��������� ��������� ������ ��� 0, ���� ����� ����� ����

void InitKbd(void)
{
	keyHead = -1; 	//  ����� ������
	keyTail = 0;		//  ��������� ������� ��� ������� ��������� �������
}

//  ��������� ���������� (���������� �� �������� �����)
unsigned char ScanKbd(void)
{
	unsigned char row,col,tmp1,fefe;
	unsigned char res = 0;

	for (row=0; row < 4; row++)
    {
		PORTA = 1 << row;		//  ������ ������� �� ������������ ���

		for (tmp1 = 0; tmp1 < 100; ++tmp1);

      	tmp1 = PINC;


//		if (PIND != 0x1B)			�������� �� PD7 - �������
		if (PINB & 1)
			tmp1 |= _BV(PC4);
		else
			tmp1 &= ~_BV(PC4);

	//	delayms(10);

		//if (tmp1 == PINC)
	      	// See if any column is active (low):
		    for (col=0; col<5; col++)
			{
				fefe = col*4 + row;		//  fefe - ����� �������
		    	if ((tmp1 & (1<<col)) == 0)//== (1 << col))
		        {
					if (keys_state[fefe])
					{
					//  ���� ��� ���������� ������� ���� ������, � ������ ��������
						AddKey(fefe);
						res = 1;
					}
		        }

		  	    keys_state[fefe] = tmp1 & (1<<col);
		    }
	}

	return res;
}

//  ��������� � ����� ���������� ������ � ������� �������
void AddKey(unsigned char key)
{
	unsigned char keyTail_ = keyTail;
	unsigned char keyHead_ = keyHead;

	if (keyTail_ == KBD_BUFFER_FULL)
	{	//  ����� ���������� - ������� ����� ������ ������

		keyHead_++;
		if (keyHead == KBD_BUFFER_SIZE)
		{
			keyHead_ = 0;
			keyTail_ = KBD_BUFFER_SIZE - 1;
		}else
			keyTail_ = keyHead - 1;
	}

	kbd_buf[keyTail_] = key + 1;		//  ����� 0 ��� ������ ��������

	keyTail_++;

	if (keyHead_ == KBD_BUFFER_EMPTY)
		//  ������ ������� � ������
		keyHead_ = 0;

	if ((keyTail_ == keyHead_) || (keyTail_ == KBD_BUFFER_SIZE))
		keyTail_ = KBD_BUFFER_FULL;

	keyTail = keyTail_;
	keyHead = keyHead_;
}

unsigned char getcha()
{
	unsigned res = 0;
	unsigned char keyHead_ = keyHead;

	if (keyHead_ != KBD_BUFFER_EMPTY)
	{

		res = kbd_buf[keyHead_];

		keyHead_++;

		if (keyHead_ == KBD_BUFFER_SIZE)
			keyHead_ = 0;
		if (keyHead_ == keyTail)
		{	//  ����� �������
			keyHead_ = KBD_BUFFER_EMPTY;
			keyTail = 0;
		}

		keyHead = keyHead_;
	}

	return res;
}


