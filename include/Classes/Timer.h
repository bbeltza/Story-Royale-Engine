#pragma once
#include <standard.h>
#include "Classes/Signal.h"

class Timer
{
    static std::chrono::steady_clock s_global_clock;
    static std::vector<Timer*> *s_timers;

    public:
    Timer(delta_model duration, bool looped=false);
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
    delta_model m_Duration;
    delta_model m_timestamp;

    static delta_model global_update();
};