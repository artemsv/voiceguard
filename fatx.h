//  30.11.2006


// 4096 ������� �� 528 ���� = 2 162 688 ����
// 1 ���� = 8 �������
// 512 ������ �� 4224 ����  = 2 162 688 ����

//  FATX

/*
	������ PT_SIZE ������� - ���� ������������� �������, PAGE TABLE (PT)
	�� ���� ������������� ������� ����� �������, ������ ��� ������� 
	������������� ��������, 0 - ������ ��������, 1 - �������. ������� �����
	�������� ���� ��������.

	��� ����� ���������� - �� 1 �� PAGE_COUNT - PT_SIZE - 1

	������ PAGE_SIZE - PAGE_USER_AREA ���� ������ �������� ��������� ��� 
	��������� ��������. 
*/

#define PT_SIZE			16			//  ������ PT  � ���������
#define FAT_SIZE		16			//  ������ FAT � ���������

#define BITMAP_PAGE		PT_SIZE		//  �������� � ������� ������ ������� ;) - 

#define FILES_COUNT		PAGE_COUNT - PAGE_SIZE - 1	//  ����. ���-�� ������
#define FILE_ENTRY_IN_PAGE	(PAGE_USER_AREA / sizeof(short))
#define LAST_PAGE_MARKER	0xFFFF
#define EMPTY_PAGE_MARKER	0x0000

//  ���� ��������� ������� ��������

//  �������� ������� �����
#define FILE_SIZE		2			
//  �������� ��������� �������� ����� �� �����
#define FILE_NEXT		FILE_SIZE + sizeof(short)	

/*
	
	��������� � - ��������������� ��������� � �������� �������

	������������ ����� ������� VG

*/

typedef struct tagFILE_ENTRY
{
	unsigned short pageno;
	unsigned short size;

} FILE_ENTRY, *PFILE_ENTRY;

char fatxInit(void);

//  index - ����� �������� ��� ����� �����, ������� ������������� ����� ��������
unsigned short fatxGetFAT(unsigned short index);

void fatxSetFAT(unsigned short index, unsigned short value);

unsigned short fatxGetEmptyPage();

//  ������� ����� ����													C2
//  ���� � RootDir ������ ���������(�������) ������ - ����� ������ �����
//  ���� � FAT ������ ��������� �������� - ������ �������� ������ �����
unsigned char fatxCreateFile();

//  ������� ���� fileno
void fatxDeleteFile(unsigned char fileno);

//  �������� ����� fileno ��������� �������� � ���������� �� �����
//  ���� ��������� ������� ��� - ���� 
unsigned short fatxAddPageToFile(unsigned char fileno, unsigned short count);

//  ���������� ����� ������������ ������
char fatxGetFileCount(void);
unsigned short fatxFileExists(unsigned char fileno);
void fatxFormat(void);
unsigned short fatxGetNextPage(unsigned short prevpageno);
char fatxOpenFile(unsigned char fileno, unsigned char mode);
//  ��������� ������� �������� ����
void fatxCloseFile(void);
char fatxReadPageFromFile(unsigned char* pPage);
char fatxWritePageToFile(unsigned char* pPage);
char fatxReadByteFromFile(unsigned char* resbyte);
char fatxFileSeek(unsigned short pos);
char fatxWriteByteToFile(unsigned char byte);
char fatxFlushFile(void);


/*
char DeleteFile(unsigned short fileno);								//  C3

// ���������� ���� byte � ���� ����� fileno
char WriteFile(unsigned short fileno, unsigned char byte);			//  C4

//  ��������� ���� ����� fileno
char PlayFile(unsigned short fileno);								//	C5

//  ���� ������ Enter, ���������� ������������ ����
char RecordFile(unsigned short fileno);								//	C6
*/
