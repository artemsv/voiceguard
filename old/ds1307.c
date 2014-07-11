/*******************************************************************************************
* ���������� ��� ���������� �����
DS1307
*******************************************************************************************/

#include "twi.h"
#include "ds1307.h"

rtc_type rtc_time;                                    //��������� �����

//������������� �����
void rtc_init(void)
{
  unsigned char temp;
  i2c_start();
  i2c_tx_addr(DS1307_ADDR|WR);
  i2c_tx(0);                                          //����� �������� ���������
  i2c_start();
  i2c_tx_addr(DS1307_ADDR|RD);
  temp=i2c_rx(NOT_ACK);                               //������ ���
  i2c_stop();

  if(temp & (1<<CH))                                  //���� ������� ����� ���������
    {
      rtc_set_time(0, 0, 0);                          //������������� �����
      rtc_set_date(1, 1, 2000);                       //������������� ����

      i2c_start();
      i2c_tx_addr(DS1307_ADDR|WR);
      i2c_tx(DS1307_CONTROL);
      i2c_tx(0x90);
      i2c_stop();

    }
}

//������������� ��������� �����
void rtc_set_time(unsigned char hours, unsigned char minutes, unsigned char seconds)
{
  i2c_start();
  i2c_tx_addr(DS1307_ADDR|WR);
  i2c_tx(0);
/*
  i2c_tx(dec2bcd(seconds));
  i2c_tx(dec2bcd(minutes));
  i2c_tx(dec2bcd(hours));
*/
  i2c_stop();
}

//������������� ��������� ����
void rtc_set_date(unsigned char day, unsigned char month, unsigned int year)
{
  i2c_start();
  i2c_tx_addr(DS1307_ADDR|WR);
  i2c_tx(4);
/*
  i2c_tx(dec2bcd(day));
  i2c_tx(dec2bcd(month));
  i2c_tx(dec2bcd(year-2000));
*/
  i2c_stop();
}

//�������� ������� ����� � ��������� � rtc_var
void rtc_get_time(rtc_type *rtc_var)
{
  i2c_start();
  i2c_tx_addr(DS1307_ADDR|WR);
  i2c_tx(0);
  i2c_start();
  i2c_tx_addr(DS1307_ADDR|RD);

  rtc_var->seconds=(i2c_rx(ACK));
  rtc_var->minutes=(i2c_rx(ACK));
  rtc_var->hours=(i2c_rx(ACK) & 0x3f);
/*
  rtc_var->seconds=bcd2dec(i2c_rx(ACK));
  rtc_var->minutes=bcd2dec(i2c_rx(ACK));
  rtc_var->hours=bcd2dec(i2c_rx(ACK) & 0x3f);
*/
  i2c_rx(ACK);

  rtc_var->day=(i2c_rx(ACK));
  rtc_var->month=(i2c_rx(ACK));
  rtc_var->year=(i2c_rx(NOT_ACK)) + 2000;
/*
  rtc_var->day=bcd2dec(i2c_rx(ACK));
  rtc_var->month=bcd2dec(i2c_rx(ACK));
  rtc_var->year=bcd2dec(i2c_rx(NOT_ACK)) + 2000;
*/
  i2c_stop();
}

