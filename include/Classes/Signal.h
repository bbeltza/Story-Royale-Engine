#pragma once

typedef void (*EventFunction)(void *);

#define event_callback(fn) (EventFunction) fn

struct Signal;
class Connection;

struct Signal
{
    Signal(bool multithreaded=true): m_multithreaded(multithreaded) {}
    Signal(Signal &other) = delete;
    ~Signal();

    Connection *Connect(EventFunction fn);
    Connection *Once(EventFunction fn);
    void* Wait();

    inline void Fire() { return Fire(nullptr); }
    void Fire(void *userdata);
    void DisconnectAll();

private:
    Connection *m_handlerListHead = nullptr;
    void* m_waitSem = create_sem();
    void* m_returnData = nullptr;

    bool m_multithreaded;

    void* create_sem();
};

class Connection
{
    friend struct Signal;

    Connection(Signal *signal, EventFunction fn, void *self, bool once = false)
    : m_signal(signal),
      m_fn(fn),
      m_connected(1),
      m_next(nullptr),
      m_class(self),
      m_once(once)  {}
    Connection(Signal *signal, EventFunction fn, bool once = false) : Connection(signal, fn, nullptr, once) {}
    Connection(Connection &other) = delete;

public:
    void Disconnect() { m_connected = 0; }
    void Reconnect() { m_connected = 1; }

private:
    friend struct Signal;
    ~Connection() {}

    EventFunction m_fn;
    void *m_class;
    const Signal *m_signal;
    Connection *m_next;

    bool m_connected;
    const bool m_once;
};