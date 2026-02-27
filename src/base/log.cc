#include <Base/Log.h>
#include <utils/mem.h>

#include <standard>

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
            size(buffer_size) {}
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
    };

    enum LogType
    {
        LOGTYPE_APP,
        LOGTYPE_ENGINE,
        LOGTYPE_SDL
    };
}

namespace
{
    sre::Log &log_instance()
    {
        static sre::Log i;
        return i;
    }

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

extern "C" void sre_logflush()
{
    auto& instance = log_instance();

    std::lock_guard<std::recursive_mutex> guard{instance.mutex};
    while (!instance.msg_queue.empty())
    {
        auto& msg = instance.msg_queue.front();
        static constexpr char extra_characters[] = "[]: ";
        const size_t typestr_size = TYPE_SIZES[msg.type];
        const size_t catstr_size = CATEGORY_SIZES[msg.category];
        size_t buffer_size = typestr_size + catstr_size + msg.size + sizeof(extra_characters) + 2;

        const char* const strings[] = {
            extra_characters,
            TYPE_STRINGS[msg.type],
            extra_characters + 1,
            extra_characters,
            CATEGORY_STRINGS[msg.category],
            extra_characters + 1,
            msg.buffer       
        };
        const size_t sizes[] = {
            1,
            typestr_size,
            1,
            1,
            catstr_size,
            sizeof(extra_characters) - 2,
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
        
        // Console writing code
        FILE* console = msg.category == sre::LOGCATEGORY_INFO ? stdout : stderr;
        fwrite(buffer, buffer_size, 1, console);
        //

        instance.msg_queue.pop_front();
    }
}

int sre_logEx(int type, int category, const char* fmt, va_list va)
{
    assert(fmt != NULL);
    assert(type >= sre::LOGTYPE_APP && type <= sre::LOGTYPE_SDL);

    auto& instance = log_instance();
    std::lock_guard<std::recursive_mutex> guard{instance.mutex};

    va_list cpy;
    va_copy(cpy, va);
    int len = vsnprintf(NULL, 0, fmt, cpy) + 1;
    instance.msg_queue.emplace_back(
        type,
        category,
        len
    );
    auto& msg = instance.msg_queue.back();
    
    if (len == 1)
        msg.buffer[0] = '\0';
    else
        vsnprintf(msg.buffer, msg.size, fmt, va);

    sre_logflush();

    return msg.size - 1;
}

int sre_logsimpleEx(int type, int category, const char* str)
{
    assert(str != NULL);
    assert(type >= sre::LOGTYPE_APP && type <= sre::LOGTYPE_SDL);

    auto& instance = log_instance();

    std::lock_guard<std::recursive_mutex> barney{instance.mutex};
    int len = strlen(str) + 1;
    instance.msg_queue.emplace_back(
        type,
        category,
        len
    );
    strncpy(instance.msg_queue.back().buffer, str, len);
    sre_logflush();

    return len - 1;
}