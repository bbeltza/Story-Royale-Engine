#pragma once
#include "datatypes/timestamp.h"
#include "Base/Signal.hpp"

struct GameInstance
{
    GameInstance() {}
    virtual ~GameInstance();

    virtual void Update(sre::timeStamp dt) {}
    virtual void preRender() {}
    virtual void postRender() {}

    Signal<sre::timeStamp> Updated{this};
    Signal<> Rendered{this};

    void Destroy();
};

struct WorldInstance: public GameInstance
{
    WorldInstance() {}
    virtual void pUpdate(sre::timeStamp dt) {}
    Signal<sre::timeStamp> pUpdated{this};
};