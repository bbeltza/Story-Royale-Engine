#include <stdlib.h>
#include <stdio.h>
#include "System.h"

void _system_err(unsigned short LINE, const char* FILE, const char* fmt, ...)
{
    printf("\x1b[1;31mError:\n");
    printf("\tLine: %u\n", LINE);
    printf("\tFile: %s\n\t\tMessage: \x1b[2m", FILE);

    va_list args;
    va_start(args, fmt);

    while (*fmt)
    {
        if (*fmt == '%')
        {
            fmt++;
            if (*fmt == 'd' || *fmt == 'i') printf("%d", va_arg(args, signed int));
            else if (*fmt == 'o') printf("%o", va_arg(args, int));
            else if (*fmt == 'x' || *fmt == 'X') printf("%x", va_arg(args, int));
            else if (*fmt == 'u') printf("%u", va_arg(args, unsigned int));
            else if (*fmt == 's') printf("%s", va_arg(args, char*));
            else if (*fmt == 'c') printf("%c", va_arg(args, char));
            else if (*fmt == 'f') printf("%f", va_arg(args, float));
            else if (*fmt == 'g' || *fmt == 'G') printf("%g", va_arg(args, float));
            else if (*fmt == 'e' || *fmt == 'E') printf("%e", va_arg(args, float));
            else if (*fmt == 'p') printf("%p", va_arg(args, void*));
            else if (*fmt == 'n') printf("%n", va_arg(args, int*));
            else if (*fmt == '%') putchar('%');
            else if (*fmt == 'l')
            {
                fmt++;
                if (*fmt == 'f') printf("%lf", va_arg(args, double));
                else if (*fmt == 'd' || *fmt == 'i') printf("%ld", va_arg(args, long));
                else if (*fmt == 'u') printf("%lu", va_arg(args, unsigned long));
                else if (*fmt == 'l')
                {
                    fmt++;
                    if (*fmt == 'd' || *fmt == 'i') printf("%lld", va_arg(args, long long));
                    else if (*fmt == 'u') printf("%llu", va_arg(args, unsigned long long));
                }
            }
            else if (*fmt == 'L')
            {
                fmt++;
                if (*fmt == 'f') printf("%Lf", va_arg(args, long double));
            }
        }
        else
        {
            putchar(*fmt);
        }

        fmt++;
    }
    va_end(args);

    printf("\033[0m\n\7");

#ifdef _DEBUG
    printf("The execution has been paused. ");
    system("pause");
#else
    abort();
#endif // _DEBUG    
    
    printf("\n");
}