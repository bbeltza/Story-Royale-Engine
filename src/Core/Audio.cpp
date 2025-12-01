#include <standard>
#include "../internal.h"
#include "../internal.hpp"

#include "Base/Audio.hpp"
#include "Base/Thread.hpp"

#include "GameSettings.h"

void __audio_callback(void* data, uint8_t* stream, int len)
{
	if (!engine.containers_service) return;

	memset(stream, 0, len);
	
	engine.audio_stream = stream;
	engine.audio_slen = len;
	__update_audio();
}

void __update_audio()
{
	const uint8_t channel_count = engine.audio_spec.channels;
	const size_t sample_len = static_cast<size_t>(engine.audio_slen / 2);

	for (Audio* audio : _containers->audio_queue)
	{
		const uint32_t loop_end = ((uint32_t)audio->Info.loop_end < audio->m_data->m_len ? audio->Info.loop_end : audio->m_data->m_len);

		const uint8_t audio_channels = audio->m_data->m_spec.channels;
		const size_t minchannels = ut_min(audio_channels, channel_count);

		const float faudio_channel_ratio = (float)engine.audio_spec.channels / (float)audio->m_data->m_spec.channels;
		const float faudio_sample_len = (float)audio->m_data->m_spec.freq / (float)engine.audio_spec.freq / faudio_channel_ratio;

		const float step_count = faudio_sample_len * audio->Info.speed / (audio_channels / minchannels);


		for (size_t i = 0; i < sample_len; i += minchannels)
		{
			double a = audio->m_fsamplepos - audio->m_samplepos;

			//

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
					_containers->stopped_audios.push(audio);
					break;
				}
			}
			else
				audio->m_fadevol = 1;
			
			int vol = static_cast<int>(audio->Info.volume * audio->m_fadevol * SDL_MIX_MAXVOLUME);
			ut_setclamp(vol, 0, SDL_MIX_MAXVOLUME);

			uint8_t* dest = engine.audio_stream + i * 2;
			short* src = reinterpret_cast<short*>(audio->m_data->m_data) + audio->m_samplepos * audio_channels;

			short vals[2] = {
				src[0],
				src[1 - 2 + audio_channels]
			};
			short nextvals[2] = {
				src[audio_channels],
				src[audio_channels*2 - 1]
			};

			if (channel_count == 1)
			{
				short val = (vals[0] + vals[1]) / 2;
				short next = (nextvals[0] + nextvals[1]) / 2;

				val = (short)ut_lerp(val, next, a);

				SDL_MixAudioFormat(dest, (Uint8*)&val, engine.audio_spec.format, 2, vol);
			}
			else
			{
				//a = pow(a, exp(1));
				//a = pow(a, 2);
				vals[0] = (short)ut_lerp(vals[0], nextvals[0], a);
				vals[1] = (short)ut_lerp(vals[1], nextvals[1], a);

				SDL_MixAudioFormat(dest, (Uint8*)vals, engine.audio_spec.format, 4, vol);
			}

			audio->m_fsamplepos += step_count;
			audio->m_samplepos = static_cast<uint32_t>(audio->m_fsamplepos);

			if (audio->Info.looped && audio->m_samplepos >= loop_end)
			{
				audio->m_fsamplepos = audio->Info.loop_start;
			}
			else if (audio->m_samplepos >= loop_end)
			{
				_containers->stopped_audios.push(audio);
				break;
			}
		}
	}

	while (!_containers->stopped_audios.empty())
	{
		_containers->stopped_audios.front()->Stop();
		_containers->stopped_audios.pop();
	}
}

void __setup_audio_device()
{
	SDL_AudioSpec desiredspec{0};
	desiredspec.callback = __audio_callback;

	desiredspec.freq = sre::game_settings.AudioOptions.Frequency; // I should make an enum for different frequencies instead of letting the user have free choice
	desiredspec.channels = 2 - sre::game_settings.AudioOptions.Mono;
	desiredspec.samples = 512;
	desiredspec.format = AUDIO_S16;

	int CHANGES = 0;
	if (!desiredspec.freq)
		CHANGES |= SDL_AUDIO_ALLOW_FREQUENCY_CHANGE;

	engine.audio_device = SDL_OpenAudioDevice(NULL, 0, &desiredspec, &engine.audio_spec, CHANGES);
}