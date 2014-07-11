//  30.11.2006


// 4096 страниц по 528 байт = 2 162 688 байт
// 1 блок = 8 страниц
// 512 блоков по 4224 байт  = 2 162 688 байт

//  FATX

/*
	первые PT_SIZE страниц - файл распределения страниц, PAGE TABLE (PT)
	за ними располагается битовая карта страниц, каждый бит которой 
	соответствует странице, 0 - пустая страница, 1 - занятая. Битовая карта
	занимает одну страницу.

	все файлы нумеруются - от 1 до PAGE_COUNT - PT_SIZE - 1

	Первые PAGE_SIZE - PAGE_USER_AREA байт каждой страницы отводятся под 
	служебные сведения. 
*/

#define PT_SIZE			16			//  размер PT  в страницах
#define FAT_SIZE		16			//  размер FAT в страницах

#define BITMAP_PAGE		PT_SIZE		//  страница с битовой картой страниц ;) - 

#define FILES_COUNT		PAGE_COUNT - PAGE_SIZE - 1	//  макс. кол-во файлов
#define FILE_ENTRY_IN_PAGE	(PAGE_USER_AREA / sizeof(short))
#define LAST_PAGE_MARKER	0xFFFF
#define EMPTY_PAGE_MARKER	0x0000

//  поля служебной области страницы

//  смещение размера файла
#define FILE_SIZE		2			
//  смещение следующей страницы этого же файла
#define FILE_NEXT		FILE_SIZE + sizeof(short)	

/*
	
	Интерфейс С - высокоуровневый интерфейс к файловой системе

	используется ядром системы VG

*/

typedef struct tagFILE_ENTRY
{
	unsigned short pageno;
	unsigned short size;

} FILE_ENTRY, *PFILE_ENTRY;

char fatxInit(void);

//  index - номер страницы или номер слова, которое соответствует своей странице
unsigned short fatxGetFAT(unsigned short index);

void fatxSetFAT(unsigned short index, unsigned short value);

unsigned short fatxGetEmptyPage();

//  создает новый файл													C2
//  ищет в RootDir первый свободный(нулевой) индекс - номер нового файла
//  ищет в FAT первую свободную страницу - первая страница нового файла
unsigned char fatxCreateFile();

//  удаляет файл fileno
void fatxDeleteFile(unsigned char fileno);

//  выделяет файлу fileno свободную страницу о возвращает ее номер
//  если свободных страниц нет - ноль 
unsigned short fatxAddPageToFile(unsigned char fileno, unsigned short count);

//  возвращает число существующих файлов
char fatxGetFileCount(void);
unsigned short fatxFileExists(unsigned char fileno);
void fatxFormat(void);
unsigned short fatxGetNextPage(unsigned short prevpageno);
char fatxOpenFile(unsigned char fileno, unsigned char mode);
//  закрывает текущий открытый файл
void fatxCloseFile(void);
char fatxReadPageFromFile(unsigned char* pPage);
char fatxWritePageToFile(unsigned char* pPage);
char fatxReadByteFromFile(unsigned char* resbyte);
char fatxFileSeek(unsigned short pos);
char fatxWriteByteToFile(unsigned char byte);
char fatxFlushFile(void);


/*
char DeleteFile(unsigned short fileno);								//  C3

// записывает байт byte в файл номер fileno
char WriteFile(unsigned short fileno, unsigned char byte);			//  C4

//  проиграть файл номер fileno
char PlayFile(unsigned short fileno);								//	C5

//  пока нажата Enter, записывать оцифрованный звук
char RecordFile(unsigned short fileno);								//	C6
*/
