/*******************************************************************************************
* ���������� �������� � ��������� ������������ ������ ��� ������� ����������� �����        *
* ������ 0.1                                                                               *
*******************************************************************************************/

#ifndef RTC_H
#define RTC_H

#ifndef __BCPP__
#include <string.h>
#endif

#define MIN_YEAR          2000                        //���������� ���������� ��� � �������
#define MAX_YEAR          (MIN_YEAR+99)               //����������� ���������� ��� � �������

#define DS1307_ADDR       0xd0                        //����� ���������� �����

#define DS1307_CONTROL    0x07                        //������� ����������
#define CH                0x07


typedef struct _rtc_type                              //��������� ���������� �������
{
unsigned char seconds;                                //�������
unsigned char minutes;                                //������
unsigned char hours;                                  //����
unsigned char day;                                    //����
unsigned char month;                                  //�����
unsigned int year;                                    //���
}rtc_type;

#ifndef __BCPP__
//��������� �������
void rtc_init(void);
void rtc_set_time(unsigned char hours, unsigned char minutes, unsigned char seconds);
void rtc_set_date(unsigned char day, unsigned char month, unsigned int year);
void rtc_get_time(rtc_type *rtc_var);
#endif

#endif

