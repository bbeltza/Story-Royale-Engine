#include <SDL.hpp>
#include "System.h"
#include "Base/AudioDevice.h"

#include "utils.h"

AudioData::AudioData(File& file)
{
    const void* f_data = file.getRawData();
    size_t f_size = file.getSize();

    if (!f_size) {/*Will do something*/}

    SDL_RWops* audio_rw = SDL_RWFromConstMem(f_data, f_size);

    SDL_LoadWAV_RW(audio_rw, 1, &m_spec, (Uint8**)&m_data, &m_len);

    System::CheckForSDLErrors();
    

    m_len /= AUDIO_BYTESIZE(m_spec.format);

    printf(
R"({
    channels: %d
    format: %x
    frequency: %d
    samples: %u
    silence: %u
    size: %u
}
)", m_spec.channels, m_spec.format, m_spec.freq, m_spec.samples, m_spec.silence, m_spec.size
    );
}

AudioData::~AudioData()
{
    if (m_data) free((void*)m_data);
}

AudioData& AudioDevice::LoadAudio(const char* path)
{
    try
    {
        return *loaded_audios.at(path).get();
    }
    catch(const std::out_of_range& e)
    {
        File audio_file;
        audio_file.Load(path);

        loaded_audios.emplace(path, new AudioData(audio_file));
    }

    return *loaded_audios.at(path).get();
}

void AudioDevice::UnloadAudio(AudioData& data)
{
    for (auto& kv : loaded_audios)
    {
        if (kv.second.get() == &data)
            loaded_audios.erase(kv.first);
    }
}

AudioDevice::AudioDevice()
{
    SDL_AudioSpec desiredspec{0};
    desiredspec.callback = (SDL_AudioCallback)callback;
    desiredspec.userdata = this;

    desiredspec.channels = 1;
    desiredspec.freq = 32000;
    desiredspec.samples = 512;
    desiredspec.format = AUDIO_S32;

    SDL_InitSubSystem(SDL_INIT_AUDIO);

    this->m_Id = SDL_OpenAudioDevice(NULL, 0, &desiredspec, &m_Spec, 0);
    System::CheckForSDLErrors();
}

AudioDevice::~AudioDevice()
{
    SDL_CloseAudioDevice(m_Id);
}

void AudioDevice::callback(AudioDevice* dev, int32_t* stream, int len)
{
    memset(stream, 0, len);

    const uint8_t channel_count = dev->m_Spec.channels;

    const size_t byte_count = AUDIO_BYTESIZE(dev->m_Spec.format);
    const size_t sample_len = len / byte_count;

    printf("%p %d\n", stream, sample_len);

    Queue stop_queue;

    for (Audio* audio : dev->audio_queue)
    {
        const float faudio_sample_len = (float)audio->m_data->m_spec.freq / (float)dev->m_Spec.freq;
        for (size_t i = 0; i < sample_len; i++)
        {
            float a = audio->m_fsamplepos - audio->m_samplepos;

            Uint8* dst = (Uint8*)(stream + i);
            SDL_MixAudioFormat(dst, (Uint8*)((int32_t*)audio->m_data->m_data + audio->m_samplepos), dev->m_Spec.format, byte_count, 64);

            audio->m_fsamplepos += faudio_sample_len;
            audio->m_samplepos = audio->m_fsamplepos;

            if (audio->m_samplepos >= audio->m_data->m_len)
            {
                stop_queue.push(audio);
                break;
            }
        }
    }

    while (!stop_queue.empty())
    {
        dev->StopAudio(*stop_queue.front()); stop_queue.pop();
    }
}

void AudioDevice::PlayAudio(Audio& audio, bool force)
{
    SDL_LockAudioDevice(m_Id);
    if (force) StopAudio(audio);
    else if (IsAudioPlaying(audio))
    {
        SDL_UnlockAudioDevice(m_Id);
        return;
    }

    audio_queue.insert(&audio);
    
    SDL_PauseAudioDevice(m_Id, 0);

    SDL_UnlockAudioDevice(m_Id);
}

TimeStamp AudioDevice::PauseAudio(Audio& audio)
{
    SDL_LockAudioDevice(m_Id);

    audio_queue.erase(&audio);
    if (audio_queue.empty()) SDL_PauseAudioDevice(m_Id, 1);

    SDL_UnlockAudioDevice(m_Id);

    return audio.timePosition();
}