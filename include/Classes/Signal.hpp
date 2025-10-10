#pragma once
#include <unordered_set>

typedef void (*EventFunction)(void *signal_data, void *connection_data, ...);

#define event_callback(fn) (EventFunction) fn

struct Signal;
class Connection;

struct Signal
{
    friend class Connection;

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

    template <class... _Args> inline void Fire(_Args... args) {count_fire(sizeof...(args), args...);}
    void DisconnectAll();
    
    void* userdata;
    
    private:
    void* m_waitSem = create_sem();
    
    bool m_multithreaded;
    bool m_firing = false;
    
    void* create_sem();
    void count_fire(size_t count, ...);
    
    argbase* return_data = nullptr;
    std::unordered_set<Connection*> m_connections;
};

class Connection
{
    friend struct Signal;

    Connection(Signal *signal, EventFunction fn, void *data, bool once = false)
    : m_signal(signal),
      m_fn(fn),
      userdata(data),
      m_once(once)  {}
    Connection(const Connection &other) = delete;

public:
    void Disconnect();
    void* userdata;

private:
    friend struct Signal;
    ~Connection() { const_cast<Signal*>(m_signal)->m_connections.erase(this); };

    EventFunction m_fn;
    const Signal *m_signal;

    const bool m_once;
};