#include "../internal.h"
#include <sdl_renderer/sdl_renderer.h>

#include <Core/Texture.hpp>
#include <Core/Runtime.hpp>

#include <ECS/Scene.hpp>
#include <GUI/Object.hpp>

#include <utils/mem.h>

// ImGUI setup!
	#include <imgui.h>
	#include <backends/imgui_impl_sdl2.cpp> // Compile SDL2 ImGui implementation!
//

void __setup_renderer()
{
	engine.video = static_cast<sre_videodriver*>(operator new(sizeof(sre_videodriver)));
	memset(static_cast<void*>(engine.video), 0, sizeof(sre_videodriver));

	if (!sresdlrenderer_init(engine.video, engine.sdl_windowhndl))
	{
		sre::log<sre::LOGCATEGORY_ERROR>("Failed initializing the render driver");
		exit(-1);
	}
	engine.video->blend(engine.video, SRE_BLEND_BLEND);

	if (engine.video->imgui_init)
	{
		IMGUI_CHECKVERSION();
		ImGuiContext* context = ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard
					   |  ImGuiConfigFlags_NavEnableGamepad;
		
		if (!ImGui_ImplSDL2_InitForOther(engine.sdl_windowhndl) ||
			!engine.video->imgui_init(engine.video))
		{
			sre::log<sre::LOGCATEGORY_ERROR>("Failed initializing ImGui, video driver error");
			ImGui::DestroyContext(context);
		}
	}
	else
		sre::log<sre::LOGCATEGORY_WARN>("ImGui is not implemented in the current video driver");
	

	if (engine.video->texture_size)
	{
		const_cast<void* &>(engine.video->textures) = new sre::byte[SRE_TEXTURE_BASECOUNT * engine.video->texture_size] {};
		const_cast<sre_u32* &>(engine.video->texture_fl) = new sre::u32[SRE_TEXTURE_BASECOUNT] {};
		
		const_cast<size_t &>(engine.video->textures_capacity) = SRE_TEXTURE_BASECOUNT;
		const_cast<size_t &>(engine.video->texture_flcapacity) = SRE_TEXTURE_BASECOUNT;
	}

	if (engine.video->draw_clip)
	{
		const_cast<sre_rect2Dut* &>(engine.video->clipstack_base) = new sre::rect2Dut[SRE_TEXTURE_BASECOUNT]; // I think I'll make both textures and clip rect stacks start at the same size
		const_cast<size_t &>(engine.video->clipstack_size) = SRE_TEXTURE_BASECOUNT;
	}

	engine.video->vsync(engine.video, 1);
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

	if (engine.video->viewport)
		engine.video->viewport(engine.video, w, h);
}

void __display_render()
{
	engine.video->imgui_newframe();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	sre::onUpdate.fire();

	// Render current world

	if (sreECS::Scene *current = static_cast<sreECS::Scene*>(engine.current_world))
	{
		engine.video->camera = current->camera.position;

		//// Aliases for the background and the foreground (kind of old)
		const sre::col4& fg = current->foreground;

		ImGui::Begin("Hello!");
		{
			float col[4] = { fg.r/255.0f, fg.g/255.0f, fg.b/255.0f, fg.a/255.0f };
			ImGui::ColorEdit4("Foreground!", col, ImGuiColorEditFlags_Uint8);
			current->foreground = sre::col4::fromNormalized(col[0], col[1], col[2], col[3]);
		}
		ImGui::End();

		//// Clearing the screen with the background color
		engine.video->draw_clear(engine.video, &current->background);

		sre::beforeRender.fire();

		//// Drawing all the entities (doesn't run if the foreground is full opaque)
		if (fg.a < 255)
			__render_scene();

		//// Finally, filling the foreground (doesn't run if the foreground is invisible)
		if (fg.a)
			engine.video->draw_fill(engine.video, reinterpret_cast<const sre_DDFill*>(&fg));
	}
	else
	{
		engine.video->camera.x = 0;
		engine.video->camera.y = 0;

		engine.video->draw_clear(engine.video, &sre::col4::BLACK);
		sre::beforeRender.fire();
	}

	// Drawing the Gui layer
	__render_ui();

	sre::afterRender.fire();

	ImGui::Render();
	engine.video->imgui_renderdrawdata(ImGui::GetDrawData(), engine.video);
	engine.video->present(engine.video);
}