#define F_CPU 16000000UL

/* UART baud rate */
#define UART_BAUD  9600

#include "stdafx.h"
#include "utils.h"
#include "uart.h"

const unsigned short baudrates[7] = {832, 416, 207, 103, 51, 34, 16};

void USART_Transmit(unsigned char data)
{
	/* Ожидание освобождения буфера передатчика  */
	while ( !( UCSR0A & (1 << UDRE0)) );
	/* Помещение данных в буфер, отправка данных */
	UDR0 = data;
}

void GSM_Transmit(unsigned char data)
{
	/* Ожидание освобождения буфера передатчика  */
	while ( !( UCSR1A & (1 << UDRE1)) );
	/* Помещение данных в буфер, отправка данных */
	UDR1 = data;
}

void uart_init(void)
{
	unsigned char br;
	UCSR0B = 0;
	//br = eeprom_read_word((const uint16_t *)EADR_USART_BAUDRATE);
	br = eeprom_read_byte_1((unsigned char*)EADR_USART_BAUDRATE);
	if (br > 7)
		br = 2;		//  9600 на 16 Мгц

//#if F_CPU < 2000000UL && defined(U2X)
//  UCSRA = _BV(U2X);             /* improve baud rate error by using 2x clk */
//  UBRRL = (F_CPU / (8UL * br)) - 1;
//#else
//  UBRR0L = (F_CPU / (16UL * br)) - 1;
//#endif

	UCSR0A = _BV(U2X);
	UBRR0L = baudrates[br];
  	UCSR0B = _BV(TXEN) | _BV(RXEN) /*| _BV(TXCIE) */| _BV(RXCIE); /* tx/rx enable */

  	//  GSM UART
	UCSR1A = _BV(U2X);
	UBRR1L = 16;				// 115200 gsm default speed
  	UCSR1B = _BV(TXEN) | _BV(RXEN) /*| _BV(TXCIE) */| _BV(RXCIE); /* tx/rx enable */
}

/*
int uart_putchar(char c, FILE *stream)
{

  if (c == '\a')
    {
      fputs("*ring*\n", stderr);
      return 0;
    }

  if (c == '\n')
    uart_putchar('\r', stream);
  loop_until_bit_is_set(UCSR0A, UDRE);
  UDR0 = c;

  return 0;
}
*/
/*
int
uart_getchar(FILE *stream)
{
  uint8_t c;
  char *cp, *cp2;
  static char b[RX_BUFSIZE];
  static char *rxp;

  if (rxp == 0)
    for (cp = b;;)
      {
	loop_until_bit_is_set(UCSR0A, RXC);
	if (UCSR0A & _BV(FE))
	  return _FDEV_EOF;
	if (UCSR0A & _BV(DOR))
	  return _FDEV_ERR;
	c = UDR0;

	if (c == '\r')	// behaviour similar to Unix stty ICRNL
	  c = '\n';
	if (c == '\n')
	  {
	    *cp = c;
	    uart_putchar(c, stream);
	    rxp = b;
	    break;
	  }
	else if (c == '\t')
	  c = ' ';

	if ((c >= (uint8_t)' ' && c <= (uint8_t)'\x7e') ||
	    c >= (uint8_t)'\xa0')
	  {
	    if (cp == b + RX_BUFSIZE - 1)
	      uart_putchar('\a', stream);
	    else
	      {
		*cp++ = c;
		uart_putchar(c, stream);
	      }
	    continue;
	  }

	switch (c)
	  {
	  case 'c' & 0x1f:
	    return -1;

	  case '\b':
	  case '\x7f':
	    if (cp > b)
	      {
		uart_putchar('\b', stream);
		uart_putchar(' ', stream);
		uart_putchar('\b', stream);
		cp--;
	      }
	    break;

	  case 'r' & 0x1f:
	    uart_putchar('\r', stream);
	    for (cp2 = b; cp2 < cp; cp2++)
	      uart_putchar(*cp2, stream);
	    break;

	  case 'u' & 0x1f:
	    while (cp > b)
	      {
		uart_putchar('\b', stream);
		uart_putchar(' ', stream);
		uart_putchar('\b', stream);
		cp--;
	      }
	    break;

	  case 'w' & 0x1f:
	    while (cp > b && cp[-1] != ' ')
	      {
		uart_putchar('\b', stream);
		uart_putchar(' ', stream);
		uart_putchar('\b', stream);
		cp--;
	      }
	    break;
	  }
      }

  c = *rxp++;
  if (c == '\n')
    rxp = 0;

  return c;
}
*/
