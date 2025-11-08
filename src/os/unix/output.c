#include "os_unix.h"

int os_unix_outputgetc(void)
{
    char chr;
    
    
    read(STDOUT_FILENO, &chr, sizeof(char));
    return chr;
}

int os_unix_outputhasnline(void)
{
    return os_unix_outputgetc() == '\n';
}