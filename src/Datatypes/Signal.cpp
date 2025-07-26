#include "Classes/Signal.h"

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


Connection* Signal::Connect(void(*fn)(void*))
{
    auto connetion = new Connection(this, fn);
    if (m_handlerListHead)
        connetion->m_next = m_handlerListHead;

    this->m_handlerListHead = connetion;

    return connetion;
}

void Signal::Fire(void* userdata)
{
    Connection* item = m_handlerListHead;
    while (item)
    {
        if (item->m_connected)
            item->m_fn(userdata);
        
        item = item->m_next;
    }
}