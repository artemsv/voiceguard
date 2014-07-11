//  22.09.2006
#include "stdafx.h"
#include "kbd.h"

#define KBD_BUFFER_SIZE 15
#define KBD_BUFFER_FULL	0xFF	//  указывает на заполненный буфер
#define KBD_BUFFER_EMPTY	0xFF	//  указывает на пустой буфер
#define NUM_KEYS		20		//  количество клавиш на клавиатурке

//  массив содержит состояние кажд. клав. при предыдущем опросе
unsigned char keys_state[NUM_KEYS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//  буфер клавиатуры - содержит коды последних 16 нажатых клавиш
unsigned char kbd_buf[KBD_BUFFER_SIZE] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//  указатели на голову и хвост буфера
int keyHead, keyTail;

//  возвращает последний введенный символ или 0, если буфер клавы пуст

void InitKbd(void)
{
	keyHead = -1; 	//  буфер пустой
	keyTail = 0;		//  следующая позиция для вставки очередной клавиши
}

//  сканирует клавиатуру (вызывается из главного цикла)
unsigned char ScanKbd(void)
{
	unsigned char row,col,tmp1,fefe;
	unsigned char res = 0;

	for (row=0; row < 4; row++)
    {
		PORTA = 1 << row;		//  подали единицу на вертикальный ряд

		for (tmp1 = 0; tmp1 < 100; ++tmp1);

      	tmp1 = PINC;


//		if (PIND != 0x1B)			посадили на PD7 - сгорела
		if (PINB & 1)
			tmp1 |= _BV(PC4);
		else
			tmp1 &= ~_BV(PC4);

	//	delayms(10);

		//if (tmp1 == PINC)
	      	// See if any column is active (low):
		    for (col=0; col<5; col++)
			{
				fefe = col*4 + row;		//  fefe - номер клавиши
		    	if ((tmp1 & (1<<col)) == 0)//== (1 << col))
		        {
					if (keys_state[fefe])
					{
					//  если при предыдущем проходе была нажата, а сейчас отпущена
						AddKey(fefe);
						res = 1;
					}
		        }

		  	    keys_state[fefe] = tmp1 & (1<<col);
		    }
	}

	return res;
}

//  добавляет в буфер клавиатуры запись о нажатии клавиши
void AddKey(unsigned char key)
{
	unsigned char keyTail_ = keyTail;
	unsigned char keyHead_ = keyHead;

	if (keyTail_ == KBD_BUFFER_FULL)
	{	//  буфер переполнен - удаляем самую старую запись

		keyHead_++;
		if (keyHead == KBD_BUFFER_SIZE)
		{
			keyHead_ = 0;
			keyTail_ = KBD_BUFFER_SIZE - 1;
		}else
			keyTail_ = keyHead - 1;
	}

	kbd_buf[keyTail_] = key + 1;		//  чтобы 0 был пустой клавишей

	keyTail_++;

	if (keyHead_ == KBD_BUFFER_EMPTY)
		//  первая клавиша в буфере
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
		{	//  буфер опустел
			keyHead_ = KBD_BUFFER_EMPTY;
			keyTail = 0;
		}

		keyHead = keyHead_;
	}

	return res;
}


