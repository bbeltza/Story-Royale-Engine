#include <standard.h>
#include <SDL.hpp>
#include "EngineBase.h"

class ThreadPool
{
    ENGINE_BASE
    public:
    ThreadPool();
    ~ThreadPool();

    template <class Func, class ...arg>
    inline void Queue(Func func, arg... args)
    {
        return _queue_func((ThreadFunction)func, args...);
    }

    static const size_t NUM_THREADS = 12;

    private:
    typedef void (*ThreadFunction)(...);
    struct FuncBase
    {
        ThreadFunction func;
        long long args[8];
    };
    struct CallbackArgs
    {
        ThreadPool* self;
        int index;
    };

    SDL_mutex* queue_mutex = SDL_CreateMutex();
    SDL_Thread* threads[NUM_THREADS];
    std::queue<FuncBase> func_queue;


    static int thread_callback(CallbackArgs* args);

    void _queue_func(ThreadFunction func, ...);
};