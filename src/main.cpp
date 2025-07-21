#include <iostream>
#include <setjmp.h>

#include <SDL.hpp>
#include <fmt/format.h>

#include "GameSettings.h"
#include "Engine.h"
#include "System.h"

jmp_buf System::err;
std::string System::last_err;

static SDL_MessageBoxButtonData err_msg_button{
    SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT,
    0,
    "Ok"
};

static SDL_MessageBoxColorScheme err_msg_color_scheme;

static SDL_MessageBoxData err_msg_data{
    //Flags
        SDL_MESSAGEBOX_ERROR,
    //Window
        NULL,
    //Title & Message
        0, "\0",
    //Number of buttons
        1,
    // Data pointers
        &err_msg_button,
        &err_msg_color_scheme
};


int main()
{
    int result = setjmp(System::err);
    if (result)
    {
        err_msg_data.title = GameSettings::Title;
        //if (Engine && Engine->Window.sdl_window)
            //err_msg_data.window = Engine->Window.sdl_window;
        
        err_msg_data.message = System::last_err.c_str();

        System::Log("\7{}\n", err_msg_data.message);
        SDL_ShowMessageBox(&err_msg_data, NULL);
    }
    else
    {
        EngineClass e;
        Engine->Run();
    }

    return result;
}

#ifdef _WIN32
    #include <Windows.h>

    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
    {
        return main();
    }
#endif