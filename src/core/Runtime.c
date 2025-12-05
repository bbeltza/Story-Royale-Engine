#include <OS.h>
#include <stdio.h>
#include "../internal.h"

static void loop();
static int eventfilter(void*, SDL_Event *);

void __run_engine()
{
    SDL_SetEventFilter(eventfilter, NULL);
    while (__poll_events())
        loop();
}

static void loop()
{
    ++engine.frame;
    __update_classes();
    __update_input();
    
    __destroy_queue();

    __query_objects();
    
    __update_world();
    __update_layer();

    __display_render();

    #ifdef __unix__ // Update console content for unix (since it only does update for every new-line)
        fflush(stdout);
    #endif

    if (engine.target_dt)
        delay_s(engine.target_dt);
}

static int eventfilter(void* data, SDL_Event *ev)
{
    #ifdef _WIN32
    if (ev->type == SDL_WINDOWEVENT && ev->window.event == SDL_WINDOWEVENT_EXPOSED)
    {
        loop();
    }
    #endif

    return 1;
}