#pragma once

#ifndef __cplusplus
    #define sre_initialize initialize
    extern void sre_initialize();
#else
    namespace sre
    {
        // The method to be defined by the game
        // EVERY game should have this defined, even if it's empty
        // This is the entry point for the game, it's called after initializing the libraries, but before showing the window
        extern "C" void initialize();
    }

    namespace Game
    {
        
    }
#endif