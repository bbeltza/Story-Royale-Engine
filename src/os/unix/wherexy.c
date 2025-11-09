#include "os_unix.h"

#include <stdio.h>
#include <termios.h>

int wherexy(int *x, int *y)
{
    struct termios old_termios;
    tcgetattr(STDIN_FILENO, &old_termios);

    struct termios next_termios = old_termios;
    next_termios.c_lflag &=~ICANON;
    next_termios.c_lflag &=~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &next_termios);

    printf("\033[6n");
    scanf("\033[%d;%dR", x, y);

    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
}