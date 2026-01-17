#ifndef SRE_THREAD_H
#define SRE_THREAD_H
#include <C_API.h>
#include <ints.h>

#include <Datatypes/TimeStamp.h>

SRE_CAPI_BEGIN

#ifdef __cplusplus
    enum sre_Thread: sre_uptr { SRE_INVALIDTHREAD = 0 };
#else
    #define SRE_INVALIDTHREAD 0
    typedef sre_uptr sre_Thread;
#endif

// This is the function that holds the actual implementation for creating a thread
sre_Thread sre_threadcreate_delaystacksize(sre_sptr (*function)(void* data), void* data, sre_timeStamp delay, sre_usize stacksize);
inline sre_Thread sre_threadcreate_delay(sre_sptr (*function)(void* data), void* data, sre_timeStamp delay)
{ return sre_threadcreate_delaystacksize(function, data, delay, 0); }

inline sre_Thread sre_threadcreate(sre_sptr (*function)(void* data), void* data)
{ return sre_threadcreate_delay(function, data, 0); }
inline sre_Thread sre_threadcreate_stacksize(sre_sptr (*function)(void* data), void* data, sre_usize stacksize)
{ return sre_threadcreate_delaystacksize(function, data, 0, stacksize); }

//

// Wait for a thread to finish and get its return value
// @return The return value of the thread. The function may fail and return -1, use `succeeded` to check if the function hasn't failed
sre_sptr sre_threadjoin(sre_Thread thrd, int* succeeded);
void sre_threaddetach(sre_Thread thrd);

SRE_CAPI_END

#endif