#pragma once
#include <standard.h>
#include "datatypes/Signal.h"

class Timer
{
    static std::chrono::steady_clock s_global_clock;
    static std::vector<Timer*> *s_timers;

    public:
    Timer(float duration, bool looped=false);
    ~Timer();

    void Start();
    void Stop();
    void Pause();

    float getCurrentTimestamp() const {return m_timestamp;};

    bool Looped = false;

    Signal Hit_Single;
    Signal Hit;
    private:
    friend class EngineClass;
    void _hit();

    bool m_playing = false;
    float m_Duration;
    float m_timestamp;

    static float global_update();
};