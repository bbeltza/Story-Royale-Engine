#include <OS.h>
#include <stdio.h>
#include "../internal.h"

static void loop();
static int eventfilter(void *, SDL_Event *);

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

    /*
    __destroy_queue();
    
    __query_objects();
    
    */
    __update_world();
   __update_layer();
   
    __display_render();

#ifdef __unix__ // Update console content for unix (since it only does update for every new-line)
    fflush(stdout);
#endif

    if (engine.target_dt)
        delay_s(engine.target_dt);
}

static int eventfilter(void *data, SDL_Event *ev)
{
    if (ev->type == SDL_WINDOWEVENT)
    {
        switch (ev->window.event)
        {
        #ifdef _WIN32
            case SDL_WINDOWEVENT_EXPOSED:
                loop();
                break;
        #endif
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                __update_viewport();
                break;
        }
    }

    return 1;
}