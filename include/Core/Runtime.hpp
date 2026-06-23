#ifndef SRE_RUNTIME_HPP
#define SRE_RUNTIME_HPP
#include <Core/Event.hpp>

#include <Datatypes/TimeStamp.h>

namespace sre
{
    extern sre::Signal<sre::empty_t> onUpdate;
    extern sre::Signal<sre::empty_t> beforeRender;
    extern sre::Signal<sre::empty_t> afterRender;

    intptr_t current_frame();
    unsigned current_framerate();

    void set_framerate(unsigned fps);

    extern const sre::timeStamp& dt;
}

#endif