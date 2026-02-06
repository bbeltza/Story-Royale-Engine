#ifndef SRE_LOG_H
#define SRE_LOG_H
#include <C_API.h>

SRE_CAPI_BEGIN

#ifdef SRE_LOG_DISABLE
    #define sre_log(...) (void)0
#else
    extern int sre_log(const char* fmt, ...);
#endif

SRE_CAPI_END

#endif