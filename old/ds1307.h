/*******************************************************************************************
* Объявления констант и включения заголовочных файлов для системы программных часов        *
* Версия 0.1                                                                               *
*******************************************************************************************/

#ifndef RTC_H
#define RTC_H

#ifndef __BCPP__
#include <string.h>
#endif

#define MIN_YEAR          2000                        //минимально допустимый год в системе
#define MAX_YEAR          (MIN_YEAR+99)               //максимально допустимый год в системе

#define DS1307_ADDR       0xd0                        //адрес микросхемы часов

#define DS1307_CONTROL    0x07                        //регистр управления
#define CH                0x07


typedef struct _rtc_type                              //Структура системного времени
{
unsigned char seconds;                                //секунды
unsigned char minutes;                                //минуты
unsigned char hours;                                  //часы
unsigned char day;                                    //день
unsigned char month;                                  //месяц
unsigned int year;                                    //год
}rtc_type;

#ifndef __BCPP__
//Прототипы функций
void rtc_init(void);
void rtc_set_time(unsigned char hours, unsigned char minutes, unsigned char seconds);
void rtc_set_date(unsigned char day, unsigned char month, unsigned int year);
void rtc_get_time(rtc_type *rtc_var);
#endif

#endif

