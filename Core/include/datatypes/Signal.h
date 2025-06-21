#pragma once

#define EVENT_CALLBACK(name, type, identifier, CODE) static void name(void* _userdata)\
{ type* identifier = (type*)_userdata; \
CODE \
}

struct Signal;
struct Connection;


struct Signal
{
    Signal() {}
    Signal(Signal& other) = delete;
    ~Signal();

    Connection* Connect(void(*fn)(void*));

    void Fire(void* userdata);

private:
    Connection* m_handlerListHead = nullptr;
};

struct Connection
{
    Connection(Signal* signal, void (*fn)(void*))
        :m_signal(signal), m_fn(fn), m_connected(1), m_next(nullptr)
    {
    }
    Connection(Connection& other) = delete;
    void Disconnect() { m_connected = 0; }

private:
    friend struct Signal;
    ~Connection() {}

    void (*m_fn)(void*);
    const Signal* m_signal;
    Connection* m_next;

    bool m_connected;
};