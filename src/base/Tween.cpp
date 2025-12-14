#include "Base/Tween.hpp"

#include "utils/math.h"

TweenBase::Set& TweenBase::get_tweens()
{
    static Set set;
    return set;
}

void TweenBase::global_update(sre::timeStamp delta)
{
    for (TweenBase* tween : get_tweens())
    {
        if (tween->m_Playing)
            tween->base_update(delta);
    }
}

TweenBase::TweenBase(const TweenInfo& info):
    Info(info)
{
    get_tweens().insert(this);
}

TweenBase::~TweenBase()
{
    get_tweens().erase(this);
}

void TweenBase::Play()
{
    m_Playing = true;
    start();
}

sre::timeStamp TweenBase::Cancel()
{
    m_Playing = false;

    float elapse = m_elapsed;
    m_elapsed = 0;
    return elapse;
}

sre::timeStamp TweenBase::Pause()
{
    m_Playing = false;

    return m_elapsed;
}

void TweenBase::base_update(sre::timeStamp delta)
{
    float alpha;
    m_elapsed += delta;
    if (m_elapsed >= Info.duration)
    {
        alpha = 1;
        m_elapsed = 0;
        m_Playing = false;
    }
    else
        alpha = GetAlpha();
    
    step(alpha);

    if (!m_Playing)
        Completed.Fire();
}