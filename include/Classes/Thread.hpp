#pragma once
#include <SDL.hpp>
#include <standard>
#define TEMPL template <class F, class... _args>

#include "internal_def.hh"

class Thread;

namespace Threads // Designed to replace ThreadPool
{
    TEMPL Thread Create(F&& func, _args&&... args);
}

__def_internal(__update_classes)

class Thread
{
    friend class ThrdPool;
public:

typedef void* (*Function)(...);
static const uint8_t NUM_ARGS = 8;  
public:
    void Join();
    void Detach();

    ~Thread();
    Thread() {}
    Thread(Thread&& moving);

    Thread& operator=(Thread&& moving);

    struct data
    {
        ~data();
        inline bool operator==(const data& other) const { return this == &other; }
        Thread* thrd;
        SDL_Thread* handle;
        Function func;
        void* args[NUM_ARGS];
    } *_data = nullptr;
private:
    Thread(Function func, ...);
    Thread(const Thread& other) = delete;

    TEMPL friend Thread Threads::Create(F&& func, _args&&... args);

    static int invokethread_handler(data *);
    static void queue_removing();
    static std::list<data> threads_list;
    static std::queue<data*> to_remove;

    __friend_internal(__update_classes)
};

TEMPL Thread Threads::Create(F&& func, _args&&... args) { return Thread((Thread::Function)func, args...); }

#undef TEMPL