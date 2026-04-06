#include "../internal.h"
#include "drivers/drivers.h"

#include <Core/Texture.hpp>
#include <Core/Runtime.hpp>

#include <ECS/Scene.hpp>
#include <GUI/Object.hpp>

#include <utils/mem.h>

// ImGUI setup!
	#include <imgui.h>
	#include <backends/imgui_impl_sdl2.cpp> // Compile SDL2 ImGui implementation!
	#include <backends/imgui_impl_null.cpp>
//

#define VIDEO_DRIVERS				\
VIDEOINIT_DEF(sdlrenderer)			\
VIDEOINIT_DEF(opengl)				\
VIDEOINIT_DEF(d3d12)				\
//VIDEOINIT_DEF(softwarerender)

#define VIDEOINIT_DEF(x) extern "C" bool sre##x##_init(sre_videodriver* video, SDL_Window* window);
VIDEO_DRIVERS

#undef VIDEOINIT_DEF
#define VIDEOINIT_DEF(x) sre##x##_init,
static const sre_videoinit_func video_drivers[] = {
	VIDEO_DRIVERS
};

void __cleanup_renderer()
{
	engine.video->interface->quit(engine.video);
	delete[] engine.video->texture_fl;
	delete[] engine.video->clipstack_base;
	operator delete (engine.video->textures);
	operator delete (engine.video);

	engine.video = NULL;
}

void __setup_renderer()
{
	engine.video = static_cast<sre_videodriver*>(operator new(sizeof(sre_videodriver)));
	memset(static_cast<void*>(engine.video), 0, sizeof(sre_videodriver));

	enum sre_RenderDrivers
	{
		SRE_RENDERDRIVER_SDLRENDERER,
		SRE_RENDERDRIVER_OPENGL,

#if _WIN32 && 01
		SRE_RENDERDRIVER_DIRECTX12,
		SRE_RENDERDRIVER_DEFAULT = SRE_RENDERDRIVER_DIRECTX12
#elif !defined(ANDROID) && 1
        SRE_RENDERDRIVER_DEFAULT = SRE_RENDERDRIVER_OPENGL
#else
        SRE_RENDERDRIVER_DEFAULT = SRE_RENDERDRIVER_SDLRENDERER
#endif
	};

	const sre_videoinit_func init_driver = video_drivers[SRE_RENDERDRIVER_DEFAULT];
	if (!init_driver(engine.video, engine.sdl_windowhndl))
	{
		sre::log<sre::LOGCATEGORY_ERROR>("Failed initializing the render driver");
		exit(-1);
	}

	#define PFN_CHECK(i,x) if (i->x == nullptr) { sre::log<sre::LOGCATEGORY_ERROR>("Video driver error: '" #x "' interface function is missing. Please implement it"); } (void)0
	const sre_videodriverInterface* interface = engine.video->interface; assert(interface != NULL);

	PFN_CHECK(interface, quit);
	PFN_CHECK(interface, present);
	PFN_CHECK(interface, vsync);
	PFN_CHECK(interface, blend);

	PFN_CHECK(interface, tex_create);
	PFN_CHECK(interface, tex_update);
	PFN_CHECK(interface, tex_destroy);
	PFN_CHECK(interface, tex_size);
	PFN_CHECK(interface, tex_format);
	assert(engine.video->texture_size >= sizeof(int));	
	
	PFN_CHECK(interface, draw_clear);
	PFN_CHECK(interface, draw_clip);

	PFN_CHECK(interface, draw_fill);
	PFN_CHECK(interface, draw_lines);
	PFN_CHECK(interface, draw_rect);
	PFN_CHECK(interface, draw_rrect);
	PFN_CHECK(interface, draw_texture);
	PFN_CHECK(interface, draw_rtexture);	
	
	interface->blend(engine.video, SRE_BLEND_DEFAULT);

#ifndef IMGUI_DISABLE
	const sre_videodriverImGuiInterface* imgui = engine.video->imgui;
	
	if (imgui)
	{
		PFN_CHECK(imgui, imgui_init);
		PFN_CHECK(imgui, imgui_newframe);
		PFN_CHECK(imgui, imgui_renderdrawdata);

		IMGUI_CHECKVERSION();
		ImGuiContext* context = ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard
					   |  ImGuiConfigFlags_NavEnableGamepad;
		
		if (!ImGui_ImplSDL2_InitForOther(engine.sdl_windowhndl) ||
			!imgui->imgui_init(engine.video))
		{
			sre::log<sre::LOGCATEGORY_ERROR>("Failed initializing ImGui, video driver error");
			ImGui::DestroyContext(context);
		}
	}
	else
	{
		sre::log<sre::LOGCATEGORY_WARN>("ImGui is not implemented in the current video driver");
		// Create dummy context
		ImGui::CreateContext();
		ImGui_ImplNull_Init();
	}
#endif

	engine.video->textures = new sre::byte[SRE_TEXTURE_BASECOUNT * engine.video->texture_size] {};
	engine.video->texture_fl = new sre::u32[SRE_TEXTURE_BASECOUNT] {};
		
	engine.video->textures_capacity = SRE_TEXTURE_BASECOUNT;
	engine.video->texture_flcapacity = SRE_TEXTURE_BASECOUNT;

	engine.video->clipstack_base = new sre::rect2Dut[SRE_TEXTURE_BASECOUNT]; // I think I'll make both textures and clip rect stacks start with the same size
	engine.video->clipstack_size = SRE_TEXTURE_BASECOUNT;

	interface->vsync(engine.video, 1);
	engine.video->scale = 1;

	engine.render_mutex = SDL_CreateMutex();
	engine.render_cond = SDL_CreateCond();
}

void __update_viewport(int w, int h)
{
	int integer_scale;

	if (engine.auto_scalex && engine.auto_scaley)
	{
		integer_scale = ut_min(w / engine.auto_scalex, h / engine.auto_scaley);
		integer_scale = integer_scale ? integer_scale : 1;
	}
	else
	{
		integer_scale = static_cast<int>(engine.video->scale);
	}

	sre::unit scale = static_cast<sre::unit>(integer_scale);
	sre::vec2ut size = sre::vec2ut{ w, h } / scale;
	
	sre::vec2ut center = size / 2.0_ut;

	engine.osize_x = w;
	engine.osize_y = h;
	engine.scale_ratio = 1 / scale;
	engine.video->scale = scale;
	engine.video->size = size;
	engine.video->center = center;

	auto interface = engine.video->interface;
	if (interface->viewport)
		interface->viewport(engine.video, w, h);
}

void __display_render()
{
	if (SDL_GetWindowFlags(engine.sdl_windowhndl) & SDL_WINDOW_HIDDEN)
		return;

	auto interface = engine.video->interface;

#ifndef IMGUI_DISABLE
	auto imgui = engine.video->imgui;
	if (imgui)
	{
		imgui->imgui_newframe();
		ImGui_ImplSDL2_NewFrame();
	}
	else
		ImGui_ImplNull_NewFrame();
	ImGui::NewFrame();
#endif

	sre::onUpdate.fire();

	// Render current world

	if (sreECS::Scene *current = static_cast<sreECS::Scene*>(engine.current_world))
	{
		engine.video->camera = current->camera.processed_position();

		//// Aliases for the background and the foreground (kind of old)
		const sre::col4& fg = current->foreground;

	#ifndef IMGUI_DISABLE
		ImGui::Begin("Current scene"); {
			float colb[4] = { current->background.r/255.0f, current->background.g/255.0f, current->background.b/255.0f, current->background.a/255.0f };
			if (ImGui::ColorEdit4("Background", colb))
				current->background = sre::col4::fromNormalized(colb[0], colb[1], colb[2], colb[3]);
			
			float colf[4] = { fg.r/255.0f, fg.g/255.0f, fg.b/255.0f, fg.a/255.0f };
			if (ImGui::ColorEdit4("Foreground", colf))
				current->foreground = sre::col4::fromNormalized(colf[0], colf[1], colf[2], colf[3]);
		}
		ImGui::End();
	#endif

		//// Clearing the screen with the background color
		interface->draw_clear(engine.video, &current->background);

		sre::beforeRender.fire();

		//// Drawing all the entities (doesn't run if the foreground is full opaque)
		if (fg.a < 255)
			__render_scene();

		//// Finally, filling the foreground (doesn't run if the foreground is invisible)
		if (fg.a)
			interface->draw_fill(engine.video, reinterpret_cast<const sre_DDFill*>(&fg));
	}
	else
	{
		engine.video->camera.x = 0;
		engine.video->camera.y = 0;

		#if 0 // Debug easier on black backgrounds
			sre::col4 col{50, 50, 50};
		#else
			sre::col4 col = sre::BLACK;
		#endif
		interface->draw_clear(engine.video, &col);
		sre::beforeRender.fire();
	}

	// Drawing the Gui layer
	__render_ui();

	sre::afterRender.fire();

#ifndef IMGUI_DISABLE
	ImGui::Render();
	if (imgui)
		imgui->imgui_renderdrawdata(ImGui::GetDrawData(), engine.video);
#endif
	interface->present(engine.video);
}
