#pragma once
#include "datatypes/timestamp.h"
#include "Base/Signal.hpp"

struct GameInstance
{
    GameInstance() {}
    virtual ~GameInstance();

    virtual void Update(TimeStamp dt) {}
    virtual void preRender() {}
    virtual void postRender() {}

    Signal<TimeStamp> Updated{this};
    Signal<> Rendered{this};

    void Destroy();
};

struct WorldInstance: public GameInstance
{
    WorldInstance() {}
    virtual void pUpdate(TimeStamp dt) {}
    Signal<TimeStamp> pUpdated{this};
};