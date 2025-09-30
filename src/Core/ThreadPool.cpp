#include "Base/ThreadPool.hpp"

void ThrdPool::detach_threads()
{
    for (Thread *thrd : immediate_threads)
        delete thrd;
    immediate_threads.clear();
}

ThrdPool::ThrdPool(EngineClass* engine): m_Engine(engine)
{
    
}

ThrdPool::~ThrdPool()
{
}