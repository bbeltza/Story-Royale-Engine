#include "os_unix.h"

#include <stdio.h>
#include <termios.h>

int wherexy(int *x, int *y)
{
    struct termios old_termios;
    if (tcgetattr(STDIN_FILENO, &old_termios)) return 0;

    struct termios next_termios = old_termios;
    next_termios.c_lflag &=~ICANON;
    next_termios.c_lflag &=~ECHO;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &next_termios)) return 0;

    printf("\033[6n");
    scanf("\033[%d;%dR", x, y);

    if (tcsetattr(STDIN_FILENO, TCSANOW, &old_termios)) abort();

    return 1;
}