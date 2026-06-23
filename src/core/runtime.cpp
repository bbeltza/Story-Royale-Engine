#include "../internal.h"

#include <Core/Runtime.hpp>

sre::Signal<sre::empty_t> sre::onUpdate{ NULL };
sre::Signal<sre::empty_t> sre::beforeRender{ NULL };
sre::Signal<sre::empty_t> sre::afterRender{ NULL };

intptr_t sre::current_frame() { return engine.frame; }
unsigned sre::current_framerate() { return static_cast<unsigned>(1/engine.target_dt); };

void sre::set_framerate(unsigned FPS) { engine.target_dt = FPS ? 1.0_ts / FPS : 0.0_ts; }

const sre::timeStamp& sre::dt = engine.last_dt;