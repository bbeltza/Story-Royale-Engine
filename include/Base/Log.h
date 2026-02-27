#ifndef SRE_LOG_H
#define SRE_LOG_H
#include <C_API.h>
#include <stdarg.h>

SRE_CAPI_BEGIN

enum sre_LogCategory
{
    SRE_LOGCATEGORY_INFO,
    SRE_LOGCATEGORY_DEBUG,
    SRE_LOGCATEGORY_WARN,
    SRE_LOGCATEGORY_ERROR,
    SRE_NUM_LOGCATEGORIES
};

#ifdef __cplusplus
namespace sre
{
    enum LogCategory
    {
        LOGCATEGORY_INFO = SRE_LOGCATEGORY_INFO,
        LOGCATEGORY_DEBUG = SRE_LOGCATEGORY_DEBUG,
        LOGCATEGORY_WARN = SRE_LOGCATEGORY_WARN,
        LOGCATEGORY_ERROR = SRE_LOGCATEGORY_ERROR,
        NUM_LOGCATEGORIES = SRE_NUM_LOGCATEGORIES
    };
}
#endif

#ifdef SRE_DISABLE_LOGS
    #define sre_logEx(t, c, fmt, va) (void)0
    #define sre_logsimpleEx(t, c, str) (void)0

    extern "C++" {
        namespace sre
        {
            template <LogCategory category=LOGCATEGORY_DEBUG, typename... Args>
            inline int log(const char* fmt, Args&&... args)
            {
                (void)fmt;
                
                return 0;
            }
        }
    }
#else
    #ifdef SRE
        #define __LTYPE 1
    #else
        #define __LTYPE 0
    #endif

    extern int sre_logEx(int type, int category, const char* fmt, va_list va);
    extern int sre_logsimpleEx(int type, int category, const char* str);

    #ifdef __cplusplus
    extern "C++" {
        namespace sre
        {
            inline int _loglegacy(int category, const char* fmt, ...)
            {
                va_list va;
                int n;
                va_start(va, fmt);
                n = sre_logEx(__LTYPE, category, fmt, va);
                va_end(va);
                return n;
            }

            template <LogCategory category=LOGCATEGORY_DEBUG, typename... Args>
            inline int log(const char* fmt, Args... args)
            {
                return _loglegacy(category, fmt, args...); 
            }

            template <LogCategory category=LOGCATEGORY_INFO>
            inline int log(const char* str) { return sre_logsimpleEx(__LTYPE, category, str); }
        }
    }
    #else
    static inline int sre_log(enum sre_LogCategory category, const char* fmt, ...)
    {
        va_list va;
        int n;
        va_start(va, fmt);
        n = sre_logEx(
            __LTYPE,
            category,
            fmt,
            va
        );
        va_end(va);
        return n;
    }

    static inline int sre_logsimple(enum sre_LogCategory category, const char* str)
    {
        return sre_logsimpleEx(
            __LTYPE,
            category,
            str
        );
    }
    #endif
#endif

SRE_CAPI_END

#endif