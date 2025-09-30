#pragma once

typedef void (*EventFunction)(void *signal_data, void *connection_data, ...);

#define event_callback(fn) (EventFunction) fn

struct Signal;
class Connection;

struct Signal
{
    struct argbase
    {
        void* args[6];
    };

    Signal(void* userdata, bool multithreaded=true): userdata(userdata), m_multithreaded(multithreaded) {}
    Signal(Signal &other) = delete;
    ~Signal();

    template <typename Func, typename T> inline Connection* Connect(Func fn, T userdata) { return Connect(event_callback(fn), (void*)userdata); }
    template <typename Func, typename T> inline Connection* Once(Func fn, T* userdata) { return Once(event_callback(fn), (void*)userdata); }

    Connection *Connect(EventFunction fn, void* userdata);
    Connection *Once(EventFunction fn, void* userdata);
    argbase Wait();

    template <typename _First, class... _Args> inline void Fire(_First first, _Args... args) {Fire(*(void**)&first, args...);}
    void Fire(void* first=nullptr, ...);
    void DisconnectAll();

    void* userdata;

private:
    Connection *m_handlerListHead = nullptr;
    void* m_waitSem = create_sem();

    bool m_multithreaded;

    void* create_sem();

    argbase return_data;
};

class Connection
{
    friend struct Signal;

    Connection(Signal *signal, EventFunction fn, void *data, bool once = false)
    : m_signal(signal),
      m_fn(fn),
      m_connected(1),
      m_next(nullptr),
      userdata(data),
      m_once(once)  {}
    Connection(Connection &other) = delete;

public:
    void Disconnect() { m_connected = 0; }
    void Reconnect() { m_connected = 1; }
    void* userdata;

private:
    friend struct Signal;
    ~Connection() {}

    EventFunction m_fn;
    const Signal *m_signal;
    Connection *m_next;

    bool m_connected;
    const bool m_once;
};