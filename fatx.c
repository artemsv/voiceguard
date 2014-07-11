//  30.11.2006
#include "stdafx.h"
#include "fatx.h"
#include "at45.h"

#define OPEN_MODE_READ		0
#define OPEN_MODE_WRITE		1

//unsigned short page[FILE_ENTRY_IN_PAGE];
unsigned short rootdir[FILE_ENTRY_IN_PAGE];

//  при создании файла ему сразу выделяется одна страница - спорный момент

//  существует два способа чтения и записи файла - постраничное и побайтовое,
//  которые нельзя смешивать
//	- постраничное чтение использует поля curpageno
//	- побайтовое чтение использует pointer, pointeroffset, pointerpageno, cachpageno
//		page
//	- методы fatxFileSeek и fatxFlushFile можно использовать только 
//		при побайтовом способе
//
//  чтобы перейти к другому типу чтения или записи, надо переоткрыть файл???

/*
	В странице номер FAT_SIZE находится RootDir, который содержит 256 FileEntry
*/

struct 
{
	unsigned short	mode;
	unsigned short 	fileno;		
	unsigned short 	startpageno;
	unsigned short	size;			
	unsigned short 	pointer;		//  смещение указателя в байтах от начала файла
	unsigned short 	curpageno;		//  следующая страница для чтения, 0 - больше нет 
	unsigned short	cachepageno;	//  номер страницы, наход. в кэше

	unsigned short 	pointerpageno;	//  номер страницы файла, содержащей указатель
	unsigned short	pointeroffset;	//  смещение в последней странице файла
	unsigned char 	page[PAGE_USER_AREA];

	// pointer = pointerpage*512 + pointeroffset;

} cur_file;

typedef unsigned short* pword;

//  первые 80 байт в FAT равны 0xFF - это значит, что первые 40 страниц
//  флешки зарезервированы 16(с нулевой по пятнадцатую) из них - FAT,
//  шестнадцатая - RootDir, остальные 23 - резерв

char fatxInit()
{
	AT45GetPage(FAT_SIZE, (unsigned char*)&rootdir);
	cur_file.fileno = 0xFFFF;							//  нет открытого файла

/*	short k;
	for (k = 0; k < 40; k++)
		rootdir[k] = k;
*/
	return 1;
}

//  index - номер страницы или номер слова, которое соответствует своей странице
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
			{	//  нашли свободную страницу

				((pword)(&cur_file.page))[i] = LAST_PAGE_MARKER;//  последнияя страница файла
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

	//  цикл по FileEntry в RootDir'e
	for (k = 0; k < FILE_ENTRY_IN_PAGE; k = k + 2)
	{
		if (!rootdir[k])
		{	//  пустое место для нового файла

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

	//  получаем первую страницу, выделенную удаляемому файлу
	unsigned short pageno = (*(PFILE_ENTRY)(&rootdir[marker])).pageno;

	if (!pageno)
		return;					//  файла не существует

	//  обнуление FILE_ENTRY в RootDir
	rootdir[marker] = 0;
	rootdir[marker + 1] = 0;
	AT45WriteWordToPage(FAT_SIZE, marker, 0);
	AT45WriteWordToPage(FAT_SIZE, marker + 1, 0);

	//  обнуление цепочки страниц в FATe

	marker = pageno;
	//  пройдемся по цепочке и отберем все страницы у файла
	while (marker != LAST_PAGE_MARKER)
	{
		pageno = marker;
		marker = fatxGetFAT(pageno);

		fatxSetFAT(pageno, EMPTY_PAGE_MARKER);
	};
}

//  выделить файлу count страниц
unsigned short fatxAddPageToFile(unsigned char fileno, unsigned short count)
{
	//  получаем первую страницу файла
	unsigned short pageno = (*(PFILE_ENTRY)(&rootdir[fileno << 1])).pageno;

	if (!pageno)
		return 0xFFFF;			//  файл не найден

	//  находим последнюю страницу файла, на которой маркер конца цепочки	

	unsigned short newpage, marker;

	marker = pageno;
	while (marker != LAST_PAGE_MARKER)
	{
		pageno = marker;
		marker = fatxGetFAT(pageno);
	}

	//  по выходу из цикла pageno - индекс в FATe последней страницы файла,
	//  содержащей маркер LAST_PAGE_MARKER

	while (count)
	{
		newpage = fatxGetEmptyPage();

		if (!newpage)
			return 0xFFFF;		//  все страницы разобраны...

		//  делаем предыдущюю страницу непоследней
		fatxSetFAT(pageno, newpage);
		pageno = newpage;
		count--;
	}

	//  делаем свежевыделенную страницу последней
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

	//  конечно, такого быть не должно - у файла не может быть страницы номер ноль - 
	//  либо число от 0x28 до 0x0FFF либо 0xFFFF (это последняя страница)
	if (!res)
		res = 0xFFFF;

	return res;
}

char fatxOpenFile(unsigned char fileno, unsigned char mode)
{
	if ((!fatxFileExists(fileno)) ||
								//  такого файла нет или

	 (0xFFFF != cur_file.fileno))
		return 0;			//  имеется другой открытый файл и его надо закрыть

	cur_file.mode = mode;
	cur_file.fileno = fileno;
	cur_file.startpageno = (*(PFILE_ENTRY)(&rootdir[fileno << 1])).pageno;
	cur_file.curpageno = cur_file.startpageno;
	cur_file.size = (*(PFILE_ENTRY)(&rootdir[fileno << 1])).size;

	//  настройка механизма побайтового доступа

	//  в кэше файла ничего нет
	cur_file.cachepageno = 0xFFFF;
	cur_file.pointer = 0;
	cur_file.pointerpageno = cur_file.startpageno;
	cur_file.pointeroffset = 0;

	return 1;
}

//  закрывает текущий открытый файл
void fatxCloseFile(void)
{
	cur_file.fileno = 0xFFFF;
}

//  читаем текущую страницу текущего файла
char fatxReadPageFromFile(unsigned char* pPage)
{
	if ((0xFFFF == cur_file.fileno) ||
													//  нет открытого файла

	(LAST_PAGE_MARKER == cur_file.curpageno))
		return 0;									//  добрались до конца

	AT45ReadPage(cur_file.curpageno, pPage);

	//  текущая страница файла при следующем чтении - или 0xFFFF, если файлу конец
	cur_file.curpageno = fatxGetNextPage(cur_file.curpageno);

	return 1;
}

char fatxWritePageToFile(unsigned char* pPage)
{
	if ((0xFFFF == cur_file.fileno) ||
											//  нет открытого файла

	(LAST_PAGE_MARKER == cur_file.curpageno))
		return 0;									//  добрались до конца

	//  записываем переданный буфер в буфер АТ45
	AT45WriteBuffer(pPage);
	//  пишем из буфера АТ45 во флеш
	AT45WriteBufferToPage(0, cur_file.curpageno);

	//  выделяем файлу новую страницу - при след. вызове писать будем в нее
	cur_file.curpageno = fatxAddPageToFile(cur_file.fileno, 1);

	return 1;
}

char fatxReadByteFromFile(unsigned char* resbyte)
{
	if ((0xFFFF == cur_file.fileno) ||
										//  нет открытого файла

	(cur_file.pointer == cur_file.size) ||
										//  чтение за концом файла

	(OPEN_MODE_READ != cur_file.mode))			//  файл не открыт для чтения
		return 0;

	if (0xFFFF == cur_file.cachepageno)
	{	//  так как в кэше ничего нет, то это первое чтение этого файла
		AT45ReadPage(cur_file.pointerpageno, (unsigned char*)&cur_file.page);
		cur_file.cachepageno = cur_file.pointerpageno;
	
		//  текущая страница файла при следующем чтении - или 0xFFFF, если файлу конец
		cur_file.curpageno = fatxGetNextPage(cur_file.curpageno);
	};

	//  читаем наконец байтик
	*resbyte = cur_file.page[cur_file.pointeroffset];
	
	//  увеличиваем указатель для следующего чтения
	cur_file.pointer++;								//  указатель в файле
	cur_file.pointeroffset++;
	
	if (cur_file.pointeroffset == PAGE_USER_AREA)
	{	//  если вышли за границы страницы

		cur_file.pointeroffset = 0;
		//  следующая страница для чтения - эта
		cur_file.pointerpageno = fatxGetNextPage(cur_file.pointerpageno);
		//  говорим, что кеш устарел и при следующем чтении
		//  в кеш будет загружена новая страница
		cur_file.cachepageno = 0xFFFF;
	}

	return 1;
}

char fatxFileSeek(unsigned short pos)
{
	unsigned short newpageno;

	if ((0xFFFF == cur_file.fileno) ||
		        									//  нет открытого файла

	(pos > cur_file.size))						//  выход за границы файла
		return 0;					//  Seek не может увеличить размер файла

	cur_file.pointer = pos;
	newpageno = pos / PAGE_USER_AREA;

	if ((OPEN_MODE_WRITE == cur_file.mode) && (newpageno != cur_file.pointerpageno))
	{	//  произошла смена страницы в режиме записи - сохранить возможные изменения
		fatxFlushFile();
	}

	cur_file.pointerpageno = newpageno;
	cur_file.pointeroffset = pos % PAGE_USER_AREA;

	//  кеш устарел(конечно, не всегда, но нет времени для всех способов)
	cur_file.cachepageno = 0xFFFF;

	return 1;
}

//  при входе cur_file.pointeroffset ВСЕГДА указывает на новое место для байта,
//  а cur_file.pointerpageno - на правильную страницу, в которую можно писать
//  если cur_file.pointerpageno = 0хFFFF - конец флешки
char fatxWriteByteToFile(unsigned char byte)
{
	if ((0xFFFF == cur_file.fileno) ||
												//  нет открытого файла

		(0xFFFF == cur_file.pointerpageno) ||
					//  файлу не удалось выделить страницу и писать нельзя

		(OPEN_MODE_WRITE != cur_file.mode))

		return 0;								//  файл не был открыт для записи

	if (0xFFFF == cur_file.cachepageno)
	{	//  первая запись в эту страницу - загрузим в кеш копию
		AT45ReadPage(cur_file.pointerpageno, (unsigned char*)&cur_file.page);
		cur_file.cachepageno = cur_file.pointerpageno;
	}

	//  если мы здесь - в файл писать можно, и место есть
	cur_file.page[cur_file.pointeroffset] = byte;
	cur_file.pointeroffset++;
	cur_file.pointer++;

	if (cur_file.pointeroffset > cur_file.size)
		cur_file.size++;							//  добавление в конец файла

	if (PAGE_USER_AREA == cur_file.pointeroffset)
	{	//  дошли до конца текущей страницы
		
		//  сохраняем последние изменения
		fatxFlushFile();

		cur_file.pointerpageno = fatxGetEmptyPage();
		if (!cur_file.pointerpageno)
		{	//  не удалось выделить файлу свободную страницу - кончились наверное...
			cur_file.pointerpageno = 0xFFFF;
		}	
	}

	return 1;
}

//  сохраняет последние изменения в файле - последнюю измененную страницу и 
//  новую длину
char fatxFlushFile(void)
{
	if ((0xFFFF == cur_file.fileno) ||
													//  нет открытого файла

	(OPEN_MODE_WRITE != cur_file.mode))
		return 0;									//  файл не открыт для записи

	//  обновляем копию каталога в памяти
	(*(PFILE_ENTRY)(&rootdir[cur_file.fileno << 1])).size = cur_file.size;

	//  обновляем FileEntry в RootDir'e - изменилось поле size
	AT45WriteWordToPage(FAT_SIZE, (cur_file.fileno << 1) + 1, cur_file.size);

	if (0xFFFF != cur_file.cachepageno)
	{	//  если была кешированная страница

		//  записываем буфер кеша в буфер АТ45
		AT45WriteBuffer((unsigned char*)&cur_file.page);
		//  пишем из буфера АТ45 во флеш
		AT45WriteBufferToPage(0, cur_file.cachepageno);
	}

	return 1;
}
