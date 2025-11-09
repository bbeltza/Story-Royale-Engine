#include "os_unix.h"

#include "termios.h"
#include "stdio.h"

// conio's wherexy linux (maybe POSIX too) implementation, took from "https://github.com/casualsnek/linuxconio/blob/main/conio.h"
extern int wherexy(int *x, int *y);

short* os_unix_outputcoordget(short buff[2])
{
    int ibuff[2];
    wherexy(&ibuff[1], &ibuff[0]);
    buff[0] = ibuff[0] - 1;
    buff[1] = ibuff[1];
}

void os_unix_outputcoordset(const short coords[2])
{
    printf("\033[%d;%dH", coords[0], coords[1]);
}

int os_unix_outputgetc(void)
{
    return -1; // TODO: Implement outputgetc for POSIX
}

int os_unix_outputhasnline(void)
{
    short coords[2];
    os_unix_outputcoordget(coords);
    return coords[0] == 0;
}