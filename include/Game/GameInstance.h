#pragma once
#include "Datatypes/TimeStamp.h"

struct GameInstance
{
    virtual ~GameInstance() {}

    virtual void Update(TimeStamp dt) {}
    virtual void preRender() {}
    virtual void postRender() {}

    void Destroy();
};

struct WorldInstance: public GameInstance
{
    virtual void pUpdate(TimeStamp dt) {}
};