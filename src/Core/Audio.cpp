#include <SDL.hpp>
#include "System.h"
#include "Base/AudioDevice.h"

#include "vorbis.h"

#include "utils.h"

extern "C" int ConvertAudioFormat(SDL_AudioFormat f_input, SDL_AudioFormat f_output, int8_t** d_input, int len_input);

AudioData::AudioData(File& file)
{
    const void* f_data = file.getRawData();
    size_t f_size = file.getSize();

    if (!f_size) {/*Will do something*/}

    SDL_RWops* audio_rw = SDL_RWFromConstMem(f_data, f_size);
    
    {
        int channels_int, err;
        Uint8* stream_data;
        SDL_zero(m_spec);

        if (SDL_LoadWAV_RW(audio_rw, 1, &m_spec, &stream_data, &m_len))
        {
            file.setType(File::T_WAV);
            m_data = (int8_t*)malloc(m_len);
            memcpy(m_data, stream_data, m_len);
            SDL_FreeWAV(stream_data);
            m_len /= AUDIO_BYTESIZE(m_spec.format);
            m_len /= m_spec.channels;
        }
        else if ((m_len = stb_vorbis_decode_memory((uint8_t*)file.getRawData(), file.getSize(), &channels_int, &m_spec.freq, (short**)&m_data)) > 0)
        {
            file.setType(File::T_OGG);
            m_spec.channels = channels_int;
            m_spec.format = AUDIO_S16;
        }
        
        printf("m_len: %f\n", m_len);
        SDL_ClearError();
    }

    


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
    AudioData* audio;

    try
    {
        audio = loaded_audios.at(path).get();
        return *audio;
    }
    catch(const std::out_of_range&)
    {
        File audio_file;
        audio_file.Load(path);

        loaded_audios.emplace(path, new AudioData(audio_file));
        
        audio = loaded_audios.at(path).get();

        ConvertAudioFormat(audio->m_spec.format, m_Spec.format, &audio->m_data, audio->m_len * audio->m_spec.channels * AUDIO_BYTESIZE(audio->m_spec.format));
        audio->m_spec.format = m_Spec.format;

        printf("new m_len from %s: %d\n", path, audio->m_len);
    }

    return *audio;
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

    desiredspec.channels = 2;
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

    //printf("%p %d\n", stream, sample_len);

    Queue stop_queue;

    for (Audio* audio : dev->audio_queue)
    {
        const float faudio_channel_ratio = (float)dev->m_Spec.channels / (float)audio->m_data->m_spec.channels;
        const float faudio_sample_len = (float)audio->m_data->m_spec.freq / (float)dev->m_Spec.freq / faudio_channel_ratio;

        for (size_t i = 0; i < sample_len; i += audio->m_data->m_spec.channels)
        {
            float a = audio->m_fsamplepos - audio->m_samplepos;

            for (int c = 0; c < channel_count; c++)
            {
                int cc = audio->m_data->m_spec.channels > 1 ? c : 0;
                Uint8* dst = (Uint8*)(stream + i + c);
                SDL_MixAudioFormat(dst, (Uint8*)((int32_t*)audio->m_data->m_data + audio->m_samplepos * audio->m_data->m_spec.channels + cc), dev->m_Spec.format, byte_count, 64);

            }

            audio->m_fsamplepos += faudio_sample_len * audio->Info.speed;
            audio->m_samplepos = (uint32_t)audio->m_fsamplepos;

            
            int loop_end = ((signed int)audio->Info.loop_end < audio->m_data->m_len ? audio->Info.loop_end : audio->m_data->m_len);

            if (audio->Info.looped && audio->m_samplepos >= loop_end)
            {
                audio->m_fsamplepos = audio->Info.loop_start;
            }
            else if (audio->m_samplepos >= loop_end)
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