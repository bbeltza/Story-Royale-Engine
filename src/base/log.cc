#include <Base/Log.h>
#include <utils/mem.h>

#include <standard>

namespace sre
{
    template <LogCategory category=LOGCATEGORY_DEBUG, int type=0>
    class _strbuf: public std::stringbuf
    {
        int sync() override
        {
            auto s = str();
            if (s.back() == '\n') s.pop_back();
            sre_logsimpleEx(type, category, s.c_str());
            str("");
            return 0;
        }

    public:
        static _strbuf buffer;
    };

    template <LogCategory category, int type>
    _strbuf<category, type> _strbuf<category, type>::buffer;

    sre::ostream out{&_strbuf<>::buffer};
    sre::ostream odbg{&_strbuf<LOGCATEGORY_DEBUG>::buffer};
    sre::ostream oinfo{&_strbuf<LOGCATEGORY_INFO>::buffer};
    sre::ostream owarn{&_strbuf<LOGCATEGORY_WARN>::buffer};
    sre::ostream oerr{&_strbuf<LOGCATEGORY_ERROR>::buffer};

    sre::ostream edbg{&_strbuf<LOGCATEGORY_DEBUG, 1>::buffer};
    sre::ostream einfo{&_strbuf<LOGCATEGORY_INFO, 1>::buffer};
    sre::ostream ewarn{&_strbuf<LOGCATEGORY_WARN, 1>::buffer};
    sre::ostream eerr{&_strbuf<LOGCATEGORY_ERROR, 1>::buffer};

    enum LogType
    {
        LOGTYPE_APP,
        LOGTYPE_ENGINE,
        LOGTYPE_SDL
    };
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
        using namespace sre;
        switch (category)
        {
            case LOGCATEGORY_INFO:
                return ANDROID_LOG_INFO;
            case LOGCATEGORY_DEBUG:
                return ANDROID_LOG_DEBUG;
            case LOGCATEGORY_WARN:
                return ANDROID_LOG_WARN;
            case LOGCATEGORY_ERROR:
                return ANDROID_LOG_ERROR;
            default:
                abort();
        }

        return ANDROID_LOG_UNKNOWN;
    }

    extern "C" int sre_logEx(int type, int category, const char* fmt, va_list va)
    {
        return __android_log_vprint(map_cat_prio(category), map_type_tag(type), fmt, va);
    }

    extern "C" int sre_logsimpleEx(int type, int category, const char* str)
    {
        return __android_log_write(map_cat_prio(category), map_type_tag(type), str);
    }
#else

#if _WIN32 && _DEBUG
    #include <Windows.h>
#endif

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
            #if _WIN32 && _DEBUG
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
        std::recursive_mutex mutex;

        void flush();
    };
}

namespace
{
    constexpr char CSTRING_INFO[] = "INFO";
    constexpr char CSTRING_DEBUG[] = "DEBUG";
    constexpr char CSTRING_WARN[] = "WARN";
    constexpr char CSTRING_ERROR[] = "ERROR";

    constexpr const char* CATEGORY_STRINGS[] = {
        CSTRING_INFO,
        CSTRING_DEBUG,
        CSTRING_WARN,
        CSTRING_ERROR
    };
    constexpr size_t CATEGORY_SIZES[] = {
        sizeof(CSTRING_INFO) - 1,
        sizeof(CSTRING_DEBUG) - 1,
        sizeof(CSTRING_WARN) - 1,
        sizeof(CSTRING_ERROR) - 1
    };

    static_assert(
        ut_arrcount(CATEGORY_STRINGS) == ut_arrcount(CATEGORY_SIZES) &&
        ut_arrcount(CATEGORY_STRINGS) == sre::NUM_LOGCATEGORIES,
        "Log category string count mismatching with the enum categories");

    constexpr char TSTRING_APP[] = "APP";
    constexpr char TSTRING_ENGINE[] = "ENGINE";
    constexpr char TSTRING_SDL[] = "SDL";
    constexpr const char* TYPE_STRINGS[] = {
        TSTRING_APP,
        TSTRING_ENGINE,
        TSTRING_SDL
    };
    constexpr size_t TYPE_SIZES[] = {
        sizeof(TSTRING_APP) - 1,
        sizeof(TSTRING_ENGINE) - 1,
        sizeof(TSTRING_SDL) - 1
    };
}

#include <SDL_thread.h>

static sre::Log* inst;
int init_logsifnoinst()
{
    if (!inst)
    {
        SDL_Thread* thrd = SDL_CreateThread([](void*) {
            sre::Log instance;
            inst = &instance;
            while (true)
            {
                instance.flush();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            return 0;
        }, "Logging loop", NULL);
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
    std::lock_guard<std::recursive_mutex> guard{mutex};
    while (!msg_queue.empty())
    {
#ifdef ANDROID
        __android_log_write(ANDROID_LOG_DEFAULT, "APP", msg_queue.front().buffer);
#else
        static constexpr char extra_characters[] = "[]: ";
        static constexpr char style_characters_begin[] = "\033[30;1m";
        static constexpr char style_characters_end[] = "\033[0m";

        auto& msg = msg_queue.front();

        const size_t typestr_size = TYPE_SIZES[msg.type];
        const size_t catstr_size = CATEGORY_SIZES[msg.category];
        size_t buffer_size = typestr_size + catstr_size + msg.size + sizeof(extra_characters) + sizeof(style_characters_begin) + sizeof(style_characters_end);

        const char* const strings[] = {
            style_characters_begin,
            extra_characters,
            TYPE_STRINGS[msg.type],
            extra_characters + 1,
            extra_characters,
            CATEGORY_STRINGS[msg.category],
            extra_characters + 1,
            style_characters_end,

            msg.buffer
        };
        const size_t sizes[] = {
            sizeof(style_characters_begin) - 1,
            1,
            typestr_size,
            1,
            1,
            catstr_size,
            sizeof(extra_characters) - 2,
            sizeof(style_characters_end) - 1,

            static_cast<size_t>(msg.size)
        };
        constexpr int STRINGS_COUNT = ut_arrcount(strings);

        ut_dynsalloc(char, buffer, buffer_size);
        char* buffptr = buffer;
        for (int i = 0; i < STRINGS_COUNT; i++)
        {
            strncpy(buffptr, strings[i], sizes[i]);
            buffptr += sizes[i];
        }
        *buffptr = '\n';

        switch (msg.category)
        {
        case sre::LOGCATEGORY_WARN:
            buffer[3] = '3';
            break;
        case sre::LOGCATEGORY_ERROR:
            buffer[3] = '1';
            break;
        case sre::LOGCATEGORY_INFO:
            buffer[3] = '6';
            break;
        default:
            buffer[2] = '0';
            buffer[3] = '0';
            break;
        }

        // Console writing code
        FILE* console = msg.category == sre::LOGCATEGORY_INFO ? stdout : stderr;
        fwrite(buffer, buffer_size, 1, console);
        //
#endif
        msg_queue.pop_front();
    }
}

int sre_logEx(int type, int category, const char* fmt, va_list va)
{
    assert(fmt != NULL);
    assert(type >= sre::LOGTYPE_APP && type <= sre::LOGTYPE_SDL);

    init_logsifnoinst();
    std::lock_guard<std::recursive_mutex> guard{inst->mutex};

    va_list cpy;
    va_copy(cpy, va);
    int len = vsnprintf(NULL, 0, fmt, cpy) + 1;
    inst->msg_queue.emplace_back(
        type,
        category,
        len
    );
    auto& msg = inst->msg_queue.back();

    if (len == 1)
        msg.buffer[0] = '\0';
    else
        vsnprintf(msg.buffer, msg.size, fmt, va);

    return msg.size - 1;
}

int sre_logsimpleEx(int type, int category, const char* str)
{
    assert(str != NULL);
    assert(type >= sre::LOGTYPE_APP && type <= sre::LOGTYPE_SDL);

    init_logsifnoinst();

    std::lock_guard<std::recursive_mutex> barney{inst->mutex};
    int len = static_cast<int>(strlen(str) + 1);
    inst->msg_queue.emplace_back(
        type,
        category,
        len
    );
    strncpy(inst->msg_queue.back().buffer, str, len);

    return len - 1;
}

#endif