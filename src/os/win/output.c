#include "os_win.h"

#include "OS.h"

#define getstd_win(...)									\
HANDLE StdOutput = GetStdHandle(STD_OUTPUT_HANDLE);		 \
if (!StdOutput) return __VA_ARGS__

#define TO_COORDS(x) (*(COORD*)x)

#include "stdio.h"

short* os_win_outputcoordget(short buff[2])
{
	CONSOLE_SCREEN_BUFFER_INFO ConsoleInfo;
	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ConsoleInfo))
	{
		DWORD status = GetLastError();
		return NULL;
	}

	*(long*)buff = *(const long*)&ConsoleInfo.dwCursorPosition;

	return buff;
}

void os_win_outputcoordset(const short coords[2])
{
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), TO_COORDS(coords));
}

int os_win_outputgetc(void)
{
	getstd_win(0);
	
	short coords[2];
	if (!os_win_outputcoordget(coords)) return 0;

	if (coords[0] == 0)
		return '\n';

	--coords[1];

	CHAR Char;
	DWORD CharsRead;
	if (ReadConsoleOutputCharacterA(StdOutput, &Char, sizeof(CHAR), TO_COORDS(coords), &CharsRead))
		return Char;
	else
		return 0;
}

int os_win_outputhasnline(void)
{
	getstd_win(0);
	short coords[2];
	if (!os_win_outputcoordget(coords)) return 1;

	return coords[0] == 0;
}