#ifdef _MSVC_LANG

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Setupapi.lib")
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "Imm32.lib")

#endif

#include <iostream>
#include "Engine.h"

int main()
{
    EngineClass e;
    Engine->Run();
}