#pragma once
#include "Base/Thread.hpp"
#include "Base/Audio.hpp"
#include "Base/Texture.hpp"
#include "Base/Font.hpp"

struct AudioAccess
{
    template <typename... Args> AudioAccess(Args&&... args): data(std::forward<Args>(args)...) {}
    AudioData data;
};

struct _containers_service
{
    std::list<Thread::data> allocated_threads;
    std::queue<Thread::data*> finished_threads;

    std::unordered_set<Audio*> audio_queue;
    std::list<AudioAccess> loaded_audios;
	std::queue<Audio*> stopped_audios;

    std::list<Font*> loaded_fonts;

    _containers_service();
    ~_containers_service();

    inline void lock() { m_mutex.lock(); }
    inline void unlock() { m_mutex.unlock(); }
    inline bool trylock() { return m_mutex.try_lock(); }

    private:
    std::mutex m_mutex;
};

#define _containers static_cast<_containers_service*>(engine.containers_service)