#pragma once

namespace GameSettings
{
    extern const char* Title;
    namespace StartResolution
    {
        extern unsigned int width;
        extern unsigned int height;
    }
    extern unsigned short targetFPS;


    void onSet();
}