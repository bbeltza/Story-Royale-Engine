#pragma once
#include "Datatypes/TimeStamp.h"
#include "Classes/Signal.h"

struct GameInstance
{
    GameInstance(): Updated(this), Rendered(this) {}
    virtual ~GameInstance();

    virtual void Update(TimeStamp dt) {}
    virtual void preRender() {}
    virtual void postRender() {}

    Signal Updated;
    Signal Rendered;

    void Destroy();
};

struct WorldInstance: public GameInstance
{
    WorldInstance(): pUpdated(this) {}
    virtual void pUpdate(TimeStamp dt) {}
    Signal pUpdated;
};