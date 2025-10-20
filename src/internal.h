#pragma once
#include <SDL.h>
#include <stdint.h>

#include "Datatypes/TimeStamp.h"

#ifdef __cplusplus
extern "C" {
#endif

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

		// Instance data

		void* current_world; // Designed to replace Game::World::m_Current
		void* current_guilayer; // And Game::GuiLayer::m_Current

		SDL_mutex* destroyqueue_mutex;

		// Multithreading data

		SDL_Thread* entry_thread;
		void* allocated_threads; // Either an std::list or an std::unordered_map

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

		void* loaded_textures;
		void* loadedfonts; // Both std::unordered_map

		void* target_textures; // An std::vector
		size_t targetptr;

		unsigned int integer_scale;
		float real_scale;

		// Audio device data

		SDL_AudioSpec audio_spec;
		SDL_AudioDeviceID audio_device;

		void* loaded_audios; // An std::unordered_map of unique pointers to AudioData structures
		void* audio_queue; // An std::unordered_set of Audio instances

		// Input data

		float mouse_x, mouse_y;
		uint32_t mouse_press;
		SDL_TouchID input_last_touchid;
		uint8_t keyboard_state[SDL_NUM_SCANCODES];
	};

	extern struct _engine_data __engine_data;

	extern void __initialize_engine();
	extern void __run_engine();
	extern void __end_engine();

	extern struct _win_settings __setup_window_data();
	extern void __create_window(const struct _win_settings* _win);
	extern void __setup_renderer(uint32_t flags);

	extern void __setup_audio_device();

	extern int __poll_events();
	extern void __poll_input();

	extern void __update_viewport();
	extern void __update_input();
	extern void __update_classes();

	extern void __destroy_queue();

	extern void __update_world();
	extern void __update_layer();

	extern void __display_render();

#ifdef __cplusplus
}

#define currworld reinterpret_cast<::Game::World*>(engine.current_world) // ONLY USE IT WHEN YOU HAVE THE CLASS INCLUDED
#define currlayer reinterpret_cast<::Game::GuiLayer*>(engine.current_guilayer) // Same with this...
#define flags_kbstate reinterpret_cast<::Flags8*>(engine.keyboard_state)
#define flags_mousepress (*reinterpret_cast<::Flags8*>(&engine.mouse_press))
#endif

#define engine __engine_data // Macro for easier typing