#include <Base/Log.h>
#include <stdarg.h>

#include <utils/mem.h>

namespace sre
{
    // Structure containing a log message
    // *if you're asking why `size` is not a size_t we're not having messages longer than 2^31-1 haha*
    struct LogMsg
    {
        // Log message timestamps maybe?
        int _timestamp;
        int category;
        int type; // application, engine, or sdl...
        int size;
        char *buffer;
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
}

void sre_logflush()
{
    while (!log_instance.msg_queue.empty())
    {
        auto& msg = log_instance.msg_queue.front();
        size_t buffer_size = msg.size + sizeof("[LOG]:");

        ut_dynsalloc(char, buffer, buffer_size);
        strcpy(buffer, "[LOG]: ");
        strcat(buffer, msg.buffer);
        buffer[buffer_size-1] = '\n';
        
        fwrite(buffer, buffer_size, 1, stdout);

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
    assert(fmt != NULL);

    sre::LogMsg newmsg;
    newmsg.category = 0;
    newmsg.size = vsnprintf(NULL, 0, fmt, va) + 1;
    newmsg.buffer = new char[newmsg.size];
    vsnprintf(newmsg.buffer, newmsg.size, fmt, va);

    log_instance.msg_queue.push_back(newmsg);
    sre_logflush();

    return newmsg.size;
}