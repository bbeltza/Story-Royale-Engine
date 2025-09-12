#include "Classes/Timer.h"

std::chrono::high_resolution_clock Timer::s_global_clock;
std::chrono::high_resolution_clock::time_point Timer::s_last_frame_time;
static std::chrono::duration<TimeStamp> helper_ts_duration;

Timer::Set& Timer::get_timers()
{
    static Set timers;
    return timers;
}

Timer::Timer(TimeStamp duration, bool looped): m_Duration(duration), Looped(looped)
{
    get_timers().insert(this);
};

Timer::~Timer()
{
    get_timers().erase(this);
}

void Timer::Start()
{
    m_playing = true;
}


void Timer::Stop()
{
    m_playing = false;
    m_timestamp = 0;
}

void Timer::Pause()
{
    m_playing = false;
}

void Timer::_hit()
{
    Hit.Fire(this);

    if (Looped)
    {
        m_timestamp -= m_Duration;
    }
    else
    {
        m_playing = false;
    }

}

TimeStamp Timer::global_update()
{
    helper_ts_duration = s_global_clock.now() - s_last_frame_time;
    s_last_frame_time = s_global_clock.now();
    
    TimeStamp delta = helper_ts_duration.count();

    for (auto timer : get_timers())
    {
        if (!timer->m_playing) continue;
        timer->m_timestamp += delta;

        if (timer->m_timestamp >= timer->m_Duration)
            timer->Hit_Single.Fire(nullptr);
        while (timer->m_timestamp >= timer->m_Duration)
            timer->_hit();
    }

    return delta;
}