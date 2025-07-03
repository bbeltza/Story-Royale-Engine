#pragma once

#include <fmt/format.h>
#include "SDL.hpp"

#include "Window.h"
#include "Input.h"

class EngineClass
{
public:


    // Engine namespaces
    WindowClass Window;
    InputClass Input;
private:
    // Friend int main so that the entry point runs void Run()
    friend int main();

    // Constructors
    EngineClass();
    ~EngineClass();

    // Private use functions
    void Run();

    bool pollWindowEvents();

    // Private members
    bool m_wasRun = 0;
};

extern EngineClass* Engine;

// TODO:
/*

!!: Done!
!: Working on!


- Window running; !!
- Window updating/filling the background with a color; !!
- Instancing/Scenes; !!
- Collision detection; !!
- Linking with ImGui (maybe before uhhh); *maybe after
- Rendering instances/entities; !!
- Input handling; !!
- Components; !!

- User interface layers; !

- Rendering text; !
- Rendering textures;

- Audio;

- Work on the game!;

*/