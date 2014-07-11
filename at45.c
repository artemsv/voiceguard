//  01.11.2006

//  28.11.2006 - «аработало!!!!!!! ѕомен€ли микросхему AT45DB161B


// 4096 страниц по 528 байт = 2 162 688 байт
// 1 блок = 8 страниц
// 512 блоков по 4224 байт  = 2 162 688 байт

#include "stdafx.h"
#include "at45.h"
#include "spi.h"

#define CLEARED _BV(ACIS1)
#define SELECT_AT45		cbi(AT45_PORT_OUT, DF_CHIP_SELECT_)
#define UNSELECT_AT45	sbi(AT45_PORT_OUT, DF_CHIP_SELECT_)


unsigned short wBufPos[2] = {0, 0};		//  позици€ в буфере записи дл€ очередного байта
unsigned short rBufPos[2] = {0, 0};		//  позици€ в буфере чтени€ дл€ очередного байта
unsigned char buf_select_read[2] = {BUFFER_1_READ, BUFFER_2_READ};
unsigned char buf_select_write[2] = {BUFFER_1_WRITE, BUFFER_2_WRITE};
unsigned char buf_select_to_page[2] = {B1_TO_MM_PAGE_PROG_WITH_ERASE,
										B2_TO_MM_PAGE_PROG_WITH_ERASE};
unsigned char page_select_to_buf[2] = {MM_PAGE_TO_B1_XFER, MM_PAGE_TO_B2_XFER};

void AT45Init(void)
{
	wBufPos[0] = 0;
	rBufPos[0] = 0;
}

//  устанавливает позицию чтени€ буфера на слово номер pos
void AT45SetReadBufferPos(unsigned char bufno, unsigned short pos)
{
	rBufPos[bufno] = pos;
}

void AT45SetWriteBufferPos(unsigned char bufno, unsigned short pos)
{
	wBufPos[bufno] = pos;
}

void AT45erase(void)
{
    unsigned int block_counter = 0;

    ACSR |= CLEARED;                           // set signal flag that new data has to be recorded next

    // interrupt disabled, SPI port enabled, master mode, MSB first,  SPI mode 3, Fcl/4
   // SPCR = _BV(SPE) | _BV(MSTR) | _BV(CPOL) | _BV(CPHA);//0x5C;

    while (block_counter < 512)
    {
		UNSELECT_AT45;
        SELECT_AT45;

        spiWriteByte(BLOCK_ERASE);
        spiWriteByte((char)(block_counter >> 3));
        spiWriteByte((char)(block_counter << 5));
        spiWriteByte(0x00);

        UNSELECT_AT45;

        block_counter++;
        while(!(AT45_PORT_IN & _BV(BUSY_PIN)));    // wait until block is erased
    }

    //SPCR = 0x00;
}

char AT45ReadStatus(void)
{
	char res;

	while(!(AT45_PORT_IN & _BV(BUSY_PIN)));

	UNSELECT_AT45;
	SELECT_AT45;

	spiWriteByte(0xD7);
	res = spiReadByte();

	UNSELECT_AT45;
	return res;
}

char AT45WriteByteToBuffer(unsigned char bufno, unsigned char byte)
{
	while(!(AT45_PORT_IN & _BV(BUSY_PIN)));

	UNSELECT_AT45;
	SELECT_AT45;

    spiWriteByte(buf_select_write[bufno]);	//  0x84

	spiWriteByte(0x00);
    spiWriteByte((char)(wBufPos[bufno] >> 8));
    spiWriteByte((char)wBufPos[bufno] & 0xFF);

	spiWriteByte(byte);

    UNSELECT_AT45;

	wBufPos[bufno]++;

	return 1;
}

char AT45WriteBufferToPage(unsigned char bufno, unsigned short pagenum)
{
	while(!(AT45_PORT_IN & _BV(BUSY_PIN)));

	//  эта функци€ выполн€ет запись буфера в страницу
	UNSELECT_AT45;
	SELECT_AT45;

	spiWriteByte(buf_select_to_page[bufno]);		//  0x83

	spiWriteByte((char)(pagenum >> 6));
	spiWriteByte((char)(pagenum << 2));
	spiWriteByte(0x00);

	UNSELECT_AT45;

	return 0;
}

char AT45ReadPageToBuffer(unsigned char bufno, unsigned short pagenum)
{
	while(!(AT45_PORT_IN & _BV(BUSY_PIN)));

	//  эта функци€ выполн€ет чтение страницы в buffer

	UNSELECT_AT45;
	SELECT_AT45;

	spiWriteByte(page_select_to_buf[bufno]);		//  0x53

	spiWriteByte((char)(pagenum >> 6));
	spiWriteByte((char)(pagenum << 2));
	spiWriteByte(0x00);

	UNSELECT_AT45;

	return 0;
}

//  измен€ет глобальную переменную rBufPos - позицию очередного
//  считываемого байта
unsigned char AT45ReadByteFromBuffer(unsigned char bufno)
{
	unsigned char res;

	//while(!(AT45_PORT_IN & _BV(BUSY_PIN)));

	UNSELECT_AT45;
	SELECT_AT45;

	spiWriteByte(buf_select_read[bufno]);		// 0x54
    spiWriteByte(0x00);
	spiWriteByte(rBufPos[bufno] >> 8);
	spiWriteByte(rBufPos[bufno] & 0xFF);
	spiWriteByte(0xFF);

	res = spiReadByte();

	UNSELECT_AT45;

	rBufPos[bufno]++;

	return res;
}

//  чтение произвольного байта с любой страницы
unsigned char AT45ReadByteFromPage(unsigned short pageno, unsigned short byteno)
{
	unsigned char res;

	while(!(AT45_PORT_IN & _BV(BUSY_PIN)));

	UNSELECT_AT45;
    SELECT_AT45;

	spiWriteByte(MAIN_MEMORY_PAGE_READ);		//  0x52
	spiWriteByte((char)(pageno >> 6));
	spiWriteByte((char)(pageno << 2) | (char)(byteno >> 8));
	spiWriteByte((char)(byteno & 0xFF));

	spiWriteByte(0xFF);
	spiWriteByte(0xFF);
	spiWriteByte(0xFF);
	spiWriteByte(0xFF);

	res = spiReadByte();

	UNSELECT_AT45;

	return res;
}

void AT45PageErase(unsigned short pagenum)
{
	while(!(AT45_PORT_IN & _BV(BUSY_PIN)));

	UNSELECT_AT45;
    SELECT_AT45;

	spiWriteByte(PAGE_ERASE);			//  0x81
	spiWriteByte((char)(pagenum >> 6));
    spiWriteByte((char)(pagenum << 2));
    spiWriteByte(0x00);

    UNSELECT_AT45;
}

void AT45FillBuffer(unsigned char bufno, unsigned char b)
{
	unsigned short k;
	wBufPos[bufno] = 0;

	while(!(AT45_PORT_IN & _BV(BUSY_PIN)));

	UNSELECT_AT45;
    SELECT_AT45;

    spiWriteByte(buf_select_write[bufno]);	//  0x84

	spiWriteByte(0x00);
    spiWriteByte(0);
    spiWriteByte(0);

	for (k = 0; k < 512; k++)
		spiWriteByte(b);

    UNSELECT_AT45;
}

void AT45ZeroPage(unsigned short pagenum)
{
	AT45FillBuffer(0, 0);
	AT45WriteBufferToPage(0, pagenum);
}

void AT45StartContinueRead(unsigned short pageno)
{
	while(!(AT45_PORT_IN & _BV(BUSY_PIN)));

	UNSELECT_AT45;
    SELECT_AT45;

	spiWriteByte(CONTINUE_READ);			//  0x68
	spiWriteByte((char)(pageno >> 6));
    spiWriteByte((char)(pageno << 2));
    spiWriteByte(0x00);						//  с нулевого байта

    spiWriteByte(0x00);
    spiWriteByte(0x00);
    spiWriteByte(0x00);
    spiWriteByte(0x00);

}

void AT45GetPage(unsigned short pageno, unsigned char* buf)
{
	unsigned short k = 0;


	while (k < PAGE_USER_AREA)
	{
		*buf++ = spiReadByte();
		k++;
	}

    UNSELECT_AT45;
}

void AT45WriteThroughBuffer(unsigned short pageno, unsigned char b)
{
	while(!(AT45_PORT_IN & _BV(BUSY_PIN)));

	UNSELECT_AT45;
    SELECT_AT45;

	spiWriteByte(MM_PAGE_PROG_THROUGH_B1);			//  0x68
	spiWriteByte((char)(pageno >> 6));
    spiWriteByte((char)(pageno << 2));
    spiWriteByte(0x04);						//  с нулевого байта

	spiWriteByte(b);

	UNSELECT_AT45;
}

//  изменение произвольного слова в странице
char AT45WriteWordToPage(unsigned short pageno,
					unsigned char wordno, unsigned short w) //  что лучше - short или << 1
{
	AT45ReadPageToBuffer(0, pageno);

	wBufPos[0] = wordno << 1;
	AT45WriteByteToBuffer(0, w & 0xFF);
	AT45WriteByteToBuffer(0, w >> 8);

	AT45WriteBufferToPage(0, pageno);

	return 1;
}

//  изменение произвольного слова в странице
void AT45WriteByteToPage(unsigned short pageno,
					unsigned short byteno, unsigned char b)
{
	AT45ReadPageToBuffer(0, pageno);

	wBufPos[0] = byteno;
	AT45WriteByteToBuffer(0, b);

	AT45WriteBufferToPage(0, pageno);
}

void AT45ReadPage(unsigned short pageno, unsigned char* pBuf)
{
	unsigned short k = 0;

	while(!(AT45_PORT_IN & _BV(BUSY_PIN)));

	UNSELECT_AT45;
    SELECT_AT45;

	spiWriteByte(MAIN_MEMORY_PAGE_READ);		//  0x52
	spiWriteByte((char)(pageno >> 6));
	spiWriteByte((char)(pageno << 2) );
	spiWriteByte(0);

	spiWriteByte(0xFF);
	spiWriteByte(0xFF);
	spiWriteByte(0xFF);
	spiWriteByte(0xFF);

	while (k < PAGE_USER_AREA)
	{
		*pBuf++ = spiReadByte();
		k++;
	}

	UNSELECT_AT45;
}

void AT45WriteBuffer(unsigned char* pBuf)
{
	unsigned short k = 0;

	while(!(AT45_PORT_IN & _BV(BUSY_PIN)));

	UNSELECT_AT45;
	SELECT_AT45;

    spiWriteByte(BUFFER_1_WRITE);	//  0x84

	spiWriteByte(0x00);
    spiWriteByte(0);
    spiWriteByte(0);

	while (k < PAGE_USER_AREA)
	{
		spiWriteByte(*pBuf++);
		k++;
	}

    UNSELECT_AT45;
}
