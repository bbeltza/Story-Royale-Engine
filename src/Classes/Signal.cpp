#include <utility>

#include <SDL.hpp>

#include "Engine.hpp"

void* Signal::create_sem()
{
    return SDL_CreateSemaphore(0);
}

Signal::~Signal()
{
    Connection* item = m_handlerListHead;
    while (this->m_handlerListHead)
    {
        m_handlerListHead = item->m_next;
        delete item;
        item = m_handlerListHead;
    }

    SDL_DestroySemaphore((SDL_sem*)m_waitSem);
}

void Signal::DisconnectAll()
{
    Connection* item = m_handlerListHead;
    while (this->m_handlerListHead)
    {
        m_handlerListHead = item->m_next;
        item->m_connected = false;
        item = m_handlerListHead;
    }
}

#define SignalConnect(o) \
auto connetion = new Connection(this, fn, data, o);  \
    if (m_handlerListHead)  \
        connetion->m_next = m_handlerListHead;  \
    this->m_handlerListHead = connetion;    \
    return connetion;   \

Connection* Signal::Connect(EventFunction fn, void* data)
{
    SignalConnect(false)
}

Connection* Signal::Once(EventFunction fn, void* data)
{
    SignalConnect(true)
}

Signal::argbase Signal::Wait()
{
    SDL_SemWait((SDL_sem*)m_waitSem);
    return return_data;
}

void Signal::Fire(void* first, ...)
{
    va_list va;
    va_start(va, first);

    return_data.args[0] = first;
    for (int i = 1; i < 6; i++)
        return_data.args[i] = va_arg(va, void*);

    va_end(va);

    Connection* item = m_handlerListHead;
    SDL_SemPost((SDL_sem*)m_waitSem);
    while (item)
    {
        if (item->m_connected)
        {
            if (m_multithreaded)
                Engine->ThreadPool.CreateImmediateThread(item->m_fn, userdata, item->userdata,
                    return_data.args[0], return_data.args[1],
                    return_data.args[2], return_data.args[3],
                    return_data.args[4], return_data.args[5]
                    );
            else
                item->m_fn(userdata, item->userdata,
                    return_data.args[0], return_data.args[1],
                    return_data.args[2], return_data.args[3],
                    return_data.args[4], return_data.args[5]
                    );
            
            if (item->m_once)
            {
                m_handlerListHead = item->m_next;
                delete item;
            }
        }
        
        item = item->m_next;
    }
}