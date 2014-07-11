//  дата создания : 22.09.2006
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
	TCCR3B = _BV(CS02) | _BV(CS00);		//  делитель 1024

	//  частота 16000000 делится на 1024, получается 15625
	//  15625 тиков в секунду
	//  значит счетчик должен досчитать от 1 до 15625 и будет вам секунда
	//  счетчик инициализируется значением 65536 - 15625 = 49911

//	OCR3A = TIMER3_SECOND_VALUE + TIMER3_200MS_VALUE;
	TCNT3 = TIMER3_SECOND_VALUE;//0x85ED;
	//TCNT3 = TIMER3_MAX_INTERVAL;

    ETIMSK = ETIMSK | _BV(TOIE3)/* | _BV(OCIE3A)*/;
}

void InitPWM(void)
{
	// 8 бит ШИМ, используется COM1B
	TCCR1A = _BV(COM1B0) | _BV(COM1B1) | _BV(WGM10);
	TCNT1 = 0;
	TIFR = TIFR | _BV(TOV1); 			// сброс флага превышения  счётчика 1
	TCCR1B = _BV(CS11) | _BV(CS11);		// коэф. Пересчёта счётчика 1 = 1
	OCR1B = 0x00;				// обнуление выходного регистра сравнения B
	TIMSK = TIMSK | _BV(TOIE1);
}
