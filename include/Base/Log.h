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
extern "C++"
{
    #include <sstream>
}
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

    extern "C++"
    {
        #ifndef SRE_DISABLE_LOGS
            using std::ostream;
        #else
            struct ostream
            {
                template <typename T>
                inline sre::ostream& operator <<(T&& type) { return *this; }
                inline sre::ostream& operator<<(std::ostream& fn(std::ostream&)) { return *this; }
            };
            #define extern static
        #endif

        extern sre::ostream out;
        extern sre::ostream odbg;
        extern sre::ostream oinfo;
        extern sre::ostream owarn;
        extern sre::ostream oerr;

        #ifdef SRE
            // Engine output streams
            extern sre::ostream edbg;
            extern sre::ostream einfo;
            extern sre::ostream ewarn;
            extern sre::ostream eerr;
        #endif

        #ifdef extern
            #undef extern
        #endif
        
        using std::endl;
    }
}
#endif

#ifdef SRE_DISABLE_LOGS
    #define sre_logEx(t, c, fmt, va) (void)0
    #define sre_logsimpleEx(t, c, str) (void)0

    #ifdef __cplusplus
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
    #endif
#else
    #if SRE
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
            static inline int log(const char* fmt, Args... args)
            {
                return _loglegacy(category, fmt, args...); 
            }

            template <LogCategory category=LOGCATEGORY_INFO>
            static inline int log(const char* str) { return sre_logsimpleEx(__LTYPE, category, str); }

            
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