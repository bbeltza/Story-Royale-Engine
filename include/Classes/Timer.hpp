#pragma once
#include <standard>
#include "Classes/Signal.hpp"

#include "Datatypes/TimeStamp.h"

class Timer
{
    typedef std::unordered_set<Timer*> Set;

    static std::chrono::steady_clock s_global_clock;
    static std::chrono::steady_clock::time_point s_last_frame_time;
    static Set& get_timers();

    public:
    Timer(TimeStamp duration, bool looped=false);
    ~Timer();

    void Start();
    void Stop();
    void Pause();

    TimeStamp getCurrentTimestamp() const {return m_timestamp;};

    bool Looped = false;

    Signal Hit_Single;
    Signal Hit;
    private:
    friend class EngineClass;
    void _hit();

    bool m_playing = false;
    TimeStamp m_Duration;
    TimeStamp m_timestamp;

    static TimeStamp global_update();
};
