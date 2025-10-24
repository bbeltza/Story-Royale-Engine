#pragma once

#ifdef __cplusplus
#define init extern "C" void Initialize()
#else
#define init void Initialize();
#endif

init;

#ifdef __cplusplus
    namespace Game
    {
        // The method to be defined by the game using the engine
        // EVERY game should have this defined, even if it's empty
        // This is the entry point for the game, it's called after initializing the libraries, but before creating the window
        init;
    }
#endif