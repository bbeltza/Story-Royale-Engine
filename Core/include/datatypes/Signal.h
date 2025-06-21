#pragma once

#define EVENT_CALLBACK_DECLARE(name) void name(void* _userdata)
#define EVENT_CALLBACK(name, type, identifier, ...) void name(void* _userdata)\
{ type* identifier = (type*)_userdata; \
__VA_ARGS__ \
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
    Connection(Signal* signal, void (*fn)(void*), void* self):
        m_signal(signal),
        m_fn(fn),
        m_connected(1),
        m_next(nullptr),
        m_class(self)
    {}
    Connection(Signal* signal, void (*fn)(void*)) :
        Connection(signal, fn, nullptr)
    {}
    Connection(Connection& other) = delete;
    void Disconnect() { m_connected = 0; }

private:
    friend struct Signal;
    ~Connection() {}

    void (*m_fn)(void*);
    void* m_class;
    const Signal* m_signal;
    Connection* m_next;

    bool m_connected;
};