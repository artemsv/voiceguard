/*******************************************************************************************
* Объявление констант и включения заголовочных файлов для системы TWI                      *
                                                       *
*******************************************************************************************/

#ifndef I2C_H
#define I2C_H

//#include "E:\Programs\Develop\WinAVR\avr\include\avr\io.h"
#include "C:\WinAVR\avr\include\avr\io.h"

#define TWI_TWBR 20                                  //Делитель скорости шины

#define WR                0                           //Запись данных
#define RD                1                           //Чтение данных

#define SDA_LN            PC2                         //линия SDA
#define SCL_LN            PC4                         //линия SCL
#define I2C_PIN           PINC                        //порт входа
#define I2C_DDR           DDRC                        //порт направления
#define I2C_PORT          PORTC                       //порт выхода

#define IN_LN             0                           //линия на вход
#define OUT_LN            1                           //линия на выход

#define ACK               1                           //посылать подтверждение приема (не последний байт)
#define NOT_ACK           0                           //не подтверждать прием (последний байт)

#define I2C_T             0.00001                     //период импульса синхронизации (1/I2C_T = частота шины)
                                                      //при программной реализации шины

//Прототипы функций
void i2c_init_soft(void);
void i2c_start_soft(void);
void i2c_stop_soft(void);
void i2c_tx_soft(unsigned char byte);
unsigned char i2c_rx_soft(unsigned char last_byte);
unsigned char i2c_rx(unsigned char last_byte);
unsigned char in_sda(void);
void sda_io(unsigned char io_c);
void scl_set(unsigned char set_c);
void sda_set(unsigned char set_c);

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_tx(unsigned char byte);
void i2c_tx_addr(unsigned char addr);
static void i2c_delay(void);

//General Master staus codes
#define START             0x08                        //START has been transmitted
#define REP_START         0x10                        //Repeated START has been
                                                      //transmitted
//Master Transmitter staus codes
#define MTX_ADR_ACK       0x18                        //SLA+W has been tramsmitted
                                                      //and ACK received
#define MTX_ADR_NACK      0x20                        //SLA+W has been tramsmitted
                                                      //and NACK received
#define MTX_DATA_ACK      0x28                        //Data byte has been tramsmitted
                                                      //and ACK received
#define MTX_DATA_NACK     0x30                        //Data byte has been tramsmitted
                                                      //and NACK received
#define MTX_ARB_LOST      0x38                        //Arbitration lost in SLA+W or
                                                      //data bytes
//Master Receiver staus codes
#define MRX_ARB_LOST      0x38                        //Arbitration lost in SLA+R or
                                                      //NACK bit
#define MRX_ADR_ACK       0x40                        //SLA+R has been tramsmitted
                                                      //and ACK received
#define MRX_ADR_NACK      0x48                        //SLA+R has been tramsmitted
                                                      //and NACK received
#define MRX_DATA_ACK      0x50                        //Data byte has been received
                                                      //and ACK tramsmitted
#define MRX_DATA_NACK     0x58                        //Data byte has been received
                                                      //and NACK tramsmitted
#endif


