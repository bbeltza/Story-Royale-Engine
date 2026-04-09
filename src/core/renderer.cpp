#include <Core/Render.h>
#include <Core/Texture.hpp>
#include <Core/Runtime.hpp>

#include "../internal.h"

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
//VIDEOINIT_DEF(opengl)				\
//VIDEOINIT_DEF(d3d12)				\
//VIDEOINIT_DEF(d3d11)				\
//VIDEOINIT_DEF(softwarerender)

#define VIDEOINIT_DEF(x) extern "C" const sre::RenderDriverData sre##x;
VIDEO_DRIVERS
#undef VIDEOINIT_DEF

#define VIDEOINIT_DEF(x) sre##x,
static const sre::RenderDriverData video_drivers[] = {
	VIDEO_DRIVERS
};

namespace sre
{
	class CoreRenderer
	{
		public:
			static inline bool set_viewport(int w, int h, sre::unit scale) { return engine.video->set_viewportstate(w, h, scale); }
			
			static inline void render(float bg[3], sre::vec2ut camoffset);
	};
}

void __cleanup_renderer()
{
	engine.video_quit(engine.video);
	/*
	delete[] engine.video->texture_fl;
	delete[] engine.video->clipstack_base;
	operator delete (engine.video->textures);
	operator delete (engine.video);
	*/

	engine.video = NULL;
}

void __setup_renderer()
{
	enum sre_RenderDrivers
	{
		SRE_RENDERDRIVER_SDLRENDERER,

		SRE_RENDERDRIVER_DEFAULT = SRE_RENDERDRIVER_SDLRENDERER
	};

	auto driverdata = video_drivers[SRE_RENDERDRIVER_DEFAULT];
	assert(driverdata.initialize != NULL);
	assert(driverdata.destroy != NULL);
	assert(driverdata.texture_size >= sizeof(int));

	engine.video_quit = driverdata.destroy;
	engine.video = driverdata.initialize(engine.sdl_windowhndl);

	if (!engine.video)
	{
		sre::log<sre::LOGCATEGORY_ERROR>("Failed initializing the render driver");
		exit(-1);
	}

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
		integer_scale = static_cast<int>(engine.scale);
	}

	sre::unit scale = static_cast<sre::unit>(integer_scale);
	sre::vec2ut size = sre::vec2ut{ w, h } / scale;
	
	sre::vec2ut center = size / 2.0_ut;

	engine.osize_x = w;
	engine.osize_y = h;
	engine.scale_ratio = 1 / scale;
	engine.scale = scale;
	engine.vsize_x = size.x;
	engine.vsize_y = size.y;
	engine.vcenter_x = center.y;
	engine.vcenter_y = center.y;

	sre::CoreRenderer::set_viewport(w, h, scale);
}

void __display_render()
{
	if (SDL_GetWindowFlags(engine.sdl_windowhndl) & SDL_WINDOW_HIDDEN)
		return;

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

	float bg[3];
	if (sreECS::Scene *current = static_cast<sreECS::Scene*>(engine.current_world))
	{
		bg[0] = current->background.r / 255.0f;
		bg[1] = current->background.g / 255.0f;
		bg[2] = current->background.b / 255.0f;

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

		sre::beforeRender.fire();

		//// Drawing all the entities (doesn't run if the foreground is full opaque)
		if (fg.a < 255)
			__render_scene();

		//// Finally, filling the foreground (doesn't run if the foreground is invisible)
		if (fg.a)
			engine.video->draw1(0, { {  } });
	}
	else
	{
		#if 0 // Debug easier on black backgrounds
			bg[0] = 0.3f;
			bg[1] = 0.3f;
			bg[2] = 0.3f;
		#else
			bg[0] = 0.0f;
			bg[1] = 0.0f;
			bg[2] = 0.0f;
		#endif
		sre::beforeRender.fire();
	}

	// Drawing the Gui layer
	__render_ui();

	sre::afterRender.fire();

	sre::CoreRenderer::render(bg,
		engine.current_world ? sre::vec2ut{ engine.vcenter_x, engine.vcenter_y } - currscn->camera.processed_position() : 0);
}

void sre::CoreRenderer::render(float bg[3], sre::vec2ut camoffset)
{
	engine.video->clear(bg);
	engine.video->set_camerastate(camoffset.x, camoffset.y);
	
	// Perform flushes
		for (auto& queue : engine.video->m_renderqueues)
		{
			
		}
		engine.video->m_renderqueues.clear();
	//

	#ifndef IMGUI_DISABLE
	ImGui::Render();
	if (imgui)
		imgui->imgui_renderdrawdata(ImGui::GetDrawData(), engine.video);
	#endif

	engine.video->present();
}