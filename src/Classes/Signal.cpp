#include <SDL.hpp>
#include "Classes/Signal.h"

static void invoke_func(EventFunction func, void* userdata)
{
    SDL_CreateThread((SDL_ThreadFunction)func, NULL, userdata);
}

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
auto connetion = new Connection(this, fn, o);  \
    if (m_handlerListHead)  \
        connetion->m_next = m_handlerListHead;  \
    this->m_handlerListHead = connetion;    \
    return connetion;   \

Connection* Signal::Connect(EventFunction fn)
{
    SignalConnect(false)
}

Connection* Signal::Once(EventFunction fn)
{
    SignalConnect(true)
}

void* Signal::Wait()
{
    SDL_SemWait((SDL_sem*)m_waitSem);
    return m_returnData;
}

void Signal::Fire(void* userdata)
{
    m_returnData = userdata;

    Connection* item = m_handlerListHead;
    SDL_SemPost((SDL_sem*)m_waitSem);
    while (item)
    {
        if (item->m_connected)
        {
            invoke_func(item->m_fn, userdata);
            if (item->m_once)
            {
                m_handlerListHead = item->m_next;
                delete item;
            }
        }
        
        item = item->m_next;
    }
}