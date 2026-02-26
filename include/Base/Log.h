#ifndef SRE_LOG_H
#define SRE_LOG_H
#include <C_API.h>
#include <stdarg.h>

SRE_CAPI_BEGIN

enum sre_LogCategory
{
    SRE_CATEGORY_INFO,
    SRE_CATEGORY_DEBUG,
    SRE_CATEGORY_ERROR
};

#ifdef SRE_DISABLE_LOGS
    #define sre_log(fmt, ...) (void)0
    #define sre_logv(fmt, va) (void)0


#else
    extern int sre_log(const char* fmt, ...);
    extern int sre_logva(const char* fmt, va_list va);

    extern int sre_logEx(int category, const char* fmt, va_list va);

    #ifdef __cplusplus
    extern "C++" {
        namespace sre
        {
            template <typename... Args>
            inline int log(const char* fmt, Args&&... args) { return sre_log(fmt, std::forward<Args>(args)...); }
        }
    }
    #endif
#endif

SRE_CAPI_END

#endif

// Just include utils/logging for now, I wanna push myself on including Base/Log instead of this
#include <utils/logging.h>