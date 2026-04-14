#include <Core/Render.h>
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
VIDEOINIT_DEF(gl21)					\
VIDEOINIT_DEF(d3d11)				\
//VIDEOINIT_DEF(opengl)				\
//VIDEOINIT_DEF(d3d12)				\
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
	class DummyInterface: sre::RenderInterface
	{
		virtual void SRE_RENDERCALL flush_queueinstances1(sre::Sampler*const* inst_textures, const sre::RenderInstance1* instances, size_t instance_count, sre::u32 flags) override {}
        virtual void SRE_RENDERCALL flush_queueinstances2(const sre::RenderInstance2& instance, size_t point_count, sre::u32 flags) override {}
            
        virtual void SRE_RENDERCALL present() override {}
        virtual bool SRE_RENDERCALL clear(float color[3]) override { return false; }
    
        // State functions
        virtual bool SRE_RENDERCALL set_viewportstate(int w, int h, sre::unit scale) override { return false; }
        virtual bool SRE_RENDERCALL set_blendstate(sre::blendMode blending) override { return false; }
        virtual bool SRE_RENDERCALL set_camerastate(sre::unit x, sre::unit y) override { return false; }
        virtual void SRE_RENDERCALL set_clipstate(const sre::rect2Di* rectangle) override {}
        virtual void SRE_RENDERCALL set_vsync(bool enable) override {}
                
        // Texture functions
        virtual bool SRE_RENDERCALL setup_texture(sre::Sampler* texture, sre::pixelFormat format, int x, int y) override { return false; }
        virtual bool SRE_RENDERCALL update_texture(sre::Sampler* texture, const void* pixels, int pitch=0) override { return false; }
        virtual bool SRE_RENDERCALL query_texture(sre::Sampler* texture, sre::vec2i* size, sre::pixelFormat* format) override { return false; }
        virtual bool SRE_RENDERCALL destroy_texture(sre::Sampler* texture) override { return false; }
	};

	class CoreRenderer
	{
		public:
			static inline bool set_viewport(int w, int h, sre::unit scale) { return engine.video->set_viewportstate(w, h, scale); }
			static inline void render(float bg[3], sre::vec2ut camoffset);

			static void constructor(sre_RenderInterface* inter) { new(inter) DummyInterface(); }
			static void destructor(sre_RenderInterface* inter) { inter->~RenderInterface(); }
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
		SRE_RENDERDRIVER_OPENGL_21,
		SRE_RENDERDRIVER_DIRECTX_11,

		SRE_RENDERDRIVER_DEFAULT = SRE_RENDERDRIVER_DIRECTX_11
	};

	auto driverdata = video_drivers[SRE_RENDERDRIVER_DEFAULT];
	assert(driverdata.initialize != NULL);
	assert(driverdata.destroy != NULL);
	assert(driverdata.texture_size >= sizeof(int));

	engine.video_quit = driverdata.destroy;
	engine.video = driverdata.initialize(engine.sdl_windowhndl);
	engine.video_tsize = driverdata.texture_size;

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
	sre::vec2ut fsize{ w, h };
	sre::vec2ut size = fsize / scale;
	
	sre::vec2ut center = size / 2.0_ut;

	engine.osize_x = w;
	engine.osize_y = h;
	engine.scale_ratio = 1 / scale;
	engine.scale = scale;
	engine.vsize_x = size.x;
	engine.vsize_y = size.y;
	engine.vcenter_x = center.x;
	engine.vcenter_y = center.y;

	sre::CoreRenderer::set_viewport(w, h, scale);
}

size_t last_renderedframe;

void __display_render()
{
	if (SDL_GetWindowFlags(engine.sdl_windowhndl) & SDL_WINDOW_HIDDEN)
		return;

	last_renderedframe = engine.frame;

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
			engine.video->fill(fg);
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
		sre::vec2ut{engine.vcenter_x, engine.vcenter_y} - (currscn != NULL ? currscn->camera.processed_position() : 0));
}

void sre::CoreRenderer::render(float bg[3], sre::vec2ut camoffset)
{
	engine.video->clear(bg);
	engine.video->set_camerastate(ceil(camoffset.x*engine.scale), ceil(camoffset.y*engine.scale));
	
	engine.video->m_blendmode = -1;
	// Perform flushes
		size_t insti1 = 0;
		size_t insti2 = 0;
		for (auto& queue : engine.video->m_renderqueues)
		{
			if (queue.blendmode != engine.video->m_blendmode)
				engine.video->set_blendstate(static_cast<sre::blendMode>(queue.blendmode));

			switch (queue.type)
			{
			case 1:
				assert(insti1 + queue.count <= engine.video->m_rinst1cache.size());
				assert(insti1 + queue.count <= engine.video->m_texturecache.size());
				engine.video->flush_queueinstances1(&engine.video->m_texturecache.at(insti1),
													&engine.video->m_rinst1cache.at(insti1),
													queue.count,
													queue.flags
												);
				insti1 += queue.count;
				break;
			case 2:
				engine.video->flush_queueinstances2(
					reinterpret_cast<RenderInstance2&>(engine.video->m_rinst2cache.at(insti2)),
					queue.count,
					queue.flags
				);
				insti2 += queue.count*sizeof(RenderInstance2::points[0]) + sizeof(RenderInstance2::color) + sizeof(RenderInstance2::mode);
			default:
				break;
			}
		}
		engine.video->m_rinst1cache.clear();
		engine.video->m_rinst2cache.clear();
		engine.video->m_texturecache.clear();
		engine.video->m_renderqueues.clear();

		engine.video->m_blendmode = SRE_BLEND_DEFAULT;
	//

	#ifndef IMGUI_DISABLE
	ImGui::Render();
	if (imgui)
		imgui->imgui_renderdrawdata(ImGui::GetDrawData(), engine.video);
	#endif

	engine.video->present();
}

// All of sre::RenderInterface's code

sre::RenderInterface* sre::get_renderer() { assert(engine.video != NULL); return engine.video; }

void sre::RenderInterface::clip_reset() { set_clipstate(NULL); }

void sre::RenderInterface::clip_set(sre::rect2Dut zone)
{
	sre::rect2Di rect{
		static_cast<int>(zone.position.x * engine.scale),
		static_cast<int>(zone.position.y * engine.scale),
		static_cast<int>(zone.size.x * engine.scale),
		static_cast<int>(zone.size.y * engine.scale)
	};
	set_clipstate(&rect);
}

void sre::RenderInterface::draw1(sre::flags32 flags, const RenderInstance1 instances[], size_t instcount, Sampler*const samplers[])
{
	m_rinst1cache.insert(m_rinst1cache.end(), instances, instances + instcount);
	if (samplers)
		m_texturecache.insert(m_texturecache.end(), samplers, samplers + instcount); // Some textures can be NULL, in this case, either let the driver do the work, or do it ourselves with a basic texture
	else
		m_texturecache.insert(m_texturecache.end(), instcount, NULL);
	
	auto* back = m_renderqueues.empty() ? NULL : &m_renderqueues.back();
	if (!back || back->type != 1 || back->flags != flags || back->blendmode != m_blendmode)
	{
		m_renderqueues.push_back({
			instcount,
			1,
			m_blendmode,
			flags
		});
	}
	else
		back->count+= instcount;
}

void sre::RenderInterface::draw2(sre::flags32 flags, sre::col4 color, const sre::vec2ut points[], size_t pcount, sre::draw2Mode mode)
{
	sre::RenderInstance2 dummy{
		color,
		mode
	};
	m_rinst2cache.insert(m_rinst2cache.end(), reinterpret_cast<sre::byte*>(&dummy), reinterpret_cast<sre::byte*>(&dummy.points));
	m_rinst2cache.insert(m_rinst2cache.end(), reinterpret_cast<const sre::byte*>(points), reinterpret_cast<const sre::byte*>(points + pcount));
	m_renderqueues.push_back({
		pcount,
		2,
		m_blendmode,
		flags
	});
}

sre::RenderInterface::RenderInterface()
{
}

sre::RenderInterface::~RenderInterface()
{
}

extern "C" void sre_RIconstructor(sre_RenderInterface* inter) { sre::CoreRenderer::constructor(inter); }
extern "C" void sre_RIdestructor(sre_RenderInterface* inter) { sre::CoreRenderer::destructor(inter); }