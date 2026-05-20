#pragma once
#include <SDL.h>
#include <standard>

#include <C_API.h>
#include <Datatypes/Units.h>
#include <Datatypes/TimeStamp.h>
#include <ints.h>

#include <Core/Render.h>

SRE_CAPI_BEGIN
	struct _win_settings
	{
		const char* title;
		int w, h;
		uint32_t flags, renderflags;
	};

	enum _engine_eventcode
	{
		ENGINE_EVENT_DEFERV, // defer, passed by value (`argsize` = 0)
		ENGINE_EVENT_DEFERP, // defer, passed as a pointer directly in the event's `data2` (`argsize` <= sizeof(void*))
		ENGINE_EVENT_DEFERPM, // defer, passed as an allocated pointer, the pointer being `data2`
		ENGINE_EVENT_DEFERRES, // defer with response yielding (`sre_defer_res`)
		ENGINE_EVENT_RENDER,
		ENGINE_EVENT_ENTRY
	};

	struct _texture_container;
	struct _texture_env
	{
		struct _texture_container* head;
		sre_Sampler** freelist[3];
		size_t last;
	};
	
	#define SRE_VIDEOV(x, func) (x)->vfptr->func((x)->driverdata)
	#define SRE_VIDEO(x, func, ...) (x)->vfptr->func((x)->driverdata, __VA_ARGS__)
	struct _engine_renderdata
	{
		struct
		{
			const struct sre_RenderVFT* vfptr;
			char driverdata[
				#ifdef __cplusplus
					1
				#endif
			];
		}* driver;
		
		size_t texture_size;
		struct _texture_env textures;

		int index;

		short blendmode;
		bool wantclear;
		sre_rect2Di clip_rect;

		void* _vector_data[4][3];
	};

	#define SRE_pIMGUI static_cast<::sre::ImGuiInterface*>(engine.imgui)

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
		Uint32 user_event;
		SDL_threadID main_thrd;

		long long framestart_time;
		long long frameend_time;

		//void* event_queue;

		// Instance data

		void* current_world;
		void* current_guilayer;

		SDL_mutex* destroyqueue_mutex;

		void* coroutine_thread;
		bool cor_running;

		// Window data

		SDL_Window* sdl_windowhndl;
		int auto_scalex;
		int auto_scaley;
		
		// Renderer data
		
		struct _engine_renderdata video;
		void* imgui;

		int osize_x, osize_y;
		sre_unit vsize_x, vsize_y;
		sre_unit vcenter_x, vcenter_y;

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
		sre_u16 mouse_press;
		sre_u16 mouse_framepress;
		sre_unit scale;
		sre_unit scale_ratio; /* 1 / engine.scale */
		sre_u8 keyboard_state[SDL_NUM_SCANCODES / 8];
		sre_u8 keyboard_framestate[SDL_NUM_SCANCODES / 8];

		// sre_unit uni_x, uni_y; // Might replace `mouse_x` and `mouse_y`

		// Touch finger data (not working yet)
		SDL_TouchID input_last_touchid;
		int lastfingerup;
	};
	extern struct _engine_data __engine_data;

	extern void __initialize_engine();
	extern void __run_engine();
	extern void __end_engine(void);

	extern void __create_window();
	extern void __setup_renderer();
	extern void __cleanup_renderer();

	extern void __initialize_imgui(void* imgui);
	extern bool __onevent_imgui(SDL_Event* ev);
	
	extern void __setup_audio_device();

	extern void __poll_input(SDL_Event* ev);

	extern int __signal_events(SDL_Event* ev);
	extern void __queue_events();

	extern void __update_viewport(int w, int h);

	extern void __query_objects();

	extern bool __update_ecs();
	extern void __render_flush();

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