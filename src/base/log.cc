#include <Base/Log.h>
#include <utils/mem.h>
#include <utils/lockfile.h>

#include <standard>

extern "C" int sre_internallogmsg(int type, int category, const char* msg, int length);
extern "C" int sre_internallog(int type, int category, const char* fmt, va_list va);

enum sreLogType
{
    LOGTYPE_APP,
    LOGTYPE_ENGINE,
    LOGTYPE_SDL
};

enum sreLogCategory
{
    LOGCATEGORY_DEBUG = 0x00,
    LOGCATEGORY_INFO = 0x01,
    LOGCATEGORY_WARN = 0x02,
    LOGCATEGORY_ERROR = 0x03,
    LOGCATEGORY_CRITICAL = 0x04,

    NUM_CATEGORIES
};

namespace sre
{
    template <int category, int type=0>
    class _strbuf: public std::stringbuf
    {
        int sync() override
        {
            auto s = str();
            if (s.back() == '\n') s.pop_back();
            sre_internallogmsg(type, category, s.c_str(), static_cast<int>(s.size()));
            str("");
            return 0;
        }

    public:
        static _strbuf buffer;
    };

    template <int category, int type>
    _strbuf<category, type> _strbuf<category, type>::buffer;

    sre::ostream out{&_strbuf<LOGCATEGORY_DEBUG>::buffer};
    sre::ostream oinfo{&_strbuf<LOGCATEGORY_INFO>::buffer};
    sre::ostream owarn{&_strbuf<LOGCATEGORY_WARN>::buffer};

    sre::ostream edbg{&_strbuf<LOGCATEGORY_DEBUG, 1>::buffer};
    sre::ostream einfo{&_strbuf<LOGCATEGORY_INFO, 1>::buffer};
    sre::ostream ewarn{&_strbuf<LOGCATEGORY_WARN, 1>::buffer};
}

#ifdef ANDROID
    #include <android/log.h>

    static const char* map_type_tag(int type)
    {
        using namespace sre;
        switch(type)
        {
            case LOGTYPE_APP:
                return "APP";
            case LOGTYPE_ENGINE:
                return "ENGINE";
            case LOGTYPE_SDL:
                return "SDL";
            default:
                abort();
        }

        return NULL;
    }

    static int map_cat_prio(int category)
    {
        switch (category)
        {
            case LOGCATEGORY_DEBUG:
                return ANDROID_LOG_DEBUG;
            case LOGCATEGORY_INFO:
                return ANDROID_LOG_INFO;
            case LOGCATEGORY_WARN:
                return ANDROID_LOG_WARN;
            case LOGCATEGORY_ERROR:
                return ANDROID_LOG_ERROR;
            case LOGCATEGORY_CRITICAL:
                return ANDROID_LOG_FATAL;
            default:
                abort();
        }

        return ANDROID_LOG_UNKNOWN;
    }

    extern "C" int sre_internallog(int type, int category, const char* fmt, va_list va)
    {
        return __android_log_vprint(map_cat_prio(category), map_type_tag(type), fmt, va);
    }

    extern "C" int sre_internallogmsg(int type, int category, const char* str, int length)
    {
        if (!length)
            return __android_log_write(map_cat_prio(category), map_type_tag(type), str);
        
        ut_dynsalloc(char, buf, length + 1);
        strncpy(buf, str, length);
        buf[length] = '\0';
        return __android_log_write(map_cat_prio(category), map_type_tag(type), buf);
    }
#else

#if _WIN32
    #include <Windows.h>
#endif

#include <SDL_thread.h>
#include <SDL_mutex.h>
#include <thread>

namespace sre
{
    // Structure containing a log message
    // *if you're asking why `size` is not a size_t we're not having messages longer than 2^31-1 haha*
    struct LogMsg
    {
        // Log message timestamps maybe?
        time_t _timestamp;
        char *buffer;
        int type; // application, engine, or sdl...
        int category;
        int size;

        LogMsg(int type, int category, int buffer_size):
            _timestamp(time(NULL)),
            buffer(static_cast<char*>(malloc(buffer_size))),
            category(category),
            type(type),
            size(buffer_size)
        {
            #if _WIN32 && !defined(NDEBUG) && 0
                if (category == sre::LOGCATEGORY_ERROR && type == 2)
                {
                    if (IsDebuggerPresent())
                        DebugBreak();
                }
            #endif
        }
        ~LogMsg() { free(buffer); }
    };

    template <typename T>
    struct MallocAllocator
    {
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;

        using reference = T&;
        using const_reference = const T&;

        using size_type       = size_t;
        using difference_type = ptrdiff_t;

        using propagate_on_container_move_assignment = std::true_type;
        using is_always_equal = std::true_type;

        template <typename U>
        struct rebind
        {
            using other = MallocAllocator<U>;
        };

        MallocAllocator() throw() {}
        MallocAllocator(const MallocAllocator&) throw() {}

        template <typename U>
        MallocAllocator(const MallocAllocator<U>&) throw() {}

        ~MallocAllocator() throw() {}

        pointer allocate(size_type size, void const * = 0) {
            if (0 == size)
                return NULL;
            pointer ptr = (pointer)malloc(size * sizeof(T));
            if (ptr == NULL)
                throw std::bad_alloc();
            return ptr;
        }
        void deallocate(pointer ptr, size_type) { free(ptr); }
    };

    // Structure containing the logging instance
    struct Log
    {
        std::deque<LogMsg, MallocAllocator<LogMsg>> msg_queue;

        SDL_mutex* mutex = SDL_CreateMutex();
        ~Log() { SDL_DestroyMutex(mutex); }

        void flush();
    };
}

namespace
{
    namespace strs
    {
        static constexpr char CAT1[] = "DEBUG";
        static constexpr char CAT2[] = "INFO";
        static constexpr char CAT3[] = "WARN";
        static constexpr char CAT4[] = "ERROR";
        static constexpr char CAT5[] = "CRITICAL";

        static constexpr char TYPE1[] = "APP";
        static constexpr char TYPE2[] = "ENGINE";
        static constexpr char TYPE3[] = "SDL";
    };

    constexpr const char* CATEGORY_STRINGS[] = {
        strs::CAT1,
        strs::CAT2,
        strs::CAT3,
        strs::CAT4,
        strs::CAT5
    };
    constexpr size_t CATEGORY_SIZES[] = {
        sizeof(strs::CAT1) - 1,
        sizeof(strs::CAT2) - 1,
        sizeof(strs::CAT3) - 1,
        sizeof(strs::CAT4) - 1,
        sizeof(strs::CAT5) - 1
    };

    constexpr const char* TYPE_STRINGS[] = {
        strs::TYPE1,
        strs::TYPE2,
        strs::TYPE3
    };
    constexpr size_t TYPE_SIZES[] = {
        sizeof(strs::TYPE1) - 1,
        sizeof(strs::TYPE2) - 1,
        sizeof(strs::TYPE3) - 1
    };

    static_assert(
        ut_arrcount(CATEGORY_STRINGS) == ut_arrcount(CATEGORY_SIZES) &&
        ut_arrcount(CATEGORY_STRINGS) == NUM_CATEGORIES,
        "Log category string count mismatching with the existing categories");
}

static sre::Log* inst;
static int logloop_func(void*)
{
    size_t niterations = 0;
    sre::Log instance;
    inst = &instance;
    while (true)
    {
        instance.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        niterations++;
    }

    return 0;
}

int init_logsifnoinst()
{
    if (!inst)
    {
        SDL_Thread* thrd = SDL_CreateThread(logloop_func, "Logging loop", NULL);
        SDL_DetachThread(thrd);

        while (!inst)
            std::this_thread::yield();

        std::atexit([]() { inst->flush(); });
    }

    return 0;
}
static int _ = init_logsifnoinst();

void sre::Log::flush()
{
    SDL_LockMutex(mutex);
    while (!msg_queue.empty())
    {
        auto& msg = msg_queue.front();

        const char* const typestr = TYPE_STRINGS[msg.type];
        const char* const catstr = CATEGORY_STRINGS[msg.category];
        const size_t typestr_size = TYPE_SIZES[msg.type];
        const size_t catstr_size = CATEGORY_SIZES[msg.category];
        
        size_t header_size = 1 + typestr_size + 2 + catstr_size + 3; // Header: "[(TYPE)][(CATEGORY)]: \0"
        ut_dynsalloc(char, header_buf, header_size);

        {
            char* headerptr = header_buf;
            headerptr[0] = '[';
            headerptr += 1;
            strncpy(headerptr, typestr, typestr_size);
            headerptr += typestr_size;
            headerptr[0] = ']';
            headerptr[1] = '[';
            headerptr += 2;
            strncpy(headerptr, catstr, catstr_size);
            headerptr += catstr_size;
            headerptr[0] = ']';
            headerptr[1] = ':';
            headerptr[2] = ' ';
        }

        // Console writing code
        msg.buffer[msg.size-1] = '\n';

        FILE* console = msg.category == LOGCATEGORY_INFO ? stdout : stderr;
        flockfile(console);
            #if _WIN32
                HANDLE hcon = GetStdHandle(msg.category == LOGCATEGORY_INFO ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
                
                #ifdef ENABLE_VIRTUAL_TERMINAL_PROCESSING
                    DWORD dmode;
                    GetConsoleMode(hcon, &dmode);
                    if (!(dmode & ENABLE_VIRTUAL_TERMINAL_PROCESSING))
                #endif
                {
                    static constexpr WORD cat_attribs[] = {
                        0,
                        FOREGROUND_BLUE | FOREGROUND_GREEN,
                        FOREGROUND_RED | FOREGROUND_GREEN,
                        FOREGROUND_RED,
                        FOREGROUND_RED | FOREGROUND_BLUE // Critical will be purple! :D (I just wanted a darker tone for this...)
                    };
                    WORD wattribs = FOREGROUND_INTENSITY | cat_attribs[msg.category];

                    CONSOLE_SCREEN_BUFFER_INFO old_buffinfo;
                    GetConsoleScreenBufferInfo(hcon, &old_buffinfo);
                    SetConsoleTextAttribute(hcon, wattribs);
                        WriteConsoleA(hcon, header_buf, static_cast<DWORD>(header_size), NULL, NULL);
                    SetConsoleTextAttribute(hcon, old_buffinfo.wAttributes);
                        WriteConsoleA(hcon, msg.buffer, msg.size, NULL, NULL);
                    
                    goto WIN32_FINISHCONSOLE;
                }                
            #endif

            {
                static const char RESET_ESC[] = "\033[0m";
                char escape[] = "\033[1;1;39m";
                switch (msg.category)
                {
                    case LOGCATEGORY_DEBUG: break; // "30"
                    case LOGCATEGORY_INFO: escape[7] = '6'; break;
                    case LOGCATEGORY_WARN: escape[7] = '3'; break;
                    case LOGCATEGORY_ERROR: escape[7] = '1'; break;
                    case LOGCATEGORY_CRITICAL: escape[7] = '1'; escape[2] = '2'; break;
                    default: break;
                }

                fwrite_unlocked(escape, 1, sizeof(escape), console);
                    fwrite_unlocked(header_buf, 1, header_size, console);
                fwrite_unlocked(RESET_ESC, 1, sizeof(RESET_ESC), console);
                    fwrite_unlocked(msg.buffer, 1, msg.size, console);
            }
            
            #if _WIN32
                WIN32_FINISHCONSOLE:
            #endif
        funlockfile(console);

        //
        msg_queue.pop_front();
    }
    SDL_UnlockMutex(mutex);
}

static sre::LogMsg& newlog(int type, int category, int len)
{
    assert(category >= LOGCATEGORY_DEBUG && category <= LOGCATEGORY_CRITICAL);
    assert(type >= LOGTYPE_APP && type <= LOGTYPE_SDL);
    init_logsifnoinst();
    SDL_LockMutex(inst->mutex);

    inst->msg_queue.emplace_back(
        type,
        category,
        len
    );

    return inst->msg_queue.back();
    // CAUTION: It doesn't lock the instance's mutex
}

int sre_internallog(int type, int category, const char* fmt, va_list va)
{
    assert(fmt != NULL);

    int len;
    {
        va_list cpy;
        va_copy(cpy, va);
        len = vsnprintf(NULL, 0, fmt, cpy) + 1;
    }
    
    auto &msg = newlog(type, category, len);
    if (len == 1)
        msg.buffer[0] = '\0';
    else
        vsnprintf(msg.buffer, len, fmt, va);

    SDL_UnlockMutex(inst->mutex);
    return len - 1;
}

int sre_internallogmsg(int type, int category, const char* str, int length)
{
    assert(str != NULL);
    assert(type >= LOGTYPE_APP && type <= LOGTYPE_SDL);

    int len = length ? length + 1 : static_cast<int>(strlen(str) + 1);
    auto &msg = newlog(type, category, len);
    strncpy(inst->msg_queue.back().buffer, str, len);

    SDL_UnlockMutex(inst->mutex);
    return len - 1;
}

#endif
