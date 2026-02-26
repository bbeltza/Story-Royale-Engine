#include <Base/Log.h>
#include <time.h>

#include <utils/mem.h>

namespace sre
{
    // Structure containing a log message
    // *if you're asking why `size` is not a size_t we're not having messages longer than 2^31-1 haha*
    struct LogMsg
    {
        // Log message timestamps maybe?
        time_t _timestamp;
        char *buffer;
        int category;
        int type; // application, engine, or sdl...
        int size;
    };

    // Structure containing the logging instance
    struct Log
    {
        std::deque<LogMsg> msg_queue;
    };
}

namespace
{
    sre::Log log_instance;

    constexpr char CSTRING_INFO[] = "INFO";
    constexpr char CSTRING_DEBUG[] = "DEBUG";
    constexpr char CSTRING_ERROR[] = "ERROR";
    constexpr char CSTRING_POSTSTR[] = "]: ";

    constexpr const char* CATEGORY_STRINGS[] = {
        CSTRING_INFO,
        CSTRING_DEBUG,
        CSTRING_ERROR
    };
    constexpr size_t CATEGORY_SIZES[] = {
        sizeof(CSTRING_INFO),
        sizeof(CSTRING_DEBUG),
        sizeof(CSTRING_ERROR)
    };

    constexpr char TSTRING_APP[] = "APP";
    constexpr char TSTRING_ENGINE[] = "ENGINE";
    constexpr char TSTRING_SDL[] = "SDL";
    constexpr const char* TYPE_STRINGS[] = {
        TSTRING_APP,
        TSTRING_ENGINE,
        TSTRING_SDL
    };
    constexpr size_t TYPE_STRINGS[] = {
        sizeof(TSTRING_APP),
        sizeof(TSTRING_ENGINE),
        sizeof(TSTRING_SDL)
    };
}

void sre_logflush()
{
    while (!log_instance.msg_queue.empty())
    {
        auto& msg = log_instance.msg_queue.front();
        size_t buffer_size = msg.size + CATEGORY_SIZES[msg.category] + sizeof(CSTRING_POSTSTR) - 1;

        ut_dynsalloc(char, buffer, buffer_size);
        buffer[0] = '[';
        strcpy(buffer + 1, CATEGORY_STRINGS[msg.category]);
        strcpy(buffer + CATEGORY_SIZES[msg.category], CSTRING_POSTSTR);
        strcpy(buffer + buffer_size - msg.size, msg.buffer);
        buffer[buffer_size-1] = '\n';
        
        fwrite(buffer, buffer_size, 1, stdout);

        delete[] msg.buffer;
        log_instance.msg_queue.pop_back();
    }
}

int sre_log(const char* fmt, ...)
{
    int n;
    va_list va;
    va_start(va, fmt);
    n = sre_logva(fmt, va);
    va_end(va);

    return n;
}

int sre_logva(const char* fmt, va_list va)
{
    return sre_logEx(0, 0, fmt, va);
}

int sre_logEx(int type, int category, const char* fmt, va_list va)
{
    assert(fmt != NULL);

    sre::LogMsg newmsg;
    newmsg.category = category;
    newmsg.type = type;
    newmsg.size = vsnprintf(NULL, 0, fmt, va) + 1;
    newmsg.buffer = new char[newmsg.size];
    vsnprintf(newmsg.buffer, newmsg.size, fmt, va);

    log_instance.msg_queue.push_back(newmsg);
    sre_logflush();

    return newmsg.size;
}