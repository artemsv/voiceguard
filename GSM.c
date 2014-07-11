//  25.10.2006

#include "stdafx.h"
#include "GSM.h"


void GSM_Init(void)
{	// инициализация USART1

	UCSR1B = 0;

	UCSR1A = _BV(U2X);
//	UBRR1L = baudrates[br];		

  	UCSR1B = _BV(TXEN) | _BV(RXEN) /*| _BV(TXCIE) */| _BV(RXCIE); /* tx/rx enable */
}	

void GSM_TurnOn(void)
{
}

void GSM_TurnOff(void)
{
}

void GSM_SendATCommand(unsigned char cmd)
{

}


