#ifndef SRE_AUDIOSTYLE_H
#define SRE_AUDIOSTYLE_H

#include <Datatypes/TimeStamp.h>

typedef enum sre_audioStyleFlags
{
    SRE_AUDIOSTYLE_LOOPED = 1
} sre_audioStyleFlags;

typedef struct sre_audiostyle
{
    unsigned point_start;
    unsigned point_loop;
    unsigned point_end;

    sre_timeStamp fade_in;
    sre_timeStamp fade_out;

    sre_audioStyleFlags flags;
} sre_audiostyle;

#ifdef __cplusplus

namespace sre
{
    typedef sre_audiostyle audiostyle;
}

#endif


#endif