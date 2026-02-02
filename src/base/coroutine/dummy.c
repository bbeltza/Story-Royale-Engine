typedef void* coroutine_native;

static bool sys_coroutinecreate(coroutine_native* coroutine, const coroutine_data* data)
{
    return false;
}

static bool sys_coroutinepoolsetup(coroutine_native* pool)
{
    return false;
}

static void sys_coroutineswitch(const coroutine_native* coroutine)
{
    return;
}

static void sys_coroutineclose(coroutine_native* coroutine)
{

}