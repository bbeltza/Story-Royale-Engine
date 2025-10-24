#pragma once
#include "Engine.hpp"
#include "Datatypes/TimeStamp.h"

enum EasingStyle
{
    Es_Linear,
};
enum EasingDirection
{
    Ed_InOut,
    Ed_In,
    Ed_Out
};

struct TweenInfo
{
    float duration = 1;
    EasingStyle easingStyle = Es_Linear;
    EasingDirection esingDirection = Ed_InOut;

    float delay = 0;
    int repeat = 1;
    bool reverse = false;
};

extern "C" void __update_classes();

class TweenBase
{
    template <class T>
    friend class Tween;
    friend void __update_classes();

    TweenBase(const TweenInfo& info);
public:
    ~TweenBase();

    void Play();
    TimeStamp Cancel();
    TimeStamp Pause();

    Signal<> Completed;
    const TweenInfo& Info;

    inline float GetAlpha() const {return Info.duration ? m_elapsed/Info.duration : 1;}
private:

    TimeStamp m_elapsed = 0;
    bool m_Playing = false;

    void base_update(TimeStamp delta);
    virtual void step(float alpha) = 0;
    virtual void start() = 0;

    static void global_update(TimeStamp delta);

    typedef std::unordered_set<TweenBase*> Set;
    static Set& get_tweens();
};

template <class T>
class Tween: public TweenBase
{
    const T* m_src;
    T* m_target = nullptr;
    T m_start;

    inline void step(float alpha) override { *m_target = (T)ut_lerp(m_start, *m_src, alpha); }
    inline void start() override { if (m_target) m_start = *m_target; }
public:
    Tween(const TweenInfo& info, T& target, const T& source): TweenBase(info), m_target(&target), m_src(&source) {}
    Tween(const TweenInfo& info, const T& source): TweenBase(info), m_src(&source) {}
    void setTarget(T& target) { m_target = &target; }
};

