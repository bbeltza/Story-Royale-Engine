#include <Base/Signal.h>
#include <Base/Coroutine.h>

#include <utils/mem.h>

#include <SDL_mutex.h>
#include <SDL_atomic.h>

#include <assert.h>

struct sre_Signal
{
    sre_Connection* connection_head;
    void* userdata;

    sre_coroutine** coroutines;
    // If you're asking why the size and capacity are 32 bit integers and not "size" integers
    // We AIN'T having more than 2^32 coroutines, nor even 65536 are we? so I could make this be 16 bit integers
    // But at this point using 16 bit register instructions is considered heavier and thus slower

    sre_u32 coroutines_capacity;
    sre_u32 coroutines_size;
};

struct sre_Connection
{
    sre_Connection* next;
    sre_Connection* prev;
    sre_Signal* signal;

    sre_signalfunction function;

    sre_u32 flags;
    SDL_atomic_t reference;
};

enum sre_ConnectionFlags
{
    SRE_CONNECTION_NONE,
    SRE_CONNECTION_HASDATA,
    SRE_CONNECTION_ONCE
};

sre_Signal* sre_signalcreate(void* userdata)
{
    sre_Signal* signal = sre_new(sizeof(sre_Signal));

    signal->connection_head = NULL;
    signal->userdata = userdata;

    signal->coroutines_capacity = 16;
    signal->coroutines_size = 0;
    signal->coroutines = sre_newclear(sizeof(signal->coroutines) * 16);

    return signal;
}

void sre_signaldestroy(sre_Signal* signal)
{
    if (!signal) return;

    while (signal->connection_head)
    {
        sre_Connection* current = signal->connection_head;
        signal->connection_head = current->next;
        
        if (!SDL_AtomicGet(&current->reference))
            sre_delete(current);
        else
        {
            current->next = NULL;
            current->prev = NULL;
            current->signal = NULL;
        }
    }

    sre_delete(signal->coroutines);
    sre_delete(signal);
}

sre_Connection* sre_signalconnectEx(sre_Signal* signal, sre_signalfunction function, size_t extra_size, void** extra_data)
{
    if (!signal || !function) return NULL;
    
    sre_Connection* connection = sre_new(sizeof(sre_Connection) + extra_size);
    connection->signal = signal;
    connection->next = signal->connection_head;
    connection->prev = NULL;
    connection->reference.value = 0;

    if (signal->connection_head)
        signal->connection_head->prev = connection;
    signal->connection_head = connection;

    connection->flags = 0;
    connection->function = function;

    if (extra_size && extra_data)
    {
        *extra_data = connection + 1;
        connection->flags = 1;
    }

    return connection;
}

void sre_signaldisconnect(sre_Connection* connection)
{
    if (!connection) return;
    if (!connection->signal) return;

    if (connection->next) connection->next->prev = connection->prev;
    if (connection->prev) connection->prev->next = connection->next;
    else {
        assert(connection == connection->signal->connection_head);
        connection->signal->connection_head = connection->next;
    }

    connection->signal = NULL;
    if (connection->reference.value <= 0) sre_delete(connection);
}

sre_Connection* sre_signalaquire(sre_Connection* connection)
{
    assert(connection != NULL);

    SDL_AtomicAdd(&connection->reference, 1);
    return connection;
}

void sre_signalunaquire(sre_Connection* connection)
{
    if (!connection) return;
    if (SDL_AtomicAdd(&connection->reference, -1) <= 1 && !connection->signal)
        sre_delete(connection);
}

void* sre_signalwait(sre_Signal* signal)
{
    if (signal->coroutines_capacity <= signal->coroutines_size)
    {
        sre_u32 new_capacity = signal->coroutines_capacity * 2;
        sre_coroutine** new_block = sre_new(sizeof(sre_coroutine*) * new_capacity);
        memcpy(new_block, signal->coroutines, sizeof(sre_coroutine*) * signal->coroutines_capacity);
        
        sre_delete(signal->coroutines);
        signal->coroutines = new_block;
        signal->coroutines_capacity = new_capacity;
    }
    return sre_coroutinesuspendEx(&signal->coroutines[signal->coroutines_size++]);
}

bool sre_signalfire(sre_Signal* signal, void* data)
{
    if (!signal) return false;

    while (signal->coroutines_size)
        sre_coroutineresume(signal->coroutines[--signal->coroutines_size], data);
    
    for (sre_Connection* connection = signal->connection_head; connection != NULL; connection = connection->next)
        connection->function(signal->userdata, connection->flags & SRE_CONNECTION_HASDATA ? (void**)(connection + 1) : NULL, data);

    return true;
}

//

struct sre_signalconnectwrapper
{
    sre_signalfunction func;
    void* userdata;
};

void sre_signalconnectwrapper(void* signal_data, void* connection_data, void* fire_data)
{
    struct sre_signalconnectwrapper* wrapper = connection_data;
    wrapper->func(signal_data, wrapper->userdata, fire_data);
}

sre_Connection* sre_signalconnect(sre_Signal* signal, void* userdata, sre_signalfunction function)
{
    struct sre_signalconnectwrapper* data;
    sre_Connection* connection = sre_signalconnectEx(signal, sre_signalconnectwrapper, sizeof(struct sre_signalconnectwrapper), (void**)&data);
    if (!connection) return NULL;

    data->func = function;
    data->userdata = userdata;

    return connection;
}