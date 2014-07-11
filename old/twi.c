/*******************************************************************************************
* Подпрограммы управления шиной I2C                                                        *
                                                        *
*******************************************************************************************/

//#include "E:\Programs\Develop\WinAVR\avr\include\avr\io.h"
#include "C:\WinAVR\avr\include\avr\io.h"

#define F_CLK	16000000
#include "twi.h"
#include <avr/delay.h>


unsigned char i2c_error;                              //если >0, то произошла ошибка при работе с I2C
unsigned char i2c_error_soft;                         //тоже самое, для программной шины

void __delay_cycles(int r )
{
	_delay_ms(1000);
}
//Инициализация программной шины I2C
void i2c_init_soft(void)
{
  I2C_DDR&=~(1<<SDA_LN);                              //изначально линии SDA
  I2C_DDR&=~(1<<SCL_LN);                              //и SCL в высокоимпедансном состоянии
  I2C_PORT&=~(1<<SDA_LN);                             //и на них поддерживается за счет внешних резисторов
  I2C_PORT&=~(1<<SCL_LN);                             //высокий уровень
  i2c_error_soft=0;                                   //изначально ошибок нет :-)
}

//Формирует условие "СТАРТ"
void i2c_start_soft(void)
{
  if(i2c_error_soft)
    return;
  scl_set(1);
  sda_set(0);
  scl_set(0);
}

//Формирует условие "СТОП"
void i2c_stop_soft(void)
{
  sda_set(0);
  scl_set(1);
  sda_set(1);
  if(i2c_error_soft)
    i2c_init_soft();
}

//Посылка байта
void i2c_tx_soft(unsigned char byte)
{
	unsigned char i;
  if(i2c_error_soft)
    return;
  for(i=0;i<8;i++)
    {
      if(byte&0x80)
        sda_set(1);
      else
        sda_set(0);
      scl_set(1);
      scl_set(0);
      byte<<=1;
    }
  sda_io(IN_LN);
  scl_set(1);
  i2c_error_soft=in_sda();
  scl_set(0);
  sda_io(OUT_LN);
}


//Прием байта, если last_byte=0, то принимаем последний байт и подтверждение от мастера не нужно
unsigned char i2c_rx_soft(unsigned char last_byte)
{
	unsigned char i;
  unsigned char data=0;
  unsigned char mask=0x80;
  if(i2c_error_soft)
    return 0;
  sda_io(IN_LN);

  for(i=0;i<8;i++)
    {
      scl_set(1);
      if(in_sda())
        data=data+mask;
      mask>>=1;
      scl_set(0);
    }
  sda_io(OUT_LN);
  if(last_byte)
    sda_set(0);
  else
    sda_set(1);
  scl_set(1);
  scl_set(0);
  return data;
}

//Возвращает уровень линии SDA
unsigned char in_sda(void)
{
  if(I2C_PIN&(1<<SDA_LN))
    return 1;
  else
    return 0;
}

//Устанавливает линию SDA на вход или выход
void sda_io(unsigned char io_c)
{
  if(io_c==IN_LN)
    I2C_DDR&=~(1<<SDA_LN);
  else
    I2C_DDR|=(1<<SDA_LN);
  __delay_cycles(I2C_T*F_CLK);
}

//Устанавливает уровень на линии SCL
void scl_set(unsigned char set_c)
{
  if(set_c)
    {
      I2C_DDR&=~(1<<SCL_LN);
      I2C_PORT&=~(1<<SCL_LN);
    }
  else
    {
      I2C_DDR|=(1<<SCL_LN);
      I2C_PORT&=~(1<<SCL_LN);
    }
  __delay_cycles(I2C_T*F_CLK);
}

//Устанавливает уровень на линии SDA
void sda_set(unsigned char set_c)
{
  if(set_c)
    {
      I2C_DDR&=~(1<<SDA_LN);
      I2C_PORT&=~(1<<SDA_LN);
    }
  else
    {
      I2C_DDR|=(1<<SDA_LN);
      I2C_PORT&=~(1<<SDA_LN);
    }
  __delay_cycles(I2C_T*F_CLK);
}


//Инициализация аппаратной шины I2C
void i2c_init(void)
{
  TWSR=0;
  TWDR=0;
  TWAR=0;
  TWBR = TWI_TWBR;                                    //установить скорость шины i2c
  TWCR = (1<<TWEN);                                   //включить модуль i2c
  i2c_error=0;                                        //первоначально ошибок нет
}

//Формирует условие "СТАРТ"
void i2c_start(void)
{
  if(i2c_error)
    return;
  TWCR=((1<<TWEN)|(1<<TWINT)|(1<<TWSTA));
  i2c_delay();
  if((TWSR != START)&&(TWSR != REP_START))            //если были пробемы с передачей старта
    i2c_error=1;                                      //то признак ошибки
}

//Формирует условие "СТОП"
void i2c_stop(void)
{
  TWCR = ((1<<TWEN)+(1<<TWINT)+(1<<TWSTO));
  if(i2c_error)
    i2c_init();
}

//Посылка байта
void i2c_tx(unsigned char byte)
{
  if(i2c_error)
    return;
  i2c_delay();
  TWDR = byte;
  TWCR = ((1<<TWINT)+(1<<TWEN));
  i2c_delay();
//  if(TWSR != MTX_DATA_ACK)                            //если нет подтверждения от приемника, то
//    i2c_error=1;                                      //признак ошибки
}

//Передает адрес приемника + RW/RD
void i2c_tx_addr(unsigned char adr)
{
  i2c_delay();                                     //Wait for TWI interrupt flag set
  TWDR = adr;
  TWCR=((1<<TWINT)|(1<<TWEN));      //Clear int flag to send byte
  i2c_delay();             //Wait for TWI interrupt flag set
 // if((TWSR != MTX_ADR_ACK)&&(TWSR != MRX_ADR_ACK))//If NACK received return
 //   i2c_error=1;
}


//Прием байта, если last_byte=0, то принимаем последний байт и подтверждение от мастера не нужно
unsigned char i2c_rx(unsigned char last_byte)
{
  if(i2c_error)
    return 0;
  i2c_delay();
  if(last_byte)                                       //если принимаем не последний байт, то
    TWCR = ((1<<TWINT)+(1<<TWEN)+(1<<TWEA));          //посылаем подтвержение приема
  else
    TWCR=((1<<TWINT)|(1<<TWEN));                      //иначе - нет
  i2c_delay();
  if(((TWSR != MRX_DATA_NACK)&&(last_byte == NOT_ACK))&&(TWSR != MRX_DATA_ACK))
    i2c_error=1;
  return TWDR;
}

//Задержка
static void i2c_delay(void)
{
	unsigned char i;
  if(i2c_error)
    return;
  for(i=0;i<100;i++)
    {
      if(TWCR&(1<<TWINT))
        return;
      __delay_cycles(0.000001*F_CLK);
    }
  i2c_error=1;
}




