#include <standard>
#include "../internal.h"

#include "Base/Audio.hpp"
#include "Base/Thread.hpp"

#include "Sys.h"
#include "GameSettings.hpp"

void __audio_callback(void* data, uint8_t* stream, int len)
{
	if (!engine.audio_queue) return;

	memset(stream, 0, len);
	
	engine.audio_stream = stream;
	engine.audio_slen = len;
	__update_audio();
}

void __update_audio()
{
	const uint8_t channel_count = engine.audio_spec.channels;

	const size_t sample_len = static_cast<size_t>(engine.audio_slen / 2);

	std::queue<Audio*> stop_queue;

	for (Audio* audio : *_audio_queue)
	{
		const float faudio_channel_ratio = (float)engine.audio_spec.channels / (float)audio->m_data->m_spec.channels;
		const float faudio_sample_len = (float)audio->m_data->m_spec.freq / (float)engine.audio_spec.freq / faudio_channel_ratio;

		for (size_t i = 0; i <= sample_len - 1; i += audio->m_data->m_spec.channels)
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

			uint8_t* dest = engine.audio_stream + i * 2;
			short* src = reinterpret_cast<short*>(audio->m_data->m_data) + audio->m_samplepos * audio->m_data->m_spec.channels;

			short val = (short)ut_lerp(src[0], src[audio->m_data->m_spec.channels], a);
			short vals[2] = { val, val };

			int vol = static_cast<int>(audio->Info.volume * audio->m_fadevol * 64);
			if (audio->m_data->m_spec.channels == 2)
			{
				val = (short)ut_lerp(src[1], src[1+audio->m_data->m_spec.channels], a);
				vals[1] = val;
			}

			if (channel_count == 1)
			{
				val = (vals[0] + vals[1]) / 2;
				SDL_MixAudioFormat(dest, (Uint8*)&val, engine.audio_spec.format, 2, vol);
			}
			else
			{
				SDL_MixAudioFormat(dest, (Uint8*)vals, engine.audio_spec.format, 4, vol);
			}

			audio->m_fsamplepos += faudio_sample_len * audio->Info.speed;
			audio->m_samplepos = static_cast<uint32_t>(audio->m_fsamplepos);

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

void __setup_audio_device()
{
	SDL_AudioSpec desiredspec{0};
	desiredspec.callback = __audio_callback;

	desiredspec.freq = GameSettings::AudioOptions.Frequency;
	desiredspec.channels = 2;
	desiredspec.samples = 512;
	desiredspec.format = AUDIO_S16;

	int CHANGES = 0;
	if (!desiredspec.freq)
		CHANGES |= SDL_AUDIO_ALLOW_FREQUENCY_CHANGE;

	engine.audio_device = SDL_OpenAudioDevice(NULL, 0, &desiredspec, &engine.audio_spec, CHANGES);

	syslogln("%d", engine.audio_spec.freq);
}