#ifndef SRE_DEFER_H
#define SRE_DEFER_H
#include <C_API.h>

SRE_CAPI_BEGIN

typedef void (*sre_deferFunction)(void* data);

// Queue a function to the main thread for the next runtime execution point
// This is pretty useful for multithreading, since some features aren't fully supported on multiple threads
// (Like creating a texture on linux), or to solve synchronisation issues
// However, do not use any yielding functions like os.delay (delay_s) because othewise it would block the entire game
void sre_defer(sre_deferFunction func, void* userdata);

SRE_CAPI_END

#endif