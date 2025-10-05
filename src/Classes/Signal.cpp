#include <utility>

#include <SDL.hpp>

#include "Engine.hpp"

void *Signal::create_sem()
{
    return SDL_CreateSemaphore(0);
}

Signal::~Signal()
{
    DisconnectAll();
    if (SDL_WasInit(0)) SDL_DestroySemaphore((SDL_sem *)m_waitSem);
}

void Signal::DisconnectAll()
{
    while (!m_connections.empty())
    {
        auto item = *m_connections.begin();
        delete item;
        m_connections.erase(item);
    }
}

#define SignalConnect(o)                                 \
    auto connection = new Connection(this, fn, data, o); \
    m_connections.insert(connection);                    \
    return connection;

Connection *Signal::Connect(EventFunction fn, void *data){
    SignalConnect(false)}

Connection *Signal::Once(EventFunction fn, void *data){
    SignalConnect(true)}

Signal::argbase Signal::Wait()
{
    SDL_SemWait((SDL_sem *)m_waitSem);
    return return_data;
}

static std::vector<Connection*> delete_list;
void Signal::Fire(void *first, ...)
{
    va_list va;
    va_start(va, first);

    return_data.args[0] = first;
    for (int i = 1; i < 6; i++)
        return_data.args[i] = va_arg(va, void *);

    va_end(va);

    m_firing = true;
    SDL_SemPost((SDL_sem *)m_waitSem);
    for (auto item : m_connections)
    {
        if (m_multithreaded)
            Engine->ThreadPool.CreateImmediateThread(item->m_fn, userdata, item->userdata,
                                                     return_data.args[0], return_data.args[1],
                                                     return_data.args[2], return_data.args[3],
                                                     return_data.args[4], return_data.args[5]);
        else
            item->m_fn(userdata, item->userdata,
                       return_data.args[0], return_data.args[1],
                       return_data.args[2], return_data.args[3],
                       return_data.args[4], return_data.args[5]);

        if (item->m_once)
            delete_list.push_back(item);
    }
    m_firing = false;

    while (!delete_list.empty())
    {
        delete delete_list.back();
        delete_list.pop_back();
    }
}

void Connection::Disconnect()
{
    if (m_signal->m_firing)
        delete_list.push_back(this);
    else
        delete this;
}