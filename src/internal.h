#pragma once
#include <SDL.h>
#include <standard>

#include <C_API.h>
#include <datatypes/units.h>
#include <datatypes/timestamp.h>
#include <ints.h>

SRE_CAPI_BEGIN
	typedef struct sre_videodriver sre_videodriver;

	struct _win_settings
	{
		const char* title;
		int w, h;
		uint32_t flags, renderflags;
	};

	struct _engine_data
	{

		// Runtime data

		size_t frame;
		sre_timeStamp last_dt;
		sre_timeStamp target_dt;
		sre_timeStamp phys_target_dt;

		unsigned long long framestart_time;
		unsigned long long frameend_time;

		// Instance data

		void* current_world;
		void* current_guilayer;

		SDL_mutex* destroyqueue_mutex;

		SDL_Thread* entry_thread;

		// Window data

		SDL_Window* sdl_windowhndl;
		
		// Renderer data
		
		sre_videodriver* video;
		//
		SDL_Renderer* sdl_rendererhndl;
		SDL_Texture* sdl_rectTex;
		//

		SDL_mutex* sdl_rendermutex;
		int osize_x, osize_y;
		sre_unit size_x, size_y;
		sre_unit center_x, center_y;

		size_t targetptr;

		sre_unit current_scale;
		sre_unit viewport_scale;

		// Audio data

		SDL_AudioSpec audio_spec;
		SDL_AudioDeviceID audio_device;

		sre_byte* audio_stream;
		int audio_slen; // in bytes

		// Input data

		sre_unit mouse_x, mouse_y;
		uint32_t mouse_press;
		SDL_TouchID input_last_touchid;
		uint8_t keyboard_state[SDL_NUM_SCANCODES];

		void* containers_service;
	};

	extern struct _engine_data __engine_data;

	extern void __initialize_engine();
	extern void __run_engine();
	extern void __end_engine(void);

	extern void __create_window();
	extern void __setup_renderer();
	
	extern void __init_containers();

	extern void __setup_audio_device();

	extern int __poll_events();
	extern void __poll_input(SDL_Event* ev);

	extern void __update_viewport();
	extern void __update_input();

	extern void __query_objects();

	extern void __destroy_queue();

	extern void __update_classes();
	extern void __update_world();
	extern void __update_layer();

	extern void __update_audio();

	extern void __display_render();

	extern void __clean_containers();
SRE_CAPI_END

#ifdef __cplusplus
#define currscn static_cast<::sreECS::Scene*>(engine.current_world) // ONLY USE IT WHEN YOU HAVE THE CLASS INCLUDED
#define currlayer static_cast<::sreGUI::Object*>(engine.current_guilayer) // Same with this...
#define flags_kbstate reinterpret_cast<::sre::flags8*>(engine.keyboard_state)
#define flags_mousepress (*reinterpret_cast<::sre::flags32*>(&engine.mouse_press))

#define _audio_loaded static_cast<std::unordered_map<std::string, std::unique_ptr<AudioData>> *>(engine.loaded_audios)
#define _audio_queue static_cast<std::unordered_set<Audio *> *>(engine.audio_queue)
#define _audio_stopqueue static_cast<std::queue<Audio *> *>(engine.stopped_audios)

#define _fonts_loaded static_cast<std::list<TTF_Font*> *>(engine.loaded_fonts)
#endif

#define engine __engine_data // Macro for easier typing