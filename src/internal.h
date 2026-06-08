#pragma once
#include <SDL.h>
#include <standard>

#include <C_API.h>
#include <Datatypes/Units.h>
#include <Datatypes/TimeStamp.h>
#include <ints.h>

#include <Core/Render.h>
#include <Core/Window.h>

#define SRE_VIDEOV(x, func) (x)->vfptr->func((x)->driverdata)
#define SRE_VIDEO(x, func, ...) (x)->vfptr->func((x)->driverdata, __VA_ARGS__)
#define SRE_pIMGUI static_cast<::sre::ImGuiInterface*>(engine.imgui)

#if defined(_MSC_VER)
	#define SRE_ALIGN(x) __declspec(align(x))
#elif defined(__GNUC__)
	#define SRE_ALIGN(x) __attribute__((aligned(x)))
#else
	#define SRE_ALIGN(x)
#endif

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
		sre_Texture** freelist[3];
		size_t last;
	};
	
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
		
		int index;
		int flags;
		size_t texture_size;
		struct _texture_env textures;

		struct {
			sre_u32 lastdraw_flags;
			sre_blendMode blendmode;
			sre_rect2Dut scissor;

			bool desiredvsync;
			bool currentvsync;

			sre_unit camera_x;
			sre_unit camera_y;

			sre_Texture* texture;
		} state;

		void* _vector_data[4][3];
	};

	struct sre_objLayer;
	
	struct SRE_ALIGN(64) _engine_data
	{
		// Runtime data

		size_t frame;
		sre_timeStamp last_dt;
		sre_timeStamp target_dt;
		Uint32 user_event;
		SDL_threadID main_thrd;

		long long framestart_time;
		long long frameend_time;

		// Instance data

		void* _obj_layer_vdata[4];
		void* _obj_layerfl_vdata[4];

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


SRE_CAPI_BEGIN
	extern struct _engine_data __engine_data;

	extern bool __update_objects();
	extern void __setup_objects();
	extern void __cleanup_objects();

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

	extern void __render_flush();
SRE_CAPI_END

#define engine __engine_data // Macro for easier typing