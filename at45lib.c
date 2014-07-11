#include "stdafx.h"
#include "at45LIBdef.h"

const st_at45_density at45DensTab[]=              //������� ����� DataFlash
//���������+������ ��������, ���-�� �������
  { {((0x0C<<8)+264), 512},                       //1 �������
    {((0x14<<8)+264), 1024},                      //2 �������
    {((0x1C<<8)+264), 2048},                      //4 �������
    {((0x24<<8)+264), 4096},                      //8 �������
    {((0x2C<<8)+528), 4096},                      //16 �������
    {((0x34<<8)+528), 8192},                      //32 �������
    {((0x3C<<8)+1056), 8192}                      //64 �������
  };
//================================================
// ������ �������� ������� DataFlash
//------------------------------------------------
//���������: ���
//����������:�������� �������� �������
//================================================
xByte at45ReadStatus(st_at45_Data *pntr)
{ 
	SET_CS_DATAFLASH;                               //��������� CS ��� DataFlash
  txrxByteSpi(at45_StatusRegRead1, pntr->num);    //�������� ����� �������
  xByte res=txrxByteSpi(0, pntr->num);           //������ ������� �������
  RESET_CS_DATAFLASH;                             //������� CS ��� DataFlash
  return(res);
}
//================================================
// ������������� ���������� DataFlash
// ���������� �� ��� � �����
//------------------------------------------------
//��������� :��������� �� ��������� ����������
//����������:������ �������������
//================================================
xByte init_at45(st_at45_Data *pntr)
{ RESET_CS_DATAFLASH;                            //������� CS ��� DataFlash
//  initSpi(SPI_MASTER, AT45_SPI_NUM);             //����������������� SPI
  volatile xWord tmp=65535;
  short idx;
  while(--tmp);                                   //�������� 8,8��
  while(--tmp);                                   //�������� 8,8��
  tmp=16384;
  while(--tmp);                                   //�������� 2,2��
  xWord sig=(at45ReadStatus(pntr)<<8)&0x3C00;     //������� ��������� DataFlash
  tmp=sizeof(at45DensTab)>>2;                     //���-�� ������� � �������
  xByte res=1;                                    //��������� �����������
  for (idx=0; idx<tmp; idx++)               //���� � ������� ���������
  { if ((at45DensTab[idx].pageSize&0x3C00)==sig)  //��������� �������?
    { pntr->maxBufSize=at45DensTab[idx].pageSize&0x03FF;//���, ������ ������
      xLong ltmp=at45DensTab[idx].pageNum;
      pntr->maxMemSize=ltmp*(xLong)(pntr->maxBufSize);//����� ����� DataFlash
//      pntr->num=AT45_SPI_NUM;                     //����� ����� SPI
      res=0;                                      //��������� �������������
      break;
    }
  }
  if (res)                                        //DataFlash �� ��������
  { pntr->maxBufSize=0;                           //������� ��� ���������
    pntr->maxMemSize=0;
    pntr->num=0;
  }
  return(res);                                    //������ ���������
}                                                 //�������������
//================================================
// �������� ���������� DataFlash,
// ��������� BUSY � �������� �������
//------------------------------------------------
//���������: ���
//����������:�������� �������� �������
//================================================
xByte at45WaitReady(st_at45_Data *pntr)
{ xByte res=0;
  SET_CS_DATAFLASH;                               //��������� CS ��� DataFlash
  txrxByteSpi(at45_StatusRegRead1, pntr->num);    //�������� ����� �������
  while (!(res&AT45BUSY))                         //���� ���� �� �����������
  { res=txrxByteSpi(0, pntr->num);                //��� READY/nBUSY � ��������
  }                                               //������� DataFlash
  RESET_CS_DATAFLASH;                             //������� CS ��� DataFlash
  return(res);
}
//================================================
// ������ ���. ������ DataFlash � �� �����
//------------------------------------------------
//���������: ��������� �� ��������� ������ DataFlash,
//           ����� ��������
//����������:���
//================================================
void at45MemToBufTransfer(st_at45_Data *pntr, xWord pageAddr, xByte numBuf)
{ xWord addr;
  addr=pageAddr;
  xWord tmp=(pntr->maxBufSize)>>8;                //������� ������ ����
  while (tmp)
  { addr<<=1;                                     //��������� ����� ��������
    tmp>>=1;
  }
  //----- ��������� ���������� DataFlash ---------
  at45WaitReady(pntr);                            //���� ���������� DataFlash
  DelayAccess();                                  //�������� ����� �����������
  //----- ������ �������� ������ DataFlash -------
  SET_CS_DATAFLASH;                               //��������� CS ��� DataFlash
  if (numBuf)
  { txrxByteSpi(at45_MainMemPageToBuf2Transfer, pntr->num);//�������� �����
  }                                               //�������
  else
  { txrxByteSpi(at45_MainMemPageToBuf1Transfer, pntr->num);//�������� �����
  }                                               //�������
  txrxByteSpi((xByte)(addr>>8), pntr->num);       //�������� ������� ���� ������
                                                  //��������
  txrxByteSpi((xByte)addr, pntr->num);            //�������� ������� ���� ������
                                                  //��������
  txrxByteSpi(0, pntr->num);                      //���������� ��������
  RESET_CS_DATAFLASH;                             //������� CS ��� DataFlash
}
//================================================
// ������ �� DataFlash ������������� ����������
// ���� ������ � ������������� ������
//------------------------------------------------
//���������: ��������� �� ��������� ������ DataFlash,
//           ��������� �� ����� ��������� ������,
//           ��������� ����� � DataFlash,
//           ���������� �������� ����
//����������:���������� ����������� ����
//================================================
xWord at45RndRead(st_at45_Data *pntr, xByte *pdst, xLong startAddr, xWord size)
{ //xByte *pdst=pDst;
  xWord byteCntr=0;                               //������� ��������� ����
  xWord pageAddr, byteAddr;
  if((startAddr+(xLong)size)>pntr->maxMemSize)    //������ ���-�� �����������
  { size=(xWord)(pntr->maxMemSize-startAddr);     //�� ��������� �������
  }                                               //DataFlash
  if (size==0) return (0);                        //��� ������� - ��� ������
  pageAddr=(xWord)(startAddr/(xLong)pntr->maxBufSize);//������� ����� ��������
  byteAddr=(xWord)(startAddr%(xLong)pntr->maxBufSize);//����� ����� � ��������
  xWord tmp=(pntr->maxBufSize)>>8;                //������� ������ ����
  while (tmp)
  { pageAddr<<=1;                                 //��������� ����� ��������
    tmp>>=1;
  }
  pageAddr+=(byteAddr>>8);                        //������� ������ ����
  //----- ��������� ���������� DataFlash ---------
  at45WaitReady(pntr);                            //���� ���������� DataFlash
  DelayAccess();                                  //�������� ����� �����������
  //----- ������ �������� ������ DataFlash -------
  SET_CS_DATAFLASH;                               //��������� CS ��� DataFlash
  txrxByteSpi(at45_ContArrayRead1, pntr->num);    //�������� ����� �������
  txrxByteSpi((xByte)(pageAddr>>8), pntr->num);   //�������� ������� ���� ������
                                                  //��������
  txrxByteSpi((xByte)pageAddr, pntr->num);        //�������� ������� ���� ������
                                                  //�������� � ������� ���
                                                  //������ �����
  txrxByteSpi((xByte)byteAddr, pntr->num);        //�������� ������� ���� ������
                                                  //�����
  txrxByteSpi(0, pntr->num);                      //���������� ��������
  txrxByteSpi(0, pntr->num);                      //���������� ��������
  txrxByteSpi(0, pntr->num);                      //���������� ��������
  txrxByteSpi(0, pntr->num);                      //���������� ��������
  while (byteCntr<size)                           //������ ������ � �����
  { *pdst++=txrxByteSpi(0, pntr->num);            //��������� ������ �
    byteCntr++;                                   //������������ � ��������
  }                                               //�����������
  RESET_CS_DATAFLASH;                             //������� CS ��� DataFlash
  return(byteCntr);
}
//================================================
// ������ � ����� DataFlash ����� ������
//------------------------------------------------
//���������: ����� ��������� ������ ��������� ������,
//           ��������� ����� ����� � ������ DataFlash,
//           ���������� ������������ ����,
//           ����� ������
//����������:���������� ���������� ����
//================================================
xWord at45BufWrite(xByte **psrc, xWord byteAddr, xWord size, xByte numBuf, xByte numSpi)
{ xWord byteCntr=0;                               //������� ��������� ����
  xByte *pSrc=*psrc;
  xWord cntr;
  DelayAccess();                                  //�������� ����� �����������
  SET_CS_DATAFLASH;                               //��������� CS ��� DataFlash
  if (numBuf)                                     //�� ������ ���.������
  { txrxByteSpi(at45_Buf2Write, numSpi);          //�������� ����� �������
  }
  else
  { txrxByteSpi(at45_Buf1Write, numSpi);          //�������� ����� �������
  }
  txrxByteSpi(0, numSpi);                         //���������� ������
  txrxByteSpi((xByte)(byteAddr>>8), numSpi);      //������� ���� ������
                                                  //����� � ������
  txrxByteSpi((xByte)byteAddr, numSpi);           //������� ���� ������
                                                  //����� � ������
  for (cntr=0; cntr<size; cntr++)           //���� ������ � �����
  { txrxByteSpi(*pSrc++, numSpi);                 //DataFlash
    byteCntr++;                                   //������� ���-�� �������. ����
  }
  *psrc=pSrc;                                     //������ ���.��������� ������
  RESET_CS_DATAFLASH;                             //������� CS ��� DataFlash
  return(byteCntr);
}
//================================================
// ������ �� ������ DataFlash ����� ������
//------------------------------------------------
//���������: ����� ��������� ������ ��������� ������,
//           ��������� ����� ����� � ������ DataFlash,
//           ���������� ����������� ����,
//           ����� ������
//����������:���������� ���������� ����
//================================================
xWord at45BufRead(xByte **pdst, xWord byteAddr, xWord size, xByte numBuf, xByte numSpi)
{ xWord byteCntr=0;                               //������� ��������� ����
  xByte *pDst=*pdst;
  xWord cntr;
  DelayAccess();                                  //�������� ����� �����������
  SET_CS_DATAFLASH;                               //��������� CS ��� DataFlash
  if (numBuf)                                     //�� ������ ���.������
  { txrxByteSpi(at45_Buf2Read1, numSpi);          //�������� ����� �������
  }
  else
  { txrxByteSpi(at45_Buf1Read1, numSpi);          //�������� ����� �������
  }
  txrxByteSpi(0, numSpi);                         //���������� ������
  txrxByteSpi((xByte)(byteAddr>>8), numSpi);      //������� ���� ������
                                                  //����� � ������
  txrxByteSpi((xByte)byteAddr, numSpi);           //������� ���� ������
                                                  //����� � ������
  txrxByteSpi(0, numSpi);                         //���������� ������
  for (cntr=0; cntr<size; cntr++)           //���� ������ �� ������
  { *pDst=txrxByteSpi(0, numSpi);                 //DataFlash
    pDst++;                                       //��������� ��������� ������
    byteCntr++;                                   //������� ���-�� ������. ����
  }
  *pdst=pDst;                                     //������ ���.��������� ������
  RESET_CS_DATAFLASH;                             //������� CS ��� DataFlash
  return(byteCntr);
}
//================================================
// ������ ������ DataFlash � �� ���. ������
//------------------------------------------------
//���������: ��������� �� ��������� ������ DataFlash,
//           ����� ������ ��� ������
//����������:���
//================================================
void at45BufToMemWrite(st_at45_Data *pntr, xWord pageAddr, xByte numBuf)
{ xWord addr;
  addr=pageAddr;
  xWord tmp=(pntr->maxBufSize)>>8;                //������� ������ ����
  while (tmp)
  { addr<<=1;                                     //��������� ����� ��������
    tmp>>=1;
  }
  //----- ��������� ���������� DataFlash ---------
  at45WaitReady(pntr);                            //���� ���������� DataFlash
  DelayAccess();                                  //�������� ����� �����������
  //----- ������ �������� ������ DataFlash -------
  SET_CS_DATAFLASH;                               //��������� CS ��� DataFlash
  if (numBuf)
  { txrxByteSpi(at45_Buf2toMainMemPageProg_w_Erase, pntr->num);//�������� �����
  }                                               //�������
  else
  { txrxByteSpi(at45_Buf1toMainMemPageProg_w_Erase, pntr->num);//�������� �����
  }                                               //�������
  txrxByteSpi((xByte)(addr>>8), pntr->num);       //�������� ������� ���� ������
                                                  //��������
  txrxByteSpi((xByte)addr, pntr->num);            //�������� ������� ���� ������
                                                  //��������
  txrxByteSpi(0, pntr->num);                      //���������� ��������
  RESET_CS_DATAFLASH;                             //������� CS ��� DataFlash
}
//================================================
// ������ � DataFlash ������������� ����� ������
//------------------------------------------------
//���������: ��������� �� ��������� ������ DataFlash,
//           ��������� �� ����� ��������� ������,
//           ��������� ����� � DataFlash,
//           ���������� ������������ ����
//����������:���������� ���������� ����
//================================================
xWord at45RndWrite(st_at45_Data *pntr, xByte *psrc, xLong startAddr, xWord size)
{ //static xByte *psrc;
  //psrc=pSrc;
  xWord byteCntr=0;                               //������� ���������� ����
  xWord pageAddr, byteAddr, memSize, maxBufSize=pntr->maxBufSize;
  xLong addr, maxMemSize=pntr->maxMemSize;        //����� ��� �����.����
  if((startAddr+(xLong)size)>maxMemSize)          //������ ���-�� �����������
  { size=(xWord)(maxMemSize-startAddr);           //�� ��������� �������
  }                                               //DataFlash
  if (size==0) return (0);                        //��� ������� - ��� ������
  while(byteCntr<size)
  { addr=startAddr+(xLong)byteCntr;
    pageAddr=(xWord)(addr/(xLong)maxBufSize);     //������� ����� ��������
    byteAddr=(xWord)(addr%(xLong)maxBufSize);     //����� ����� � ��������
    memSize=maxBufSize-byteAddr;
    if ((size-byteCntr)<memSize) memSize=size-byteCntr;
    if ((byteAddr)||(memSize!=maxBufSize))        //���� ������ �� � ������
    { at45MemToBufTransfer(pntr, pageAddr, 0);    //�������� ��� �� �� �����
                                                  //��������, �� �������
                                                  //��������� �������� � �����
                                                  //DataFlash
      DelayAccess();                              //�������� ����� �����������
      at45WaitReady(pntr);                        //���� ���������� DataFlash
    }
    DelayAccess();                                //�������� ����� �����������
    //----- ������ � ����� DataFlash -------
    byteCntr+=at45BufWrite(&psrc, byteAddr, memSize, 0, pntr->num);//������ � �����
                                                  //DataFlash
    DelayAccess();                                //�������� ����� �����������
    at45BufToMemWrite(pntr, pageAddr, 0);         //������ ������ DataFlash � ��
                                                  //���.������
    DelayAccess();                                //�������� ����� �����������
  }
  return(byteCntr);                               //������ ���������� ����������
}                                                 //����
//------------------------------------------------
// �������� �������� DataFlash
//------------------------------------------------
//���������: ��������� �� ��������� ������ DataFlash,
//           ����� ��������
//����������:���
//------------------------------------------------
void at45PageErase(st_at45_Data *pntr, xWord pageAddr)
{ xWord addr;
  addr=pageAddr;
  xWord tmp=(pntr->maxBufSize)>>8;                //������� ������ ����
  while (tmp)
  { addr<<=1;                                     //��������� ����� ��������
    tmp>>=1;
  }
  //----- ��������� ���������� DataFlash ---------
  at45WaitReady(pntr);                            //���� ���������� DataFlash
  DelayAccess();                         //�������� ����� �����������
  //----- ������ �������� ������ DataFlash -------
  SET_CS_DATAFLASH;                               //��������� CS ��� DataFlash
  txrxByteSpi(at45_PageErase, pntr->num);         //�������� ����� �������
  txrxByteSpi((xByte)(addr>>8), pntr->num);      //�������� ������� ���� ������
                                                  //��������
  txrxByteSpi((xByte)addr, pntr->num);           //�������� ������� ���� ������
                                                  //��������
  txrxByteSpi(0, pntr->num);                     //���������� ��������
  RESET_CS_DATAFLASH;                             //������� CS ��� DataFlash
}
