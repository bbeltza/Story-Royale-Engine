// unistd stack manager
#include <unistd.h>

static bool sys_coroutinestack(void** base, size_t* size)
{
    static int PAGE_SIZE;
    if (!PAGE_SIZE)
        PAGE_SIZE = sysconf(_SC_PAGESIZE);

    const size_t stacksize = 4096*1024;
    *size = stacksize;

    if (posix_memalign(base, PAGE_SIZE, stacksize))
        return false;

    return true;
}

static void sys_coroutineclose(coroutine_native* coroutine) { free(coroutine->COROUTINE_STACKVAR); }