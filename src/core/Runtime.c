#include <OS.h>
#include <stdio.h>
#include "../internal.h"

#include <utils/logging.h>

static void loop();
static int eventfilter(void *, SDL_Event *);

void __run_engine()
{
    SDL_AddEventWatch(eventfilter, NULL);
    //SDL_AddEventWatch(__signal_events, NULL);
    while (__poll_events())
        loop();
}

static void loop()
{
    ++engine.frame;
    engine.frameend_time = os.clock();
    engine.last_dt = (engine.frameend_time - engine.framestart_time) / (sre_timeStamp)CLOCK_FREQUENCY;
    engine.framestart_time = engine.frameend_time;

    __update_classes();
    __update_input();

    __destroy_queue();

    __query_objects();
    __update_world();
    __update_layer();

   __destroy_queue();
   
    __display_render();

#ifdef __unix__ // Update console content for unix (since it only does update for every new-line)
    fflush(stdout);
#endif

    sre_timeStamp elapsed;
    elapsed = (os.clock() - engine.framestart_time) / (sre_timeStamp)CLOCK_FREQUENCY;
    elapsed = engine.target_dt - elapsed;

    if (elapsed > 0)
        delay_s(elapsed);
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
                __update_viewport(ev->window.data1, ev->window.data2);
                break;
        }
    }

    return 1;
}