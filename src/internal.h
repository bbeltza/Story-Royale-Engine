#pragma once
#include <SDL.h>
#include <standard>

#include <C_API.h>
#include <Datatypes/Units.h>
#include <Datatypes/TimeStamp.h>
#include <ints.h>

SRE_CAPI_BEGIN
	struct sre_videodriver;

	struct _win_settings
	{
		const char* title;
		int w, h;
		uint32_t flags, renderflags;
	};

	enum _engine_event
	{
		ENGINE_EVENT_DEFER,
		ENGINE_EVENT_RETDEFER,
		ENGINE_EVENT_RENDER,
		ENGINE_EVENT_ENTRY
	};

	struct _engine_retdefer
	{
    	void* userdata;
    	SDL_sem* sem;
    	sre_sptr ret;
	};

	#if defined(_MSC_VER)
		#define SRE_ALIGN(x) __declspec(align(x))
	#elif defined(__GNUC__)
		#define SRE_ALIGN(x) __attribute__((aligned(x)))
	#else
		#define SRE_ALIGN(x)
	#endif
	struct SRE_ALIGN(64) _engine_data
	{
		// Runtime data

		size_t frame;
		sre_timeStamp last_dt;
		sre_timeStamp target_dt;
		sre_timeStamp phys_target_dt;
		SDL_threadID main_thrd;

		long long framestart_time;
		long long frameend_time;

		//void* event_queue;

		// Instance data

		void* current_world;
		void* current_guilayer;

		SDL_mutex* destroyqueue_mutex;

		void* entry_thread;
		void* game_loop;

		// Window data

		SDL_Window* sdl_windowhndl;
		int auto_scalex;
		int auto_scaley;
		
		// Renderer data
		
		struct sre_videodriver* video;

		SDL_cond* render_cond;
		SDL_mutex* render_mutex;
		int osize_x, osize_y;

		// Audio data

		SDL_AudioSpec audio_spec;
		SDL_AudioDeviceID audio_device;

		void* audio_queue;
		size_t audio_queuesize;
		size_t audio_queuecap;
		int last_audioid;

		int audio_master;

		sre_byte* audio_stream;
		int audio_slen; // in bytes

		// Input data

		sre_unit mouse_x, mouse_y;
		uint32_t mouse_press;
		sre_unit scale_ratio; /* 1 / video->scale */
		SDL_TouchID input_last_touchid;
		sre_u8 keyboard_state[SDL_NUM_SCANCODES / 8];

		#if _WIN32
			int exposing;
		#endif
	};

	extern struct _engine_data __engine_data;

	extern void __initialize_engine();
	extern void __run_engine();
	extern void __end_engine(void);

	extern void __create_window();
	extern void __setup_renderer();
	extern void __cleanup_renderer();
	
	extern void __setup_audio_device();

	extern void __poll_input(SDL_Event* ev);

	extern int __event_filter(void*, SDL_Event* ev);
	extern int __signal_events(SDL_Event* ev);
	extern void __queue_events();

	extern void __update_viewport(int w, int h);

	extern void __query_objects();

	extern void __update_ecs();
	extern void __render_scene();
	extern void __render_ui();

	extern void __display_render();

	extern void __cleanup_ecs();
SRE_CAPI_END

#ifdef __cplusplus
#define currscn static_cast<::sreECS::Scene*>(engine.current_world) // ONLY USE IT WHEN YOU HAVE THE CLASS INCLUDED
#define currlayer static_cast<::sreGUI::Object*>(engine.current_guilayer) // Same with this...

#define _audio_loaded static_cast<std::unordered_map<std::string, std::unique_ptr<AudioData>> *>(engine.loaded_audios)
#define _audio_queue static_cast<std::unordered_set<Audio *> *>(engine.audio_queue)
#define _audio_stopqueue static_cast<std::queue<Audio *> *>(engine.stopped_audios)
#endif

#define engine __engine_data // Macro for easier typing