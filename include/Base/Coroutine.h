#ifndef SRE_COROUTINE_H
#define SRE_COROUTINE_H
#include <C_API.h>
#include <Datatypes/TimeStamp.h>

SRE_CAPI_BEGIN

typedef struct sre_coroutine sre_coroutine;
typedef void (*sre_coroutineFunction)(void* userdata);

// Create new coroutine handle
// @return Handle to the coroutine, or `NULL` if creation fails
sre_coroutine* sre_coroutinecreate(sre_coroutineFunction function, void* userdata);

bool sre_coroutineresume(sre_coroutine* coroutine);
bool sre_coroutinesuspend(sre_coroutine* coroutine);
bool sre_coroutineyield(sre_coroutine* coroutine, sre_timeStamp time);

// Cancel coroutine execution
void sre_coroutinecancel(sre_coroutine* coroutine);
// Close and free coroutine
void sre_coroutineclose(sre_coroutine* coroutine);

// Pointer to a coroutine that's `NULL`
// You don't have to use it, it's mostly used by the C++ API to still use the references to coroutines in the functions
extern const sre_coroutine* SRE_COROUTINE_NULL;

SRE_CAPI_END

#ifdef __cplusplus
#define __COROUTINE(x) *reinterpret_cast<sre_coroutine**>(&x)
#define __COROUTINE_NULL *reinterpret_cast<Coroutine*>(&SRE_COROUTINE_NULL)

namespace sre
{
    // RAII coroutine object (Only contains sre_coroutinecreate and sre_coroutineclose)
    class Coroutine
    {
        sre_coroutine* m_coroutine = NULL;
    public:
        ~Coroutine() { sre_coroutineclose(m_coroutine); }

        template <typename Fn>
        Coroutine(Fn&& fx, void* userdata);
    };

    bool coroutine_resume(Coroutine& coroutine = __COROUTINE_NULL) { return sre_coroutineresume(__COROUTINE(coroutine)); }
    bool coroutine_suspend(Coroutine& coroutine = __COROUTINE_NULL) { return sre_coroutinesuspend(__COROUTINE(coroutine)); }
    bool coroutine_yield(sre::timeStamp time, Coroutine& coroutine = __COROUTINE_NULL) { return sre_coroutineyield(__COROUTINE(coroutine), time); }
    
    void coroutine_cancel(Coroutine& coroutine = __COROUTINE_NULL) { return sre_coroutinecancel(__COROUTINE(coroutine)); }
}

#undef __COROUTINE_NULL
#undef __COROUTINE
#endif

#endif