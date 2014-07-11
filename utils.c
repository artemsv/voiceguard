//  12.10.2006

#include "stdafx.h"
#include "string.h"
#include <interrupt.h>

#include "utils.h"
#include "lcd.h"
#include "spi.h"

void delayms(unsigned short ms)
{
	unsigned short k;
	unsigned short i;

	for (i = 0; i < ms; i++)
	{
		for (k = 0; k < 798; k++)
		{	
			asm("nop");
		}
	}
}

void delay_loop_2(uint16_t __count)
{
	__asm__ volatile (
		"1: sbiw %0,1" "\n\t"
		"brne 1b"
		: "=w" (__count)
		: "0" (__count)
	);
}

void epmrbl(void *pointer_ram,
                   const void *pointer_eeprom,
                   size_t size)

{
	eeprom_read_block(pointer_ram, pointer_eeprom, size);
}

void epmwbl(const void *pointer_ram,
                    void *pointer_eeprom,
                    size_t size)
{
	eeprom_write_block(pointer_ram, pointer_eeprom, size);
}

unsigned short epmrw(const uint16_t * adr)
{
	return eeprom_read_word(adr);
}

uint8_t eeprom_read_byte_1 (unsigned char* addr) 
{
  uint8_t result;
  asm volatile
      ( XCALL " __eeprom_read_byte_" _REG_LOCATION_SUFFIX CR_TAB
        "mov %1,__tmp_reg__"
       : "+x" (addr),
         "=r" (result)
       : );
  return result;
}

void eeprom_write_byte_1 (unsigned char *addr, unsigned char value)
{
  asm volatile (
         "mov __tmp_reg__,%1"      CR_TAB
         XCALL " __eeprom_write_byte_" _REG_LOCATION_SUFFIX
       : "+x" (addr)
       : "r"  (value)
       : "memory"
      );
}

void Error(unsigned char errcode)
{
	char ee[10] = {0};
	cli();
	ClearScreen();
	OutStr("Error ");
	OutStr(itoa(errcode, ee, 10));
}

void SetSpeakerVolume(unsigned char value)
{
	PORTB &= ~(1 << CSR);
	spiTransferByte(0x11);
	spiTransferByte(value);
	//spiTransferByte(0x21);
//	spiTransferByte(0x01);
	PORTB |= (1 << CSR);
}

unsigned short hitrmas[3] = {1, 16, 256};

unsigned char HexByte(unsigned char byte)
{
	if ((byte >= 0x30) && (byte <= 0x39))
		return byte - 0x30;
	else
	if ((byte >= 'A') && (byte <= 'F'))
		return byte - 'A' + 10;
	
	return byte;
}

unsigned short HexToInt(char* sz)
{
	unsigned char k, len;
	unsigned short res = 0;

	k = len = strlen(sz);
	//k = len;
	
	do
	{
		k--;
		res = res + HexByte(sz[k]) * hitrmas[len - k - 1];
	}while (k);

	return res;
}
