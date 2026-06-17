#ifndef SRE_MUTEX_HPP
#define SRE_MUTEX_HPP

struct SDL_mutex;

namespace sre
{
    // RAII mutex class (wraps SDL_mutex*, at least right now)
    class Mutex
    {
        SDL_mutex* m_sdlhandle = nullptr;
    public:
        constexpr Mutex(SDL_mutex* sdlmutex): m_sdlhandle(sdlmutex) {}
        Mutex();
        ~Mutex();

        void lock();
        bool unlock();
        bool try_lock();
    };
}

#endif