#ifndef SRE_AUDIOSTYLE_H
#define SRE_AUDIOSTYLE_H

#include <Datatypes/TimeStamp.h>

typedef struct sre_audiostyle
{
    unsigned point_start;
    unsigned point_loop;
    unsigned point_end;

    sre_timeStamp fade_in;
    sre_timeStamp fade_out;
} sre_audiostyle;


#endif