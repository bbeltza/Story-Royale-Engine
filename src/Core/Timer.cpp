#include "Classes/Timer.h"

std::vector<Timer*> *Timer::s_timers = nullptr;
std::chrono::steady_clock Timer::s_global_clock;
static std::chrono::duration<float> helper_float_duration;


Timer::Timer(float duration, bool looped): m_Duration(duration), Looped(looped)
{
    if (!s_timers) s_timers = new std::vector<Timer*>; // Heap allocating it because it automatically empties itself when stack allocating
    s_timers->push_back(this);
    printf("%p\n", &s_timers);
};

Timer::~Timer()
{
    for (auto it = s_timers->begin(); it != s_timers->end(); it++)
    {
            if (*it == this)
            {
                s_timers->erase(it);
                break;
            }
    }

    if (s_timers->empty())
    {
        delete s_timers;
        s_timers = nullptr;
    }
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

float Timer::global_update()
{
    static std::chrono::duration<float> last_frame_time = s_global_clock.now().time_since_epoch();
    helper_float_duration = s_global_clock.now().time_since_epoch() - last_frame_time;
    last_frame_time = s_global_clock.now().time_since_epoch();
    float delta = helper_float_duration.count();

    if (!s_timers) goto ret;
    for (auto timer : *s_timers)
    {
        if (!timer->m_playing) continue;
        timer->m_timestamp += delta;

        if (timer->m_timestamp >= timer->m_Duration)
            timer->Hit_Single.Fire(nullptr);
        while (timer->m_timestamp >= timer->m_Duration)
            timer->_hit();
    }

    ret:

    return delta;
}