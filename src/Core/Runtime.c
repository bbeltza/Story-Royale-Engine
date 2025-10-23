#include "../internal.h"

static void loop();

void __run_engine()
{
    while (__poll_events())
        loop();
}

static void loop()
{
    ++engine.frame;
    __update_classes();
    __update_viewport();
    __update_input();
    
    __destroy_queue();

    __query_objects();
    
    __update_world();
    __update_layer();

    __display_render();

    if (engine.target_ms) SDL_Delay(engine.target_ms);
}