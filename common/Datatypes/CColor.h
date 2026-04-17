#ifndef SRE_COLOR_H
#define SRE_COLOR_H

// Might join this header with Color.hpp (so it'll be Color.h)

#include <stdint.h>
typedef uint8_t sre_colT;

typedef struct sre_col3
{
    sre_colT r;
    sre_colT g;
    sre_colT b;
} sre_col3;

typedef union sre_col4
{
    sre_col3 col3;     
    struct {
        sre_colT r;        
        sre_colT g;        
        sre_colT b;
        sre_colT a;
    };
} sre_col4;

#endif