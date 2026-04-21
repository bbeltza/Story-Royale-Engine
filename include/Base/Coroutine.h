#ifndef SRE_COROUTINE_H
#define SRE_COROUTINE_H
#include <stdbool.h>
#include <stddef.h>

#include <C_API.h>
#include <Datatypes/TimeStamp.h>

SRE_CAPI_BEGIN

typedef struct sre_coroutine sre_coroutine;
typedef void (*sre_coroutineFunction)(void* userdata);

typedef enum
{
    SRE_COROUTINESTATE_INVALID,
    SRE_COROUTINESTATE_RUNNING,
    SRE_COROUTINESTATE_SUSPENDED,
    SRE_COROUTINESTATE_CANCELLED
    // There might be `FINISHED` too
} sre_coroutineState;

// Will initialize and run coroutine context in the calling thread
void _coroutine_coreinit(void* running);

// Create new coroutine handle
// @return Handle to the coroutine, or `NULL` if creation fails
sre_coroutine* sre_coroutinecreate(bool suspended, sre_coroutineFunction function, void* userdata);
// Get the current coroutine, or `NULL` if the calling thread doesn't come from a coroutine
sre_coroutine* sre_coroutinecurrent();

bool sre_coroutineresume(sre_coroutine* coroutine, void* data);

// Note that these 2 functions don't yield if the calling coroutine is in a `CANCELLED` state
bool sre_coroutinesuspend(); // Suspend calling coroutine
bool sre_coroutineyield(sre_timeStamp delay); // Yield current coroutine for `time` seconds

// Suspend the current coroutine and retrieve data from it
bool sre_coroutinesuspendEx(void** data);

sre_coroutineState sre_coroutinestate(const sre_coroutine* coroutine);
bool sre_coroutinerunning(); // Check if current coroutine is running

// Set the state of the coroutine to `CANCELLED` and let the coroutine cleanup
// Nothing will ever yield the coroutine and the thread running the coroutine will block until the coroutine has finished
// You may use some upcoming helper macros to avoid doing some operations if the coroutine is cancelled
void sre_coroutinecancel(sre_coroutine* coroutine);

SRE_CAPI_END

#ifdef __cplusplus

namespace sre
{
    enum coroutineState
    {
        COROUTINESTATE_INVALID = SRE_COROUTINESTATE_INVALID,
        COROUTINESTATE_RUNNING = SRE_COROUTINESTATE_RUNNING,
        COROUTINESTATE_SUSPENDED = SRE_COROUTINESTATE_SUSPENDED,
        COROUTINESTATE_CANCELLED = SRE_COROUTINESTATE_CANCELLED
    };

    // RAII coroutine object
    class Coroutine
    {
        sre_coroutine* m_coroutine = NULL;
    public:
        constexpr Coroutine() = default;
        ~Coroutine() { cancel(); }

        template <typename Ret, typename Ptr>
        using TFn = Ret (Ptr* data);

        template <typename Ret, typename Ptr>
        Coroutine(bool suspended, TFn<Ret, Ptr> fx, void* userdata):
            m_coroutine(sre_coroutinecreate(suspended, reinterpret_cast<sre_coroutineFunction>(fx), userdata))
        {}

        Coroutine(bool suspended, void fx(void* data), void* userdata):
            m_coroutine(sre_coroutinecreate(suspended, fx, userdata))
        {}

        Coroutine(Coroutine&& moving): m_coroutine(moving.m_coroutine) {
            moving.m_coroutine = NULL;
        }

        void operator =(Coroutine&& moving) {
            m_coroutine = moving.m_coroutine;
            moving.m_coroutine = NULL;
        }
    public:
        bool resume(void* data=NULL) { return sre_coroutineresume(m_coroutine, data); }
        void cancel() { sre_coroutinecancel(m_coroutine); m_coroutine = NULL; }
        coroutineState state() const { return static_cast<coroutineState>(sre_coroutinestate(m_coroutine)); }
    };

    inline bool coroutine_running() { return sre_coroutinerunning(); }
    inline bool coroutine_suspend() { return sre_coroutinesuspend(); }
    inline bool coroutine_yield(sre::timeStamp time) { return sre_coroutineyield(time); }
}

#endif

#endif