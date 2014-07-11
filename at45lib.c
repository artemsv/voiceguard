#include "stdafx.h"
#include "at45LIBdef.h"

const st_at45_density at45DensTab[]=              //таблица типов DataFlash
//сигнатура+размер страницы, кол-во страниц
  { {((0x0C<<8)+264), 512},                       //1 Мегабит
    {((0x14<<8)+264), 1024},                      //2 Мегабит
    {((0x1C<<8)+264), 2048},                      //4 Мегабит
    {((0x24<<8)+264), 4096},                      //8 Мегабит
    {((0x2C<<8)+528), 4096},                      //16 Мегабит
    {((0x34<<8)+528), 8192},                      //32 Мегабит
    {((0x3C<<8)+1056), 8192}                      //64 Мегабит
  };
//================================================
// Чтение регистра статуса DataFlash
//------------------------------------------------
//аргументы: нет
//возвращает:значение регистра статуса
//================================================
xByte at45ReadStatus(st_at45_Data *pntr)
{ 
	SET_CS_DATAFLASH;                               //установим CS для DataFlash
  txrxByteSpi(at45_StatusRegRead1, pntr->num);    //передаем номер команды
  xByte res=txrxByteSpi(0, pntr->num);           //читаем регистр статуса
  RESET_CS_DATAFLASH;                             //сбросим CS для DataFlash
  return(res);
}
//================================================
// Инициализация переменных DataFlash
// определяем ее тип и объем
//------------------------------------------------
//аргументы :указатель на структуру переменных
//возвращает:ошибку инициализации
//================================================
xByte init_at45(st_at45_Data *pntr)
{ RESET_CS_DATAFLASH;                            //сбросим CS для DataFlash
//  initSpi(SPI_MASTER, AT45_SPI_NUM);             //проинициализируем SPI
  volatile xWord tmp=65535;
  short idx;
  while(--tmp);                                   //задержка 8,8мс
  while(--tmp);                                   //задержка 8,8мс
  tmp=16384;
  while(--tmp);                                   //задержка 2,2мс
  xWord sig=(at45ReadStatus(pntr)<<8)&0x3C00;     //считаем сигнатуру DataFlash
  tmp=sizeof(at45DensTab)>>2;                     //кол-во записей в таблице
  xByte res=1;                                    //результат диагностики
  for (idx=0; idx<tmp; idx++)               //ищем в таблице сигнатуру
  { if ((at45DensTab[idx].pageSize&0x3C00)==sig)  //сигнатура совпала?
    { pntr->maxBufSize=at45DensTab[idx].pageSize&0x03FF;//ага, размер буфера
      xLong ltmp=at45DensTab[idx].pageNum;
      pntr->maxMemSize=ltmp*(xLong)(pntr->maxBufSize);//общий объем DataFlash
//      pntr->num=AT45_SPI_NUM;                     //номер порта SPI
      res=0;                                      //результат положительный
      break;
    }
  }
  if (res)                                        //DataFlash не опознана
  { pntr->maxBufSize=0;                           //обнулим все параметры
    pntr->maxMemSize=0;
    pntr->num=0;
  }
  return(res);                                    //вернем результат
}                                                 //идентификации
//================================================
// Ожидание готовности DataFlash,
// проверяем BUSY в регистре статуса
//------------------------------------------------
//аргументы: нет
//возвращает:значение регистра статуса
//================================================
xByte at45WaitReady(st_at45_Data *pntr)
{ xByte res=0;
  SET_CS_DATAFLASH;                               //установим CS для DataFlash
  txrxByteSpi(at45_StatusRegRead1, pntr->num);    //передаем номер команды
  while (!(res&AT45BUSY))                         //ждем пока не установится
  { res=txrxByteSpi(0, pntr->num);                //бит READY/nBUSY в регистре
  }                                               //статуса DataFlash
  RESET_CS_DATAFLASH;                             //сбросим CS для DataFlash
  return(res);
}
//================================================
// Чтение осн. памяти DataFlash в ее буфер
//------------------------------------------------
//аргументы: указатель на структуру данных DataFlash,
//           номер страницы
//возвращает:нет
//================================================
void at45MemToBufTransfer(st_at45_Data *pntr, xWord pageAddr, xByte numBuf)
{ xWord addr;
  addr=pageAddr;
  xWord tmp=(pntr->maxBufSize)>>8;                //выделим второй байт
  while (tmp)
  { addr<<=1;                                     //выровняем номер страницы
    tmp>>=1;
  }
  //----- проверяем готовность DataFlash ---------
  at45WaitReady(pntr);                            //ждем готовности DataFlash
  DelayAccess();                                  //задержка между обращениями
  //----- чтение основной памяти DataFlash -------
  SET_CS_DATAFLASH;                               //установим CS для DataFlash
  if (numBuf)
  { txrxByteSpi(at45_MainMemPageToBuf2Transfer, pntr->num);//передаем номер
  }                                               //команды
  else
  { txrxByteSpi(at45_MainMemPageToBuf1Transfer, pntr->num);//передаем номер
  }                                               //команды
  txrxByteSpi((xByte)(addr>>8), pntr->num);       //передаем старший байт номера
                                                  //страницы
  txrxByteSpi((xByte)addr, pntr->num);            //передаем младший байт номера
                                                  //страницы
  txrxByteSpi(0, pntr->num);                      //формальная передача
  RESET_CS_DATAFLASH;                             //сбросим CS для DataFlash
}
//================================================
// Чтение из DataFlash произвольного количества
// байт данных с произвольного адреса
//------------------------------------------------
//аргументы: указатель на структуру данных DataFlash,
//           указатель на буфер приемника данных,
//           стартовый адрес в DataFlash,
//           количество читаемых байт
//возвращает:количество прочитанных байт
//================================================
xWord at45RndRead(st_at45_Data *pntr, xByte *pdst, xLong startAddr, xWord size)
{ //xByte *pdst=pDst;
  xWord byteCntr=0;                               //счетчик считанных байт
  xWord pageAddr, byteAddr;
  if((startAddr+(xLong)size)>pntr->maxMemSize)    //урежем кол-во запрошенных
  { size=(xWord)(pntr->maxMemSize-startAddr);     //до реального размера
  }                                               //DataFlash
  if (size==0) return (0);                        //нет размера - нет записи
  pageAddr=(xWord)(startAddr/(xLong)pntr->maxBufSize);//получим номер страницы
  byteAddr=(xWord)(startAddr%(xLong)pntr->maxBufSize);//номер байта в странице
  xWord tmp=(pntr->maxBufSize)>>8;                //выделим второй байт
  while (tmp)
  { pageAddr<<=1;                                 //выровняем номер страницы
    tmp>>=1;
  }
  pageAddr+=(byteAddr>>8);                        //добавим второй байт
  //----- проверяем готовность DataFlash ---------
  at45WaitReady(pntr);                            //ждем готовности DataFlash
  DelayAccess();                                  //задержка между обращениями
  //----- чтение основной памяти DataFlash -------
  SET_CS_DATAFLASH;                               //установим CS для DataFlash
  txrxByteSpi(at45_ContArrayRead1, pntr->num);    //передаем номер команды
  txrxByteSpi((xByte)(pageAddr>>8), pntr->num);   //передаем старший байт номера
                                                  //страницы
  txrxByteSpi((xByte)pageAddr, pntr->num);        //передаем младший байт номера
                                                  //страницы и старший бит
                                                  //номера байта
  txrxByteSpi((xByte)byteAddr, pntr->num);        //передаем младший байт номера
                                                  //байта
  txrxByteSpi(0, pntr->num);                      //формальная передача
  txrxByteSpi(0, pntr->num);                      //формальная передача
  txrxByteSpi(0, pntr->num);                      //формальная передача
  txrxByteSpi(0, pntr->num);                      //формальная передача
  while (byteCntr<size)                           //читаем данные в буфер
  { *pdst++=txrxByteSpi(0, pntr->num);            //приемника данных в
    byteCntr++;                                   //соответствии с заданным
  }                                               //количеством
  RESET_CS_DATAFLASH;                             //сбросим CS для DataFlash
  return(byteCntr);
}
//================================================
// Запись в буфер DataFlash блока данных
//------------------------------------------------
//аргументы: адрес указателя буфера источника данных,
//           стартовый адрес байта в буфере DataFlash,
//           количество записываемых байт,
//           номер буфера
//возвращает:количество записанных байт
//================================================
xWord at45BufWrite(xByte **psrc, xWord byteAddr, xWord size, xByte numBuf, xByte numSpi)
{ xWord byteCntr=0;                               //счетчик считанных байт
  xByte *pSrc=*psrc;
  xWord cntr;
  DelayAccess();                                  //задержка между обращениями
  SET_CS_DATAFLASH;                               //установим CS для DataFlash
  if (numBuf)                                     //по номеру тек.буфера
  { txrxByteSpi(at45_Buf2Write, numSpi);          //передаем номер команды
  }
  else
  { txrxByteSpi(at45_Buf1Write, numSpi);          //передаем номер команды
  }
  txrxByteSpi(0, numSpi);                         //формальная запись
  txrxByteSpi((xByte)(byteAddr>>8), numSpi);      //старший байт адреса
                                                  //байта в буфере
  txrxByteSpi((xByte)byteAddr, numSpi);           //младший байт адреса
                                                  //байта в буфере
  for (cntr=0; cntr<size; cntr++)           //цикл записи в буфер
  { txrxByteSpi(*pSrc++, numSpi);                 //DataFlash
    byteCntr++;                                   //считаем кол-во записан. байт
  }
  *psrc=pSrc;                                     //вернем тек.указатель данных
  RESET_CS_DATAFLASH;                             //сбросим CS для DataFlash
  return(byteCntr);
}
//================================================
// Чтение из буфера DataFlash блока данных
//------------------------------------------------
//аргументы: адрес указателя буфера приемника данных,
//           стартовый адрес байта в буфере DataFlash,
//           количество запрошенных байт,
//           номер буфера
//возвращает:количество записанных байт
//================================================
xWord at45BufRead(xByte **pdst, xWord byteAddr, xWord size, xByte numBuf, xByte numSpi)
{ xWord byteCntr=0;                               //счетчик считанных байт
  xByte *pDst=*pdst;
  xWord cntr;
  DelayAccess();                                  //задержка между обращениями
  SET_CS_DATAFLASH;                               //установим CS для DataFlash
  if (numBuf)                                     //по номеру тек.буфера
  { txrxByteSpi(at45_Buf2Read1, numSpi);          //передаем номер команды
  }
  else
  { txrxByteSpi(at45_Buf1Read1, numSpi);          //передаем номер команды
  }
  txrxByteSpi(0, numSpi);                         //формальная запись
  txrxByteSpi((xByte)(byteAddr>>8), numSpi);      //старший байт адреса
                                                  //байта в буфере
  txrxByteSpi((xByte)byteAddr, numSpi);           //младший байт адреса
                                                  //байта в буфере
  txrxByteSpi(0, numSpi);                         //формальная запись
  for (cntr=0; cntr<size; cntr++)           //цикл чтения из буфера
  { *pDst=txrxByteSpi(0, numSpi);                 //DataFlash
    pDst++;                                       //инкремент указателя данных
    byteCntr++;                                   //считаем кол-во считан. байт
  }
  *pdst=pDst;                                     //вернем тек.указатель данных
  RESET_CS_DATAFLASH;                             //сбросим CS для DataFlash
  return(byteCntr);
}
//================================================
// Запись буфера DataFlash в ее осн. память
//------------------------------------------------
//аргументы: указатель на структуру данных DataFlash,
//           номер буфера для чтения
//возвращает:нет
//================================================
void at45BufToMemWrite(st_at45_Data *pntr, xWord pageAddr, xByte numBuf)
{ xWord addr;
  addr=pageAddr;
  xWord tmp=(pntr->maxBufSize)>>8;                //выделим второй байт
  while (tmp)
  { addr<<=1;                                     //выровняем номер страницы
    tmp>>=1;
  }
  //----- проверяем готовность DataFlash ---------
  at45WaitReady(pntr);                            //ждем готовности DataFlash
  DelayAccess();                                  //задержка между обращениями
  //----- чтение основной памяти DataFlash -------
  SET_CS_DATAFLASH;                               //установим CS для DataFlash
  if (numBuf)
  { txrxByteSpi(at45_Buf2toMainMemPageProg_w_Erase, pntr->num);//передаем номер
  }                                               //команды
  else
  { txrxByteSpi(at45_Buf1toMainMemPageProg_w_Erase, pntr->num);//передаем номер
  }                                               //команды
  txrxByteSpi((xByte)(addr>>8), pntr->num);       //передаем старший байт номера
                                                  //страницы
  txrxByteSpi((xByte)addr, pntr->num);            //передаем младший байт номера
                                                  //страницы
  txrxByteSpi(0, pntr->num);                      //формальная передача
  RESET_CS_DATAFLASH;                             //сбросим CS для DataFlash
}
//================================================
// Запись в DataFlash произвольного блока данных
//------------------------------------------------
//аргументы: указатель на структуру данных DataFlash,
//           указатель на буфер источника данных,
//           стартовый адрес в DataFlash,
//           количество записываемых байт
//возвращает:количество записанных байт
//================================================
xWord at45RndWrite(st_at45_Data *pntr, xByte *psrc, xLong startAddr, xWord size)
{ //static xByte *psrc;
  //psrc=pSrc;
  xWord byteCntr=0;                               //счетчик записанных байт
  xWord pageAddr, byteAddr, memSize, maxBufSize=pntr->maxBufSize;
  xLong addr, maxMemSize=pntr->maxMemSize;        //адрес для внутр.нужд
  if((startAddr+(xLong)size)>maxMemSize)          //урежем кол-во запрошенных
  { size=(xWord)(maxMemSize-startAddr);           //до реального размера
  }                                               //DataFlash
  if (size==0) return (0);                        //нет размера - нет записи
  while(byteCntr<size)
  { addr=startAddr+(xLong)byteCntr;
    pageAddr=(xWord)(addr/(xLong)maxBufSize);     //получим номер страницы
    byteAddr=(xWord)(addr%(xLong)maxBufSize);     //номер байта в странице
    memSize=maxBufSize-byteAddr;
    if ((size-byteCntr)<memSize) memSize=size-byteCntr;
    if ((byteAddr)||(memSize!=maxBufSize))        //если запись не с начала
    { at45MemToBufTransfer(pntr, pageAddr, 0);    //страницы или не до конца
                                                  //страницы, то сначала
                                                  //скопируем страницу в буфер
                                                  //DataFlash
      DelayAccess();                              //задержка между обращениями
      at45WaitReady(pntr);                        //ждем готовности DataFlash
    }
    DelayAccess();                                //задержка между обращениями
    //----- запись в буфер DataFlash -------
    byteCntr+=at45BufWrite(&psrc, byteAddr, memSize, 0, pntr->num);//запись в буфер
                                                  //DataFlash
    DelayAccess();                                //задержка между обращениями
    at45BufToMemWrite(pntr, pageAddr, 0);         //запись буфера DataFlash в ее
                                                  //осн.память
    DelayAccess();                                //задержка между обращениями
  }
  return(byteCntr);                               //вернем количество записанных
}                                                 //байт
//------------------------------------------------
// Стирание страницы DataFlash
//------------------------------------------------
//аргументы: указатель на структуру данных DataFlash,
//           номер страницы
//возвращает:нет
//------------------------------------------------
void at45PageErase(st_at45_Data *pntr, xWord pageAddr)
{ xWord addr;
  addr=pageAddr;
  xWord tmp=(pntr->maxBufSize)>>8;                //выделим второй байт
  while (tmp)
  { addr<<=1;                                     //выровняем номер страницы
    tmp>>=1;
  }
  //----- проверяем готовность DataFlash ---------
  at45WaitReady(pntr);                            //ждем готовности DataFlash
  DelayAccess();                         //задержка между обращениями
  //----- чтение основной памяти DataFlash -------
  SET_CS_DATAFLASH;                               //установим CS для DataFlash
  txrxByteSpi(at45_PageErase, pntr->num);         //передаем номер команды
  txrxByteSpi((xByte)(addr>>8), pntr->num);      //передаем старший байт номера
                                                  //страницы
  txrxByteSpi((xByte)addr, pntr->num);           //передаем младший байт номера
                                                  //страницы
  txrxByteSpi(0, pntr->num);                     //формальная передача
  RESET_CS_DATAFLASH;                             //сбросим CS для DataFlash
}
