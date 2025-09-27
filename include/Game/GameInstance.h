#pragma once
#include "Datatypes/TimeStamp.h"
#include "Classes/Signal.h"

struct GameInstance
{
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
    virtual void pUpdate(TimeStamp dt) {}
    Signal pUpdated;
};