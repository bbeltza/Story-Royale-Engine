#include "os_win.h"

#include "OS.h"

#define getstd_win(...)									\
HANDLE StdOutput = GetStdHandle(STD_OUTPUT_HANDLE);		 \
if (!StdOutput) return __VA_ARGS__

#define TO_COORDS(x) (*(COORD*)x)

#include "stdio.h"

int* os_win_outputcoordget(int buff[2])
{
	CONSOLE_SCREEN_BUFFER_INFO ConsoleInfo;
	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ConsoleInfo))
	{
		DWORD status = GetLastError();
		return NULL;
	}

	buff[0] = ConsoleInfo.dwCursorPosition.X;
	buff[1] = ConsoleInfo.dwCursorPosition.Y;

	return buff;
}

void os_win_outputcoordset(const int coords[2])
{
	COORD _coords = { (SHORT)coords[0], (SHORT)coords[1] };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), _coords);
}

int os_win_outputgetc(void)
{
	getstd_win(0);
	
	int coords[2];
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
	int coords[2];
	if (!os_win_outputcoordget(coords)) return 1;

	return coords[0] == 0;
}