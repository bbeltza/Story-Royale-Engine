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

struct _containers_service: private std::mutex
{
    std::list<Thread::data> allocated_threads;
    std::queue<Thread::data*> finished_threads;

    std::unordered_set<Audio*> audio_queue;
    std::list<AudioAccess> loaded_audios;
	std::queue<Audio*> stopped_audios;

    _containers_service();
    ~_containers_service();

    using mutex::lock;
    using mutex::unlock;
    using mutex::try_lock;
};

#define _containers static_cast<_containers_service*>(engine.containers_service)