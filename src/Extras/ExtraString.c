#include <string.h>
#include <C/extra_string.h>
#include "utils/math.h"

const char* strnrchr(const char* _src, int _Char, size_t _Count)
{
    size_t l = strlen(_src);
    const char* new_ptr = _src+ ut_min(l, _Count);
    while (new_ptr != _src && *new_ptr != _Char)
        new_ptr--;
    
    return new_ptr;
}