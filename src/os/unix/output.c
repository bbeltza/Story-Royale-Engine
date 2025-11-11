#include "os_unix.h"

#include "termios.h"
#include "stdio.h"

// conio's wherexy linux (maybe POSIX too) implementation, took from "https://github.com/casualsnek/linuxconio/blob/main/conio.h"
extern int wherexy(int *x, int *y);

int* os_unix_outputcoordget(int buff[2])
{
    if (!wherexy(&buff[1], &buff[0])) return NULL;
    buff[0]--;
    buff[1]--;

    return buff;
}

void os_unix_outputcoordset(const int coords[2])
{
    printf("\033[%d;%dH", coords[0], coords[1]);
}

int os_unix_outputgetc(void)
{
    return -1; // TODO: Implement outputgetc for POSIX
}

int os_unix_outputhasnline(void)
{
    int coords[2];
    if (os_unix_outputcoordget(coords))
        return coords[0] == 0;
    return 0;
}