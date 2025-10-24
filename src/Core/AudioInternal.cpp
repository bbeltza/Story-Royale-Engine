#include <standard>
#include "../internal.h"

#include "Base/Audio.hpp"
#include "Classes/Thread.hpp"

#include "Sys.h"

extern "C" int ConvertAudioFormat(SDL_AudioFormat f_input, SDL_AudioFormat f_output, int8_t **d_input, int len_input);

#define loaded reinterpret_cast<std::unordered_map<std::string, std::unique_ptr<AudioData>> *>(engine.loaded_audios)
#define aqueue reinterpret_cast<std::unordered_set<Audio*> *>(engine.audio_queue)

void __setup_audio_device()
{
	SDL_AudioSpec desiredspec{ 0 };
	desiredspec.callback = (SDL_AudioCallback)__audio_callback;

	desiredspec.channels = 2;
	desiredspec.freq = 32000;
	desiredspec.samples = 512;
	desiredspec.format = AUDIO_S32;

	engine.audio_device = SDL_OpenAudioDevice(NULL, 0, &desiredspec, &engine.audio_spec, 0);
}

void Audio::threadedload(AudioData* audio)
{
    audio->Load();
    ConvertAudioFormat(audio->m_spec.format, engine.audio_spec.format, &audio->m_data, audio->m_len * audio->m_spec.channels * AUDIO_BYTESIZE(audio->m_spec.format));

    audio->m_spec.format = engine.audio_spec.format;
    audio->m_loaded = true;

    audio->Loaded.Fire();
}

AudioData& Audio::Load(const char* path)
{
	AudioData* audio;

	if (loaded->count(path) == 0)
	{
		loaded->emplace(path, new AudioData(path));
		audio = loaded->at(path).get();
		audio->thrd = Threads::Create(threadedload, audio);
	}
	else
		audio = loaded->at(path).get();

	return *audio;
}

void AudioData::Unload()
{
	for (auto& kv : *loaded)
	{
		if (kv.second.get() == this)
			return (void)loaded->erase(kv.first);
	}
	abort();
}

void __audio_callback(void* data, uint8_t* stream, int len)
{
	if (!aqueue) return;

	memset(stream, 0, len);

	const uint8_t channel_count = engine.audio_spec.channels;

	const uint32_t byte_count = AUDIO_BYTESIZE(engine.audio_spec.format);
	const size_t sample_len = static_cast<size_t>(len / byte_count);

	std::queue<Audio*> stop_queue;

	for (Audio* audio : *aqueue)
	{
		const float faudio_channel_ratio = (float)engine.audio_spec.channels / (float)audio->m_data->m_spec.channels;
		const float faudio_sample_len = (float)audio->m_data->m_spec.freq / (float)engine.audio_spec.freq / faudio_channel_ratio;

		for (size_t i = 0; i <= sample_len - 2; i += audio->m_data->m_spec.channels)
		{
			double a = audio->m_fsamplepos - audio->m_samplepos;

			if (audio->m_fadein)
			{
				audio->m_fadevol += audio->Info.fade_in / engine.audio_spec.freq;
				if (audio->m_fadevol >= 1)
				{
					audio->m_fadein = false;
					audio->m_fadevol = 1;
				}
			}
			else if (audio->m_fadeout)
			{
				audio->m_fadevol -= audio->Info.fade_out / engine.audio_spec.freq;
				if (audio->m_fadevol <= 0)
				{
					audio->m_fadeout = false;
					stop_queue.push(audio);
					break;
				}
			}
			else
				audio->m_fadevol = 1;

			for (size_t c = 0; c < channel_count; c++)
			{
				size_t cc = audio->m_data->m_spec.channels > 1 ? c : 0;
				uint8_t* dst = stream + (i + c) * byte_count;
				int32_t* src = reinterpret_cast<int32_t*>(audio->m_data->m_data) + audio->m_samplepos * audio->m_data->m_spec.channels + cc;
				int vol = static_cast<int>(audio->Info.volume * audio->m_fadevol * 64);
				SDL_MixAudioFormat(dst, (Uint8*)src, engine.audio_spec.format, 4, vol);
				
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
		stop_queue.front()->Stop();
		stop_queue.pop();
	}
}

void Audio::Play(bool force)
{
	if (!m_data->m_loaded)
		m_data->Loaded.Wait();
	SDL_LockAudioDevice(engine.audio_device);
	if (force)
		Stop();
	else if (IsPlaying())
	{
		SDL_UnlockAudioDevice(engine.audio_device);
		return;
	}

	aqueue->insert(this);

	if (Info.fade_in > 0)
	{
		m_fadein = true;
		m_fadevol = 0;
	}
	m_fadeout = false;

	SDL_PauseAudioDevice(engine.audio_device, 0);

	SDL_UnlockAudioDevice(engine.audio_device);
}

TimeStamp Audio::Stop()
{
	auto ts = Pause();
	m_fsamplepos = 0;
	m_samplepos = 0;
	return ts;
}

TimeStamp Audio::Pause()
{
	SDL_LockAudioDevice(engine.audio_device);

	aqueue->erase(this);
	if (aqueue->empty())
		SDL_PauseAudioDevice(engine.audio_device, 1);

	SDL_UnlockAudioDevice(engine.audio_device);

	return timePosition();
}

bool Audio::IsPlaying() const
{
	return aqueue->count(const_cast<Audio*>(this)) != 0;
}