#include "../internal.h"

#include <Base/Runtime.hpp>
#include "Base/Thread.hpp"

Signal<> sre::onUpdate{ NULL };
Signal<> sre::beforeRender{ NULL };
Signal<> sre::afterRender{ NULL };

intptr_t sre::current_frame() { return engine.frame; }

void sre::set_framerate(unsigned FPS) { engine.target_dt = FPS ? 1.0_ts / FPS : 0.0_ts; }
void sre::set_tickrate(unsigned FPS) { assert(FPS != 0); engine.phys_target_dt = 1.0_ts / FPS; }

const sre::timeStamp& sre::dt = engine.last_dt;
const sre::timeStamp& sre::pdt = engine.phys_target_dt;