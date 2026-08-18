#ifndef SRE_ENTRY_H
#define SRE_ENTRY_H
#include <C_API.h>

SRE_CAPI_BEGIN
    #define sre_initialize initialize
    extern void sre_initialize(void);
SRE_CAPI_END

#ifdef __cplusplus
    namespace sre
    {
        // The method to be defined by the game
        // EVERY game should have this defined, even if it's empty
        // This is the entry point for the game, it's called after initializing the libraries, but before showing the window
        extern "C" void initialize(void);
    }
#endif

#endif
