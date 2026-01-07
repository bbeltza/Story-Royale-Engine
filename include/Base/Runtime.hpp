#ifndef SRE_RUNTIME_HPP
#define SRE_RUNTIME_HPP
#include <Base/Signal.hpp>
#include <Base/Event.hpp>

#include <datatypes/timestamp.h>

namespace sre
{
    using ::Signal;

    extern sre::Signal<sre::Event> onEvent;
    extern sre::Signal<> onUpdate;
    extern sre::Signal<> beforeRender;
    extern sre::Signal<> afterRender;

    intptr_t current_frame();
    unsigned current_framerate();
    unsigned current_tickrate();

    void set_framerate(unsigned fps);
    void set_tickrate(unsigned tps);

    extern const sre::timeStamp& dt;
    extern const sre::timeStamp& pdt;
}

#endif