#ifndef SRE_LOG_H
#define SRE_LOG_H
#include <C_API.h>
#include <stdarg.h>

#ifdef __cplusplus
    #include <sstream>
#endif

#define SRE_LOG_DEBUG "" // Does nothing, it's the default if you don't specify anything
#define SRE_LOG_INFO "\x01"
#define SRE_LOG_WARN "\x02"

SRE_CAPI_BEGIN

#ifdef __cplusplus
namespace sre
{
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

        #ifdef SRE
            // Engine output streams
            extern sre::ostream edbg;
            extern sre::ostream einfo;
            extern sre::ostream ewarn;
        #endif

        #ifdef extern
            #undef extern
        #endif
        
        using std::endl;
    }
}
#endif


#ifndef SRE_DISABLE_LOGS
    #if SRE
        #define SRE_LOGGING_TYPE 1
    #else
        #define SRE_LOGGING_TYPE 0
    #endif

    // extern int sre_internallog(int type, int category, const char* fmt, va_list va)
    // extern int sre_internallogmsg(int type, int category, const char* msg, int length)

    static inline int sre_log(const char* fmt, ...)
    {
        extern int sre_internallog(int type, int category, const char* fmt, va_list va);

        int category;
        switch (fmt[0])
        {
            case '\x01':
            case '\x02':
            case '\x03':
            case '\x04':
                category = fmt[0];
                fmt++;
                break;
            default:
                category = 0;
        }

        int res;
        va_list va;
        va_start(va, fmt);
            res = sre_internallog(SRE_LOGGING_TYPE, category, fmt, va);
        va_end(va);

        return res;
    }

    static inline int sre_logmsg(const char* msg, const char* category_tag, int length)
    {
        extern int sre_internallogmsg(int type, int category, const char* msg, int length);
        return sre_internallogmsg(SRE_LOGGING_TYPE, category_tag ? category_tag[0] : 0, msg, length);
    }

    extern int sre_logEx(int type, int category, const char* fmt, va_list va);
    extern int sre_logsimpleEx(int type, int category, const char* str);

    #ifdef __cplusplus
        extern "C++" {
            namespace sre
            {
                template <typename... Args>
                static inline int log(const char* fmt, Args... args) { return sre_log(fmt, args...); }
                static inline int logmsg(const char* msg, const char* category=SRE_LOG_DEBUG, int length=0) { return sre_logmsg(msg, category, length); }

                static inline int log(const char* msg)
                {
                    const char* category_tag;
                    switch (msg[0])
                    {
                        case '\x01':
                        case '\x02':
                        case '\x03':
                            category_tag = msg;
                            msg++;
                            break;
                        default:
                            category_tag = NULL;
                    }

                    return sre_logmsg(msg, category_tag, 0);
                }
            }
        }
    #endif
#else
    #define sre_log(fmt, ...) (void)0
    #define sre_logmsg(msg, cat, len) (void)0

    #ifdef __cplusplus
        extern "C++" {
            namespace sre
            {
                template <typename... Args>
                inline int log(const char* fmt, Args&&... args)
                {
                    (void)fmt;
                    return 0;
                }
                inline int log(const char* msg, const char* category=SRE_LOG_DEBUG, int length=0)
                {
                    (void)msg;
                    (void)category;
                    (void)length;
                    return 0;
                }
            }
        }
    #endif
#endif

SRE_CAPI_END

#endif