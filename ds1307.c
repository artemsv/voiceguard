//  24.09.2006

#include "stdafx.h"
#include "i2cmaster.h"
//#include "ds1307.h"
#include "lcd.h"

void InitRTC()
{
	i2c_init();         
	i2c_start(0xD0+I2C_WRITE);  
	i2c_write(0x07);
	i2c_write(0x90);
	i2c_stop();
}

void ReadTime(PRTC_TIME prtc)
{
	i2c_start_wait(0xD0+I2C_WRITE);  
	i2c_write(0x00);					//  устанавливаем адрес чтения
	i2c_start_wait(0xD0+I2C_READ);   
	prtc->sec = i2c_readAck();
	prtc->min = i2c_readAck();
 	prtc->hour = i2c_readAck();
	prtc->weekday = i2c_readAck();
	prtc->day = i2c_readAck();
	prtc->month = i2c_readAck();
	prtc->year = i2c_readNak();
	i2c_stop();            

	//  BCD --> DEC
	prtc->sec = (prtc->sec >> 4) * 10 + (prtc->sec & 0x0F);	
	prtc->min = (prtc->min >> 4) * 10 + (prtc->min & 0x0F);	
	prtc->hour = (prtc->hour >> 4) * 10 + (prtc->hour & 0x0F);
	prtc->day = (prtc->day >> 4) * 10 + (prtc->day & 0x0F);
	prtc->month = (prtc->month >> 4) * 10 + (prtc->month & 0x0F);
//	prtc->year = (prtc->year >> 4) * 10 + (prtc->year & 0x0F);
}


void UpdateTimePane(void)
{
	typedef unsigned char* pMas;

	RTC_TIME rtc;
	unsigned char m[4] = {0};
	unsigned char e;

	ReadTime(&rtc);
	SetCaret(0x40);

	for (e = 3; e > 1; e--)
	{
		if (((pMas)(&rtc.sec))[e - 1] < (unsigned char)10)
			OutData(0x30);
		
		OutStr(itoa(((pMas)(&rtc.sec))[e - 1], m, 10));
		
		if (e - 2)
		{
			if (rtc.sec & 1)
				OutData(':');
			else
				OutData(32);
		}
	}

	OutData(32);		//  разделительный пробел

	if (rtc.day < 10)
		OutData(0x30);

	OutStr(itoa(rtc.day, m, 10));
	OutData('.');

	if (rtc.month < 10)
		OutData(0x30);

	OutStr(itoa(rtc.month, m, 10));
	OutData('.');

	OutStr("200");
	OutStr(itoa(rtc.year, m, 10));
}

void WriteTime(PRTC_TIME prtc)
{
	i2c_start_wait(0xD0+I2C_WRITE);  
	i2c_write(0x00);					//  устанавливаем адрес чтения
	//i2c_start_wait(0xD0+I2C_READ);   
	i2c_write(prtc->sec);
	i2c_write(prtc->min);
 	i2c_write(prtc->hour);
	i2c_write(prtc->weekday);
	i2c_write(prtc->day);
	i2c_write(prtc->month);
	i2c_write(prtc->year);
	i2c_stop();            
}
