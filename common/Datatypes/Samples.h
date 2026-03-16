#ifndef SRE_SAMPLES_H
#define SRE_SAMPLES_H

typedef float sre_Sample;
typedef sre_Sample sre_Sample2D[2];

#ifdef __cplusplus

namespace sre
{
    using sample = sre_Sample;
    using sample2D = sre_Sample2D;
}

#endif

#endif