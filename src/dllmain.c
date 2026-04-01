// The engine's dynamic library source file (contains the entry point and a few helpers)

#if _WIN32
    #include <Windows.h>
    BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
    {
        // Do nothing... I don't care now
        return TRUE;
    }
#else
    __attribute__((constructor)) int _dlmain()
    {

    }
#endif