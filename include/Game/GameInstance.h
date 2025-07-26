#pragma once

struct GameInstance
{
    virtual ~GameInstance() {}

    virtual void Update(float dt) {}
    virtual void preRender() {}
    virtual void postRender() {}

    void Destroy();
};

struct WorldInstance: public GameInstance
{
    virtual void pUpdate(float dt) {}
};