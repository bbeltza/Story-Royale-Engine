#include "../internal.h"

#include "Engine.hpp"

#include "Base/Timer.hpp"
#include "Base/Tween.hpp"
#include "Base/Thread.hpp"

Signal<sre::timeStamp> Runtime::OnUpdate{ NULL };
Signal<> Runtime::BeforeRender{ NULL };
Signal<> Runtime::AfterRender{ NULL };

intptr_t Runtime::CurrentFrame() { return engine.frame; }

void Runtime::SetFramerate(unsigned short FPS) { engine.target_dt = FPS ? 1.0_ts / FPS : 0; }
void Runtime::SetPhysicsRate(unsigned short FPS) { assert(FPS != 0); engine.phys_target_dt = 1.0_ts / FPS; }

const sre::timeStamp& Runtime::delta_time = engine.last_dt;
const sre::timeStamp& Runtime::pdelta_time = engine.phys_target_dt;