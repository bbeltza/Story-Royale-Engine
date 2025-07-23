#pragma once

typedef void (*EventFunction)(void*);

#define event_callback(fn) (EventFunction)fn

struct Signal;
struct Connection;


struct Signal
{
    Signal() {}
    Signal(Signal& other) = delete;
    ~Signal();

    Connection* Connect(EventFunction fn);
    Connection* Once(EventFunction fn);

    void Fire(void* userdata);
    void DisconnectAll();

private:
    Connection* m_handlerListHead = nullptr;
};

struct Connection
{
    Connection(Signal* signal, EventFunction fn, void* self):
        m_signal(signal),
        m_fn(fn),
        m_connected(1),
        m_next(nullptr),
        m_class(self)
    {}
    Connection(Signal* signal, EventFunction fn) :
        Connection(signal, fn, nullptr)
    {}
    Connection(Connection& other) = delete;
    void Disconnect() { m_connected = 0; }
    void Reconnect() { m_connected = 1; }

private:
    friend struct Signal;
    ~Connection() {}

    EventFunction m_fn;
    void* m_class;
    const Signal* m_signal;
    Connection* m_next;

    bool m_connected;
};