#include <iostream>
#include <setjmp.h>

#include <SDL.hpp>
#include <fmt/format.h>

#include "GameSettings.hpp"
#include "Engine.hpp"
#include "Sys.h"

int main()
{
    EngineClass e;
    e.Run();

    return syscode();
}

#ifdef _WIN32
    #include <Windows.h>

    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
    {
        return main();
    }
#endif