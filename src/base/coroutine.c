#include <Base/Coroutine.h>
#include <utils/mem.h>

#if defined(_WIN32)
    #include <Windows.h>
    typedef LPVOID coroutine_native;
#else
    typedef void* coroutine_native;
#endif

struct sre_coroutine
{
    sre_coroutine* next;
    sre_coroutineState state;

    coroutine_native native;
};

sre_coroutine* sre_coroutinecreate(bool suspended, sre_coroutineFunction function, void* userdata)
{
    sre_coroutine* coroutine = sre_new(sizeof(sre_coroutine));
    memset(coroutine, 0, sizeof(coroutine));

    coroutine->native = CreateFiber(0, (LPFIBER_START_ROUTINE)function, (LPVOID)userdata);
    return NULL;
}

bool sre_coroutineresume(sre_coroutine* coroutine)
{
    return false;
}
bool sre_coroutinesuspend(sre_coroutine* coroutine)
{
    return false;
}
bool sre_coroutineyield(sre_coroutine* coroutine, sre_timeStamp time)
{
    return false;
}

sre_coroutineState sre_coroutinestate(const sre_coroutine* coroutine)
{
    return SRE_COROUTINESTATE_INVALID;
}

void sre_coroutinecancel(sre_coroutine* coroutine)
{

}

void sre_coroutineclose(sre_coroutine* coroutine)
{

}