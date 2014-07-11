//  30.11.2006
#include "stdafx.h"
#include "fatx.h"
#include "at45.h"

#define OPEN_MODE_READ		0
#define OPEN_MODE_WRITE		1

//unsigned short page[FILE_ENTRY_IN_PAGE];
unsigned short rootdir[FILE_ENTRY_IN_PAGE];

//  ��� �������� ����� ��� ����� ���������� ���� �������� - ������� ������

//  ���������� ��� ������� ������ � ������ ����� - ������������ � ����������,
//  ������� ������ ���������
//	- ������������ ������ ���������� ���� curpageno
//	- ���������� ������ ���������� pointer, pointeroffset, pointerpageno, cachpageno
//		page
//	- ������ fatxFileSeek � fatxFlushFile ����� ������������ ������ 
//		��� ���������� �������
//
//  ����� ������� � ������� ���� ������ ��� ������, ���� ����������� ����???

/*
	� �������� ����� FAT_SIZE ��������� RootDir, ������� �������� 256 FileEntry
*/

struct 
{
	unsigned short	mode;
	unsigned short 	fileno;		
	unsigned short 	startpageno;
	unsigned short	size;			
	unsigned short 	pointer;		//  �������� ��������� � ������ �� ������ �����
	unsigned short 	curpageno;		//  ��������� �������� ��� ������, 0 - ������ ��� 
	unsigned short	cachepageno;	//  ����� ��������, �����. � ����

	unsigned short 	pointerpageno;	//  ����� �������� �����, ���������� ���������
	unsigned short	pointeroffset;	//  �������� � ��������� �������� �����
	unsigned char 	page[PAGE_USER_AREA];

	// pointer = pointerpage*512 + pointeroffset;

} cur_file;

typedef unsigned short* pword;

//  ������ 80 ���� � FAT ����� 0xFF - ��� ������, ��� ������ 40 �������
//  ������ ��������������� 16(� ������� �� �����������) �� ��� - FAT,
//  ������������ - RootDir, ��������� 23 - ������

char fatxInit()
{
	AT45GetPage(FAT_SIZE, (unsigned char*)&rootdir);
	cur_file.fileno = 0xFFFF;							//  ��� ��������� �����

/*	short k;
	for (k = 0; k < 40; k++)
		rootdir[k] = k;
*/
	return 1;
}

//  index - ����� �������� ��� ����� �����, ������� ������������� ����� ��������
unsigned short fatxGetFAT(unsigned short index)
{
	unsigned short pageno, byteno, res;

	pageno = index / FILE_ENTRY_IN_PAGE;
	byteno = (index % FILE_ENTRY_IN_PAGE) << 1;

	res = AT45ReadByteFromPage(pageno, byteno) + 
			(AT45ReadByteFromPage(pageno, byteno + 1) << 8);

	return res;
}

void fatxSetFAT(unsigned short index, unsigned short value)
{
	unsigned short pageno = index / FILE_ENTRY_IN_PAGE;
	unsigned short wordno = index % FILE_ENTRY_IN_PAGE;

	AT45WriteWordToPage(pageno, wordno, value);
}

unsigned short fatxGetEmptyPage()
{
	unsigned short i, k;

	for (k = 0; k < FAT_SIZE; k++)
	{
		AT45GetPage(k, (unsigned char*)&cur_file.page);
		for (i = 0; i < FILE_ENTRY_IN_PAGE; i++)
		{
			if (  !(pword)(&cur_file.page)[i] )
			{	//  ����� ��������� ��������

				((pword)(&cur_file.page))[i] = LAST_PAGE_MARKER;//  ���������� �������� �����
				// TODO Cash last page need 

				AT45WriteWordToPage(k, i, LAST_PAGE_MARKER);
			
				return k * FILE_ENTRY_IN_PAGE + i;
			}
		}
	}

	return 0;
}

unsigned char fatxCreateFile()
{
	unsigned short k, pageno;

	//  ���� �� FileEntry � RootDir'e
	for (k = 0; k < FILE_ENTRY_IN_PAGE; k = k + 2)
	{
		if (!rootdir[k])
		{	//  ������ ����� ��� ������ �����

			pageno = fatxGetEmptyPage();
			(*(PFILE_ENTRY)(&rootdir[k])).pageno = pageno;
			(*(PFILE_ENTRY)(&rootdir[k])).size = 0;

			AT45WriteWordToPage(FAT_SIZE, k, pageno);

			return pageno ? k : 0;
		}
	}

	return 0xFF;
}

void fatxDeleteFile(unsigned char fileno)
{
	unsigned short marker = fileno << 1;

	//  �������� ������ ��������, ���������� ���������� �����
	unsigned short pageno = (*(PFILE_ENTRY)(&rootdir[marker])).pageno;

	if (!pageno)
		return;					//  ����� �� ����������

	//  ��������� FILE_ENTRY � RootDir
	rootdir[marker] = 0;
	rootdir[marker + 1] = 0;
	AT45WriteWordToPage(FAT_SIZE, marker, 0);
	AT45WriteWordToPage(FAT_SIZE, marker + 1, 0);

	//  ��������� ������� ������� � FATe

	marker = pageno;
	//  ��������� �� ������� � ������� ��� �������� � �����
	while (marker != LAST_PAGE_MARKER)
	{
		pageno = marker;
		marker = fatxGetFAT(pageno);

		fatxSetFAT(pageno, EMPTY_PAGE_MARKER);
	};
}

//  �������� ����� count �������
unsigned short fatxAddPageToFile(unsigned char fileno, unsigned short count)
{
	//  �������� ������ �������� �����
	unsigned short pageno = (*(PFILE_ENTRY)(&rootdir[fileno << 1])).pageno;

	if (!pageno)
		return 0xFFFF;			//  ���� �� ������

	//  ������� ��������� �������� �����, �� ������� ������ ����� �������	

	unsigned short newpage, marker;

	marker = pageno;
	while (marker != LAST_PAGE_MARKER)
	{
		pageno = marker;
		marker = fatxGetFAT(pageno);
	}

	//  �� ������ �� ����� pageno - ������ � FATe ��������� �������� �����,
	//  ���������� ������ LAST_PAGE_MARKER

	while (count)
	{
		newpage = fatxGetEmptyPage();

		if (!newpage)
			return 0xFFFF;		//  ��� �������� ���������...

		//  ������ ���������� �������� �����������
		fatxSetFAT(pageno, newpage);
		pageno = newpage;
		count--;
	}

	//  ������ ��������������� �������� ���������
	fatxSetFAT(pageno, LAST_PAGE_MARKER);

	return count;
}

char fatxGetFileCount()
{
	unsigned short k;
	unsigned char count = 0;

	for (k = 0; k < FILE_ENTRY_IN_PAGE; k = k + 2)
	{
		if (rootdir[k])
			count++;
	}

	return count;
}

unsigned short fatxFileExists(unsigned char fileno)
{
	return (*(PFILE_ENTRY)(&rootdir[fileno << 1])).pageno;
}

void fatxFormat(void)
{
	unsigned char k;

	for (k = 0; k < 255; k++)
		rootdir[k] = 0;
	AT45ZeroPage(16);
	AT45FillBuffer(0, 0);	
	AT45SetWriteBufferPos(0, 0);
	for (k = 0; k < 80; k++)
		AT45WriteByteToBuffer(0, 0xFF);
	AT45WriteBufferToPage(0, 0);
}

unsigned short fatxGetNextPage(unsigned short prevpageno)
{
	unsigned short res = fatxGetFAT(prevpageno);	

	//  �������, ������ ���� �� ������ - � ����� �� ����� ���� �������� ����� ���� - 
	//  ���� ����� �� 0x28 �� 0x0FFF ���� 0xFFFF (��� ��������� ��������)
	if (!res)
		res = 0xFFFF;

	return res;
}

char fatxOpenFile(unsigned char fileno, unsigned char mode)
{
	if ((!fatxFileExists(fileno)) ||
								//  ������ ����� ��� ���

	 (0xFFFF != cur_file.fileno))
		return 0;			//  ������� ������ �������� ���� � ��� ���� �������

	cur_file.mode = mode;
	cur_file.fileno = fileno;
	cur_file.startpageno = (*(PFILE_ENTRY)(&rootdir[fileno << 1])).pageno;
	cur_file.curpageno = cur_file.startpageno;
	cur_file.size = (*(PFILE_ENTRY)(&rootdir[fileno << 1])).size;

	//  ��������� ��������� ����������� �������

	//  � ���� ����� ������ ���
	cur_file.cachepageno = 0xFFFF;
	cur_file.pointer = 0;
	cur_file.pointerpageno = cur_file.startpageno;
	cur_file.pointeroffset = 0;

	return 1;
}

//  ��������� ������� �������� ����
void fatxCloseFile(void)
{
	cur_file.fileno = 0xFFFF;
}

//  ������ ������� �������� �������� �����
char fatxReadPageFromFile(unsigned char* pPage)
{
	if ((0xFFFF == cur_file.fileno) ||
													//  ��� ��������� �����

	(LAST_PAGE_MARKER == cur_file.curpageno))
		return 0;									//  ��������� �� �����

	AT45ReadPage(cur_file.curpageno, pPage);

	//  ������� �������� ����� ��� ��������� ������ - ��� 0xFFFF, ���� ����� �����
	cur_file.curpageno = fatxGetNextPage(cur_file.curpageno);

	return 1;
}

char fatxWritePageToFile(unsigned char* pPage)
{
	if ((0xFFFF == cur_file.fileno) ||
											//  ��� ��������� �����

	(LAST_PAGE_MARKER == cur_file.curpageno))
		return 0;									//  ��������� �� �����

	//  ���������� ���������� ����� � ����� ��45
	AT45WriteBuffer(pPage);
	//  ����� �� ������ ��45 �� ����
	AT45WriteBufferToPage(0, cur_file.curpageno);

	//  �������� ����� ����� �������� - ��� ����. ������ ������ ����� � ���
	cur_file.curpageno = fatxAddPageToFile(cur_file.fileno, 1);

	return 1;
}

char fatxReadByteFromFile(unsigned char* resbyte)
{
	if ((0xFFFF == cur_file.fileno) ||
										//  ��� ��������� �����

	(cur_file.pointer == cur_file.size) ||
										//  ������ �� ������ �����

	(OPEN_MODE_READ != cur_file.mode))			//  ���� �� ������ ��� ������
		return 0;

	if (0xFFFF == cur_file.cachepageno)
	{	//  ��� ��� � ���� ������ ���, �� ��� ������ ������ ����� �����
		AT45ReadPage(cur_file.pointerpageno, (unsigned char*)&cur_file.page);
		cur_file.cachepageno = cur_file.pointerpageno;
	
		//  ������� �������� ����� ��� ��������� ������ - ��� 0xFFFF, ���� ����� �����
		cur_file.curpageno = fatxGetNextPage(cur_file.curpageno);
	};

	//  ������ ������� ������
	*resbyte = cur_file.page[cur_file.pointeroffset];
	
	//  ����������� ��������� ��� ���������� ������
	cur_file.pointer++;								//  ��������� � �����
	cur_file.pointeroffset++;
	
	if (cur_file.pointeroffset == PAGE_USER_AREA)
	{	//  ���� ����� �� ������� ��������

		cur_file.pointeroffset = 0;
		//  ��������� �������� ��� ������ - ���
		cur_file.pointerpageno = fatxGetNextPage(cur_file.pointerpageno);
		//  �������, ��� ��� ������� � ��� ��������� ������
		//  � ��� ����� ��������� ����� ��������
		cur_file.cachepageno = 0xFFFF;
	}

	return 1;
}

char fatxFileSeek(unsigned short pos)
{
	unsigned short newpageno;

	if ((0xFFFF == cur_file.fileno) ||
		        									//  ��� ��������� �����

	(pos > cur_file.size))						//  ����� �� ������� �����
		return 0;					//  Seek �� ����� ��������� ������ �����

	cur_file.pointer = pos;
	newpageno = pos / PAGE_USER_AREA;

	if ((OPEN_MODE_WRITE == cur_file.mode) && (newpageno != cur_file.pointerpageno))
	{	//  ��������� ����� �������� � ������ ������ - ��������� ��������� ���������
		fatxFlushFile();
	}

	cur_file.pointerpageno = newpageno;
	cur_file.pointeroffset = pos % PAGE_USER_AREA;

	//  ��� �������(�������, �� ������, �� ��� ������� ��� ���� ��������)
	cur_file.cachepageno = 0xFFFF;

	return 1;
}

//  ��� ����� cur_file.pointeroffset ������ ��������� �� ����� ����� ��� �����,
//  � cur_file.pointerpageno - �� ���������� ��������, � ������� ����� ������
//  ���� cur_file.pointerpageno = 0�FFFF - ����� ������
char fatxWriteByteToFile(unsigned char byte)
{
	if ((0xFFFF == cur_file.fileno) ||
												//  ��� ��������� �����

		(0xFFFF == cur_file.pointerpageno) ||
					//  ����� �� ������� �������� �������� � ������ ������

		(OPEN_MODE_WRITE != cur_file.mode))

		return 0;								//  ���� �� ��� ������ ��� ������

	if (0xFFFF == cur_file.cachepageno)
	{	//  ������ ������ � ��� �������� - �������� � ��� �����
		AT45ReadPage(cur_file.pointerpageno, (unsigned char*)&cur_file.page);
		cur_file.cachepageno = cur_file.pointerpageno;
	}

	//  ���� �� ����� - � ���� ������ �����, � ����� ����
	cur_file.page[cur_file.pointeroffset] = byte;
	cur_file.pointeroffset++;
	cur_file.pointer++;

	if (cur_file.pointeroffset > cur_file.size)
		cur_file.size++;							//  ���������� � ����� �����

	if (PAGE_USER_AREA == cur_file.pointeroffset)
	{	//  ����� �� ����� ������� ��������
		
		//  ��������� ��������� ���������
		fatxFlushFile();

		cur_file.pointerpageno = fatxGetEmptyPage();
		if (!cur_file.pointerpageno)
		{	//  �� ������� �������� ����� ��������� �������� - ��������� ��������...
			cur_file.pointerpageno = 0xFFFF;
		}	
	}

	return 1;
}

//  ��������� ��������� ��������� � ����� - ��������� ���������� �������� � 
//  ����� �����
char fatxFlushFile(void)
{
	if ((0xFFFF == cur_file.fileno) ||
													//  ��� ��������� �����

	(OPEN_MODE_WRITE != cur_file.mode))
		return 0;									//  ���� �� ������ ��� ������

	//  ��������� ����� �������� � ������
	(*(PFILE_ENTRY)(&rootdir[cur_file.fileno << 1])).size = cur_file.size;

	//  ��������� FileEntry � RootDir'e - ���������� ���� size
	AT45WriteWordToPage(FAT_SIZE, (cur_file.fileno << 1) + 1, cur_file.size);

	if (0xFFFF != cur_file.cachepageno)
	{	//  ���� ���� ������������ ��������

		//  ���������� ����� ���� � ����� ��45
		AT45WriteBuffer((unsigned char*)&cur_file.page);
		//  ����� �� ������ ��45 �� ����
		AT45WriteBufferToPage(0, cur_file.cachepageno);
	}

	return 1;
}
