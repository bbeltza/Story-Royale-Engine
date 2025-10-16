#pragma once
#include "Datatypes/TimeStamp.h"
#include "Classes/Signal.hpp"

struct GameInstance
{
    GameInstance() {}
    virtual ~GameInstance();

    virtual void Update(TimeStamp dt) {}
    virtual void preRender() {}
    virtual void postRender() {}

    Signal Updated{this};
    Signal Rendered{this};

    void Destroy();
};

struct WorldInstance: public GameInstance
{
    WorldInstance() {}
    virtual void pUpdate(TimeStamp dt) {}
    Signal pUpdated{this};
};