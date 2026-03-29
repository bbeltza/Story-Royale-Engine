static bool sys_coroutinestack(void** base, size_t* size)
{
    const size_t stacksize = 4096*1024;
    *size = stacksize;

    if (posix_memalign(base, PAGE_SIZE, stacksize))
        return false;

    return true;
}

static void sys_coroutineclose(coroutine_native* coroutine) { free(coroutine->COROUTINE_STACKVAR); }