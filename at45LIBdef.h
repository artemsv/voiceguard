//Добавить в H фале строки:

#ifndef DATAFLASH_CS
#define DATAFLASH_CS 0x10
#define RESET_CS_DATAFLASH (PORTB |=DATAFLASH_CS)
#define SET_CS_DATAFLASH PORTB &= ~DATAFLASH_CS
#endif

//Это позволит вам у себя в проекте определить эти макросы, в тоже время в аш файле они не будут переопределены...
//Но, опять таки, Си файл должен будет быть включен в ваш проект не ввиде библиеотеки, а в виде исходного файла


//#include "..\..\_xCOMMON\_INC\_xOS_LOC.h"

#define 

typedef unsigned long xLong;
typedef unsigned short xWord;
typedef unsigned char xByte;

#ifndef _I_AT45DATAFLASH_DEF
  #pragma pack(1)
  typedef struct st_at45_Data
    { xLong maxMemSize;                           //объем DataFlash в байтах
      xWord maxBufSize;                           //размер буфера DataFlash
      xByte num;                                  //текущий номер буфера и порта
                                                  //SPI
    } st_at45_Data;
  #pragma pack()
  #pragma pack(2)
  typedef struct st_at45_density
    { xWord pageSize;                             //размер страницы/буфера
      xWord pageNum;                              //количество страниц
    } st_at45_density;
  #pragma pack()
  #define AT45_NUM_TYPE       7                   //кол-во известных типов
                                                  //DataFlash
  #define _I_AT45DATAFLASH_DEF 1
#endif
#ifndef _I_AT45CMD_DEF
//================================================
//  Groupe of SCK Mode depend
//================================================
//  Read Commands
//------------------------------------------------
  #define at45_ContArrayRead0                 0x68
  #define at45_ContArrayRead1                 0xE8
  #define at45_MainMemPageRead0               0x52
  #define at45_MainMemPageRead1               0xD2
  #define at45_Buf1Read0                      0x54
  #define at45_Buf1Read1                      0xD4
  #define at45_Buf2Read0                      0x56
  #define at45_Buf2Read1                      0xD6
  #define at45_StatusRegRead0                 0x57
  #define at45_StatusRegRead1                 0xD7
//================================================
//  Groupe of SCK Mode undepend
//================================================
//  Program and Erase Commands
//------------------------------------------------
  #define at45_Buf1Write                      0x84
  #define at45_Buf2Write                      0x87
  #define at45_Buf1toMainMemPageProg_w_Erase  0x83
  #define at45_Buf2toMainMemPageProg_w_Erase  0x86
  #define at45_Buf1toMainMemPageProg_wo_Erase 0x88
  #define at45_Buf2toMainMemPageProg_wo_Erase 0x89
  #define at45_PageErase                      0x81
  #define at45_BlockErase                     0x50
  #define at45_MainMemPageProgThrBuf1         0x82
  #define at45_MainMemPageProgThrBuf2         0x85
//------------------------------------------------
//  Additional Commands
//------------------------------------------------
  #define at45_MainMemPageToBuf1Transfer      0x53
  #define at45_MainMemPageToBuf2Transfer      0x55
  #define at45_MainMemPageToBuf1Compare       0x60
  #define at45_MainMemPageToBuf2Compare       0x61
  #define at45_AutoPageRewriteThrBuf1         0x58
  #define at45_AutoPageRewriteThrBuf2         0x59
//================================================
  #define AT45BUSY                            0x80
  #define AT45COMP                            0x40
  #define _I_AT45CMD_DEF   1
#endif
extern const st_at45_density at45DensTab[AT45_NUM_TYPE];
extern xByte at45ReadStatus(st_at45_Data *pntr);
extern xByte init_at45(st_at45_Data *pntr);
extern xByte at45WaitReady(st_at45_Data *pntr);
extern void at45MemToBufTransfer(st_at45_Data *pntr, xWord pageAddr, xByte numBuf);
extern xWord at45RndRead(st_at45_Data *pntr, xByte *pdst, xLong startAddr, xWord size);
extern xWord at45BufRead(xByte **pdst, xWord byteAddr, xWord size, xByte numBuf, xByte numSpi);
extern xWord at45BufWrite(xByte **psrc, xWord byteAddr, xWord size, xByte numBuf, xByte numSpi);
extern void at45BufToMemWrite(st_at45_Data *pntr, xWord pageAddr, xByte numBuf);
extern xWord at45RndWrite(st_at45_Data *pntr, xByte *psrc, xLong startAddr, xWord size);
extern void at45PageErase(st_at45_Data *pntr, xWord pageAddr);
extern xByte txrxByteSpi(xByte data, xByte num);
extern xByte initSpi(xByte mode, xByte num);
