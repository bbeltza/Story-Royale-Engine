#pragma once
#include <SDL.h>
#include <stdint.h>

#include "C/API.h"
#include "Datatypes/TimeStamp.h"

_CAPI_BEGIN
	struct _win_settings
	{
		const char* title;
		int w, h;
		uint32_t flags, renderflags;
	};

	struct _engine_data
	{

		// Runtime data

		intptr_t frame;
		TimeStamp last_dt;
		TimeStamp target_dt;

		// Instance data

		void* current_world;
		void* current_guilayer;

		SDL_mutex* destroyqueue_mutex;

		SDL_Thread* entry_thread;

		// Window data

		SDL_Window* sdl_windowhndl;
		SDL_Event sdl_eventhndl;
		
		// Renderer data
		
		SDL_Renderer* sdl_rendererhndl;
		SDL_Texture* sdl_rectTex;
		SDL_mutex* sdl_rendermutex;
		SDL_Rect viewport;
		int osize_x, osize_y;
		float center_x, center_y;

		size_t targetptr;

		float viewport_scale;

		// Audio data

		SDL_AudioSpec audio_spec;
		SDL_AudioDeviceID audio_device;

		unsigned char* audio_stream;
		int audio_slen; // in bytes

		// Input data

		float mouse_x, mouse_y;
		uint32_t mouse_press;
		SDL_TouchID input_last_touchid;
		uint8_t keyboard_state[SDL_NUM_SCANCODES];

		void* containers_service;
	};

	extern struct _engine_data __engine_data;

	extern void __initialize_engine();
	extern void __run_engine();
	extern void __end_engine(void);

	extern struct _win_settings __setup_window_data();
	extern void __create_window(const struct _win_settings* _win);
	extern void __setup_renderer(uint32_t flags);
	
	extern void __init_containers();
	extern void __init_actions();

	extern void __setup_audio_device();

	extern int __poll_events();
	extern void __poll_input();

	extern void __update_viewport();
	extern void __update_input();

	extern void __query_objects();

	extern void __destroy_queue();

	extern void __update_timers();
	extern void __update_classes();
	extern void __update_world();
	extern void __update_layer();

	extern void __update_audio();

	extern void __display_render();

	extern void __clean_containers();
_CAPI_END

#ifdef __cplusplus
#define currworld static_cast<::Game::World*>(engine.current_world) // ONLY USE IT WHEN YOU HAVE THE CLASS INCLUDED
#define currlayer static_cast<::Game::GuiLayer*>(engine.current_guilayer) // Same with this...
#define flags_kbstate reinterpret_cast<::Flags8*>(engine.keyboard_state)
#define flags_mousepress (*reinterpret_cast<::Flags32*>(&engine.mouse_press))

#define _audio_loaded static_cast<std::unordered_map<std::string, std::unique_ptr<AudioData>> *>(engine.loaded_audios)
#define _audio_queue static_cast<std::unordered_set<Audio *> *>(engine.audio_queue)
#define _audio_stopqueue static_cast<std::queue<Audio *> *>(engine.stopped_audios)

#define _fonts_loaded static_cast<std::list<TTF_Font*> *>(engine.loaded_fonts)
#endif

#define engine __engine_data // Macro for easier typing