#pragma once
#include "EngineBase.h"

#include "Classes/File.h"
#include "Classes/Signal.h"

#define AUDIO_BYTESIZE(x) (SDL_AUDIO_BITSIZE(x)/8)

class AudioData
{
    friend class AudioDevice;

    AudioData() = delete;
    AudioData(File&);

    int8_t* m_data = nullptr;
    uint32_t m_len;
    SDL_AudioSpec m_spec;

    bool m_loaded = false;

    const size_t f_size;
    const void* f_data;

    public:
    ~AudioData();

    Signal Loaded;

    inline uint32_t len() {return m_len;} 
    inline uint32_t freq() {return m_spec.freq;} 

    private:
    static void threaded_load(AudioData& self, SDL_RWops* audio_rw, File& file);
};

#include "Classes/Audio.h"

class AudioDevice
{
    ENGINE_BASE
    AudioDevice();
    ~AudioDevice();

    typedef std::unordered_map<std::string, std::unique_ptr<AudioData>> Map;
    typedef std::unordered_set<Audio*> QueueSet;
    typedef std::queue<Audio*> Queue;

    public:
    AudioData& LoadAudio(const char* path);
    void UnloadAudio(AudioData& audio);

    void PlayAudio(Audio& audio, bool force=false);
    TimeStamp PauseAudio(Audio& audio);
    TimeStamp StopAudio(Audio& audio) { auto t_pos = PauseAudio(audio); audio.m_fsamplepos = 0; return t_pos; }

    bool IsAudioPlaying(Audio& audio) {return audio_queue.find(&audio) != audio_queue.end();}

    private:
    static void callback(AudioDevice* dev, int32_t* stream, int len);
    static void threadedload(AudioDevice* dev, const char* path);
    
    SDL_AudioSpec m_Spec;
    SDL_AudioDeviceID m_Id;

    Map loaded_audios;
    QueueSet audio_queue;
};
