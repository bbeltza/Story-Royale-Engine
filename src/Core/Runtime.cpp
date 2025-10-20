#include "../internal.h"

#include "Engine.hpp"

#include "Classes/Timer.hpp"
#include "Classes/Tween.hpp"

Signal<TimeStamp> Runtime::OnUpdate{ NULL, false };
Signal<> Runtime::BeforeRender{ NULL, false };
Signal<> Runtime::AfterRender{ NULL, false };

intptr_t Runtime::CurrentFrame() { return engine.frame; }

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

    // __query_objects();
    
    __update_layer();

    __display_render();
}

static void __update_classes()
{
    engine.last_dt = Timer::global_update();
    TweenBase::global_update(engine.last_dt);
}