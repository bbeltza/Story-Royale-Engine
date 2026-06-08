#ifndef SRE_WINDOW_H
#define SRE_WINDOW_H
#include <C_API.h>
#include <stdbool.h>

// New Window API (future use)

SRE_CAPI_BEGIN
typedef struct sre_Window sre_Window;
sre_Window* sre_mainwindow(void);

enum sreWindowCmd
{
    SRE_WINDOWCMD_UNKNOWN,
    
    SRE_WINDOWCMD_SETPOS, // int x, x
    SRE_WINDOWCMD_SETSIZE, // int w, h
    SRE_WINDOWCMD_SETFULLSCREEN, // bool fullscreen
    SRE_WINDOWCMD_SETTITLE, // const char title[]

    SRE_WINDOWCMD_MINIMIZE,
    SRE_WINDOWCMD_MAXIMIZE,

    SRE_WINDOWCMD_HIDE,
    SRE_WINDOWCMD_SHOW, // bool focus

    SRE_WINDOWCMD_FLASH // enum sreWindowFlash action
};

enum sreWindowFlash
{
    SRE_WINDOWFLASH_CONSTANT,
    SRE_WINDOWFLASH_QUICK,
    SRE_WINDOWFLASH_CANCEL
};

bool sre_defer_windowcommand(sre_Window* window, enum sreWindowCmd cmd,  ...);

#define sre_window_set_pos(window, w, h) sre_defer_windowcommand(window, SRE_WINDOWCMD_SETSIZE, w, h)
#define sre_window_set_size(window, x, y) sre_defer_windowcommand(window, SRE_WINDOWCMD_SETSIZE, x, y)
#define sre_window_set_fullscreen(window, fullscreen) sre_defer_windowcommand(window, SRE_WINDOWCMD_SETFULLSCREEN, fullscreen)
#define sre_window_set_title(window, title) sre_defer_windowcommand(window, SRE_WINDOWCMD_SETTITLE, title)

#define sre_window_maximize(window) sre_defer_windowcommand(window, SRE_WINDOWCMD_MAXIMIZE)
#define sre_window_minimize(window) sre_defer_windowcommand(window, SRE_WINDOWCMD_MINIMIZE)

#define sre_window_hide(window) sre_defer_windowcommand(window, SRE_WINDOWCMD_HIDE)
#define sre_window_show(window, focus) sre_defer_windowcommand(window, SRE_WINDOWCMD_SHOW, focus)

#define sre_window_flash(window, action) sre_defer_windowcommand(window, SRE_WINDOWCMD_FLASH, action)

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