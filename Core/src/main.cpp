#ifdef _MSVC_LANG

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Setupapi.lib")
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "Imm32.lib")

#endif

#include <iostream>
#include "SDL.hpp"
#include "Engine.h"

int main()
{
    Engine::Init();
}