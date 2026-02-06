#include "../internal.h"

#include <Core/Runtime.hpp>
#include <Core/Thread.hpp>

sre::Signal<sre::empty_t> sre::onUpdate{ NULL };
sre::Signal<sre::empty_t> sre::beforeRender{ NULL };
sre::Signal<sre::empty_t> sre::afterRender{ NULL };

intptr_t sre::current_frame() { return engine.frame; }
unsigned current_framerate() { return static_cast<unsigned>(1/engine.target_dt); };
unsigned current_tickrate() { return static_cast<unsigned>(1/engine.phys_target_dt); }

void sre::set_framerate(unsigned FPS) { engine.target_dt = FPS ? 1.0_ts / FPS : 0.0_ts; }
void sre::set_tickrate(unsigned FPS) { assert(FPS != 0); engine.phys_target_dt = 1.0_ts / FPS; }

const sre::timeStamp& sre::dt = engine.last_dt;
const sre::timeStamp& sre::pdt = engine.phys_target_dt;