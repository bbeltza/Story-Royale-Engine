#include "os_win.h"

#include "OS.h"

#define getstd_win										\
HANDLE StdOutput = GetStdHandle(STD_OUTPUT_HANDLE);		 \
if (!StdOutput) return 0

#define getinfo_win						\
CONSOLE_SCREEN_BUFFER_INFO ConsoleInfo;  \
if (!GetConsoleScreenBufferInfo(StdOutput, &ConsoleInfo)) return 0

int os_win_outputgetc(void)
{
	getstd_win;
	getinfo_win;

	if (ConsoleInfo.dwCursorPosition.X == 0)
		return '\n';

	ConsoleInfo.dwCursorPosition.X--;

	CHAR Char;
	DWORD CharsRead;
	if (ReadConsoleOutputCharacterA(StdOutput, &Char, sizeof(CHAR), ConsoleInfo.dwCursorPosition, &CharsRead))
		return Char;
	else
		return 0;
}

int os_win_outputhasnline(void)
{
	getstd_win;
	getinfo_win;

	return ConsoleInfo.dwCursorPosition.X == 0;
}