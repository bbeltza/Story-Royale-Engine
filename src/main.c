#define SDL_MAIN_HANDLED
#include "internal.h"

struct _engine_data __engine_data;

int main()
{
    __initialize_engine();
    __run_engine();
    __end_engine();
}

#ifdef _WIN32
    #include <Windows.h>

    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
    {
        return main();
    }
#endif