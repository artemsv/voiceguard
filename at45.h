//  01.11.2006

//  эти две константы в дальнейшем предполагается получать от самой флешки
#define PAGE_SIZE		528			//  размер в байтах одной страницы
#define PAGE_COUNT		4096		//  количество страниц
#define PAGE_USER_AREA	512			//  размер области данных страницы

//  порт, к коему подсоед. флешка
#define AT45_PORT_OUT	PORTB
#define AT45_PORT_IN 	PINE
#define BUSY_PIN		PE2

// DataFlash reset port pin (PB 0)
#define DF_RESET 0x01
         
// DataFlash ready/busy status port pin (PB0)
#define DF_RDY_BUSY 0x00

// DataFlash boot sector write protection (PB 2)
#define DF_WRITE_PROTECT 0x04

// DataFlash chip select port pin (PB 4)
#define DF_CHIP_SELECT 	0x10
#define DF_CHIP_SELECT_	PB4

// buffer 1 
#define BUFFER_1 0x00
// buffer 2
#define BUFFER_2 0x01

// defines for all opcodes
#define BUFFER_1_WRITE 0x84			// buffer 1 write 
#define BUFFER_2_WRITE 0x87			// buffer 2 write 
#define BUFFER_1_READ 0x54			// buffer 1 read
#define BUFFER_2_READ 0x56			// buffer 2 read
// buffer 1 to main memory page program with built-in erase
#define B1_TO_MM_PAGE_PROG_WITH_ERASE 0x83
// buffer 2 to main memory page program with built-in erase
#define B2_TO_MM_PAGE_PROG_WITH_ERASE 0x86
// buffer 1 to main memory page program without built-in erase
#define B1_TO_MM_PAGE_PROG_WITHOUT_ERASE 0x88
// buffer 2 to main memory page program without built-in erase
#define B2_TO_MM_PAGE_PROG_WITHOUT_ERASE 0x89
// main memory page program through buffer 1
#define MM_PAGE_PROG_THROUGH_B1 0x82
// main memory page program through buffer 2
#define MM_PAGE_PROG_THROUGH_B2 0x85
// auto page rewrite through buffer 1
#define AUTO_PAGE_REWRITE_THROUGH_B1 0x58
// auto page rewrite through buffer 2
#define AUTO_PAGE_REWRITE_THROUGH_B2 0x59
// main memory page compare to buffer 1
#define MM_PAGE_TO_B1_COMP 0x60
// main memory page compare to buffer 2
#define MM_PAGE_TO_B2_COMP 0x61
// main memory page to buffer 1 transfer
#define MM_PAGE_TO_B1_XFER 0x53
// main memory page to buffer 2 transfer
#define MM_PAGE_TO_B2_XFER 0x55
// DataFlash status register for reading density, compare status, 
// and ready/busy status
#define STATUS_REGISTER 0x57

// main memory page read
#define MAIN_MEMORY_PAGE_READ 0x52

// erase a 528 byte page
#define PAGE_ERASE 0x81

// erase 512 pages
#define BLOCK_ERASE 0x50

#define CONTINUE_READ	0xE8

/*

	Интерфейс X - низкоуровневый интерфейс к флешке

	используется драйвером файловой системы FATX
*/

//  запись в буфер 1

void AT45Init(void);
void AT45SetReadBufferPos(unsigned char bufno, unsigned short pos);
void AT45SetWriteBufferPos(unsigned char bufno, unsigned short pos);
void AT45erase(void);
char AT45ReadStatus(void);
char AT45WriteByteToBuffer(unsigned char bufno, unsigned char byte);
char AT45WriteBufferToPage(unsigned char bufno, unsigned short pagenum);
char AT45ReadPageToBuffer(unsigned char bufno, unsigned short pagenum);

//void AT45write_to_flash(unsigned char flash_data);

unsigned char AT45ReadByteFromBuffer(unsigned char bufno);
unsigned char AT45ReadByteFromPage(unsigned short pageno, 
					unsigned short byteno);
void AT45PageErase(unsigned short pagenum);
void AT45FillBuffer(unsigned char bufno, unsigned char b);
void AT45ZeroPage(unsigned short pagenum);

void AT45StartContinueRead(unsigned short pageno);

//  возвращает в буфере buf прочитанную страницу номер pageno
//  используется CONTINUOUS ARRAY READ
void AT45GetPage(unsigned short pageno, unsigned char* buf);

/*
	X1	-	читать слово с заданной страницы
	X2	-	читать байты в цикле и сохранят их в буфер
	X3	-	изменить слово на заданной странице
	X4	-	писать байты из буфера в цикле в заданную страницу
	X5	-	стереть страницу 
	X6	-	проиграть страницу
*/
/*
//  чтение произвольного слова с любой страницы				X1
unsigned short AT45ReadWordFromPage(unsigned short pageno, 
					unsigned char byteno);  //  что лучше - short или << 1

*/

void AT45WriteThroughBuffer(unsigned short pageno, unsigned char b);

//  изменение произвольного слова в странице
char AT45WriteWordToPage(unsigned short pageno, 
					unsigned char wordno, unsigned short w);  //  что лучше - short или << 1
//  изменение произвольного байта в странице
void AT45WriteByteToPage(unsigned short pageno, 
					unsigned short byteno, unsigned char b);

//  читает страницу номер pageno в буфер по адресу buf				  
void AT45ReadPage(unsigned short pageno, unsigned char* pBuf);
void AT45WriteBuffer(unsigned char* pBuf);

