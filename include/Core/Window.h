#ifndef SRE_WINDOW_H
#define SRE_WINDOW_H
#include <C_API.h>

// New Window API (future use)

SRE_CAPI_BEGIN
typedef struct sre_Window sre_Window;
sre_Window* sre_mainwindow(void);

SRE_CAPI_END

#ifdef __cplusplus

namespace sre
{
    class Window
    {
        Window() = delete;
        public:
            // C++ Window class technically has no declared members, it's just a reference to a C sre_Window pointer (aka, sre::main_window() and sre_mainwindow() return the exact same pointer)
            sre_Window* to_c() { return reinterpret_cast<sre_Window*>(this); }
            static Window* from_c(sre_Window* hwnd) { return reinterpret_cast<Window*>(hwnd); }
    };

    Window* main_window(void);
}

#endif

#endif