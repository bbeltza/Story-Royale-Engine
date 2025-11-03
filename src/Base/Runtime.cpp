#include "../internal.h"

#include "Engine.hpp"

#include "Base/Timer.hpp"
#include "Base/Tween.hpp"
#include "Base/Thread.hpp"

Signal<TimeStamp> Runtime::OnUpdate{ NULL, false };
Signal<> Runtime::BeforeRender{ NULL, false };
Signal<> Runtime::AfterRender{ NULL, false };

intptr_t Runtime::CurrentFrame() { return engine.frame; }

void Runtime::SetFramerate(unsigned short FPS) { engine.target_ms = FPS ? 1000 / FPS : 0; }