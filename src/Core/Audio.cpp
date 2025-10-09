#include <SDL.hpp>
#include "Engine.hpp"
#include "Base/AudioDevice.hpp"

#include "vorbis.h"

#include "utils.h"

extern "C" int ConvertAudioFormat(SDL_AudioFormat f_input, SDL_AudioFormat f_output, int8_t **d_input, int len_input);

void AudioData::Load()
{
    const int f_size = (int)m_file.getSize();
    const void* f_data = m_file.getRawData();

    SDL_RWops *audio_rw = SDL_RWFromConstMem(f_data, f_size);
    
    int channels_int;
    Uint8 *stream_data;
    SDL_zero(m_spec);
    
    if (SDL_LoadWAV_RW(audio_rw, 0, &m_spec, &stream_data, &m_len))
    {
        m_file.setType(File::T_WAV);
        m_data = (int8_t *)malloc(m_len);
        memcpy(m_data, stream_data, m_len);
        SDL_FreeWAV(stream_data);
        m_len /= AUDIO_BYTESIZE(m_spec.format);
        m_len /= m_spec.channels;
    }
    else if ((m_len = stb_vorbis_decode_memory((uint8_t *)f_data, f_size, &channels_int, &m_spec.freq, (short **)&m_data)) > 0)
    {
        m_file.setType(File::T_OGG);
        m_spec.channels = channels_int;
        m_spec.format = AUDIO_S16;
    }

    SDL_ClearError();
    SDL_FreeRW(audio_rw);
}

AudioData::AudioData(const char *path): Loaded(this)
{
    m_file.Load(path);

    if (!m_file.getSize())
    { /*Will do something*/
    }
}

AudioData::~AudioData()
{
    if (m_data)
        free((void *)m_data);
}

void AudDevice::threadedload(AudDevice *dev, AudioData* audio)
{
    audio->Load();
    ConvertAudioFormat(audio->m_spec.format, dev->m_Spec.format, &audio->m_data, audio->m_len * audio->m_spec.channels * AUDIO_BYTESIZE(audio->m_spec.format));

    audio->m_spec.format = dev->m_Spec.format;
    audio->m_loaded = true;


    audio->Loaded.Fire();
}

AudioData &AudDevice::LoadAudio(const char *path)
{
    AudioData *audio;

    if (loaded_audios.count(path) == 0)
    {
        loaded_audios.emplace(path, new AudioData(path));
        audio = loaded_audios.at(path).get();
        m_Engine->ThreadPool.CreateImmediateThread(threadedload, this, audio);
    }
    else
        audio = loaded_audios.at(path).get();

    return *audio;
}

void AudDevice::UnloadAudio(AudioData &data)
{
    for (auto &kv : loaded_audios)
    {
        if (kv.second.get() == &data)
            loaded_audios.erase(kv.first);
    }
}

AudDevice::AudDevice(EngineClass* engine): m_Engine(engine)
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
}

AudDevice::~AudDevice()
{
    SDL_CloseAudioDevice(m_Id);
}

void AudDevice::callback(AudDevice *dev, int32_t *stream, int len)
{
    memset(stream, 0, len);

    const uint8_t channel_count = dev->m_Spec.channels;

    const uint32_t byte_count = AUDIO_BYTESIZE(dev->m_Spec.format);
    const size_t sample_len = len / byte_count;

    Queue stop_queue;

    for (Audio *audio : dev->audio_queue)
    {
        const float faudio_channel_ratio = (float)dev->m_Spec.channels / (float)audio->m_data->m_spec.channels;
        const float faudio_sample_len = (float)audio->m_data->m_spec.freq / (float)dev->m_Spec.freq / faudio_channel_ratio;

        for (size_t i = 0; i < sample_len; i += audio->m_data->m_spec.channels)
        {
            double a = audio->m_fsamplepos - audio->m_samplepos;

            if (audio->m_fadein)
            {
                audio->m_fadevol += audio->Info.fade_in / dev->m_Spec.freq;
                if (audio->m_fadevol >= 1)
                {
                    audio->m_fadein = false;
                    audio->m_fadevol = 1;
                }
            }
            else if (audio->m_fadeout)
            {
                audio->m_fadevol -= audio->Info.fade_out / dev->m_Spec.freq;
                if (audio->m_fadevol <= 0)
                {
                    audio->m_fadeout = false;
                    stop_queue.push(audio);
                    break;
                }
            }
            else
                audio->m_fadevol = 1;

            for (int c = 0; c < channel_count; c++)
            {
                int cc = audio->m_data->m_spec.channels > 1 ? c : 0;
                Uint8 *dst = (Uint8 *)(stream + i + c);
                SDL_MixAudioFormat(dst, (Uint8 *)((int32_t *)audio->m_data->m_data + audio->m_samplepos * audio->m_data->m_spec.channels + cc), dev->m_Spec.format, byte_count, (int)(audio->Info.volume * audio->m_fadevol * 64));
            }

            audio->m_fsamplepos += faudio_sample_len * audio->Info.speed;
            audio->m_samplepos = (uint32_t)audio->m_fsamplepos;

            uint32_t loop_end = ((uint32_t)audio->Info.loop_end < audio->m_data->m_len ? audio->Info.loop_end : audio->m_data->m_len);

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
        dev->StopAudio(*stop_queue.front());
        stop_queue.pop();
    }
}

void AudDevice::PlayAudio(Audio &audio, bool force)
{
    if (!audio.m_data->m_loaded)
        audio.m_data->Loaded.Wait();
    
    SDL_LockAudioDevice(m_Id);
    if (force)
        StopAudio(audio);
    else if (IsAudioPlaying(audio))
    {
        SDL_UnlockAudioDevice(m_Id);
        return;
    }

    audio_queue.insert(&audio);

    if (audio.Info.fade_in > 0)
    {
        audio.m_fadein = true;
        audio.m_fadevol = 0;
    }

    SDL_PauseAudioDevice(m_Id, 0);

    SDL_UnlockAudioDevice(m_Id);
}

TimeStamp AudDevice::PauseAudio(Audio &audio)
{
    SDL_LockAudioDevice(m_Id);

    audio_queue.erase(&audio);
    if (audio_queue.empty())
        SDL_PauseAudioDevice(m_Id, 1);

    SDL_UnlockAudioDevice(m_Id);

    return audio.timePosition();
}
