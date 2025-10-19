#pragma once
#include <SDL.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

	struct _engine_data
	{
		// Runtime data

		signed size_t frame;

		// Instance data

		void* current_world; // Designed to replace Game::World::m_Current
		void* current_guilayer; // And Game::GuiLayer::m_Current

		// Multithreading data

		void* allocated_threads; // Either an std::list or an std::unordered_map

		// Window data

		SDL_Window* sdl_windowhndl;
		SDL_Event sdl_eventhndl;

		// Renderer data

		SDL_Renderer* sdl_rendererhndl;
		SDL_Texture* sdl_rectTex;
		SDL_mutex* sdl_rendermutex;
		SDL_Rect viewport;

		void* loaded_textures;
		void* loadedfonts; // Both std::unordered_map

		void* target_textures; // An std::vector
		size_t targetptr;

		unsigned int integer_scale;

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

	extern void __initialize_engine(_engine_data* engine);
	extern void __run_current_engine();

#ifdef __cplusplus
}
#endif