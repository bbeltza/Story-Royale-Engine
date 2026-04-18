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

namespace sre
{
	struct RenderQueue
	{
	    size_t count;
	    sre::Sampler* texture;
	    char type; // 1 for RenderInstance1, or 2 for RenderInstance2
	    short blendmode;
	    sre::flags32 flags;
		sre::rect2Di clip_rect;
	};

	struct RenderVectors // What _vector_data in engine.video contains
	{
		std::vector<RenderInstance1> rinst1cache;
		std::vector<sre::byte> rinst2cache; // sre::byte to be more like a point buffer. RenderInstance2's size varies
            
        // Render queue, what will be inserted in the draw functions
        std::vector<RenderQueue> renderqueues;
	};
}

#if _WIN32
	#define VIDEOINIT_DEF_WIN32 VIDEOINIT_DEF
#else
	#define VIDEOINIT_DEF_WIN32(x)
#endif

#define VIDEO_DRIVERS				\
VIDEOINIT_DEF(sdlrenderer)			\
VIDEOINIT_DEF(gl11)					\
VIDEOINIT_DEF(gl21)					\
VIDEOINIT_DEF(gl32)					\
VIDEOINIT_DEF_WIN32(d3d11)			\
//VIDEOINIT_DEF_WIN32(d3d12)		\

#define VIDEOINIT_DEF(x) extern "C" const sre_RenderDriverData sre##x;
VIDEO_DRIVERS
#undef VIDEOINIT_DEF

#define VIDEOINIT_DEF(x) sre##x,
static const sre_RenderDriverData video_drivers[] = {
	VIDEO_DRIVERS
};

extern "C" void __cleanup_textures();

void __cleanup_renderer()
{
	__cleanup_textures();
	(*engine.video.vfptr)->destructor(engine.video.vfptr+1);
	operator delete(engine.video.vfptr);
	engine.video.vfptr = NULL;
}

void __setup_renderer()
{
	enum sre_RenderDrivers
	{
		SRE_RENDERDRIVER_SDLRENDERER,
		SRE_RENDERDRIVER_OPENGL_11,
		SRE_RENDERDRIVER_OPENGL_21,
		SRE_RENDERDRIVER_OPENGL_32,
		#if _WIN32
			SRE_RENDERDRIVER_DIRECTX_11,
			SRE_RENDERDRIVER_DEFAULT = SRE_RENDERDRIVER_DIRECTX_11
		#else
			SRE_RENDERDRIVER_DEFAULT = SRE_RENDERDRIVER_OPENGL_11
		#endif
	};

	auto driverdata = video_drivers[SRE_RENDERDRIVER_DEFAULT];
	assert(driverdata.initialize != NULL);
	assert(driverdata.renderer_size >= sizeof(void*));
	assert(driverdata.texture_size >= sizeof(int));

	engine.video.vfptr = static_cast<const sre_RenderVFT**>(operator new(sizeof(void*) + driverdata.renderer_size));
	*engine.video.vfptr = NULL;
	if (!driverdata.initialize(engine.video.vfptr, engine.video.vfptr+1, engine.sdl_windowhndl))
	{
		operator delete(engine.video.vfptr);
		sre::log<sre::LOGCATEGORY_ERROR>("Failed initializing the render driver");
		exit(-1);
	}

	assert((*engine.video.vfptr) != NULL && "Maybe forgot to set up the vft? (virtual function table)");

	new(engine.video._vector_data) sre::RenderVectors{};
	engine.video.texture_size = driverdata.texture_size;

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

	SRE_VIDEO(engine.video.vfptr, set_viewportstate, w, h, scale);
}

size_t last_renderedframe;

static void flush_rendercmds(float bg[3], sre::vec2ut camoffset);

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

	sre::render_clipreset();

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
			sre::render_fill(fg);
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

	flush_rendercmds(bg, 
		sre::vec2ut{engine.vcenter_x, engine.vcenter_y} - (currscn != NULL ? currscn->camera.processed_position() : 0));
}

#define m reinterpret_cast<sre::RenderVectors&>(engine.video._vector_data)

static void flush_rendercmds(float bg[3], sre::vec2ut camoffset)
{
	using namespace sre;

	SRE_VIDEO(engine.video.vfptr, clear, bg);
	SRE_VIDEO(engine.video.vfptr, set_camerastate, ceil(camoffset.x*engine.scale), ceil(camoffset.y*engine.scale));
	
	engine.video.blendmode = -1;
	// Perform flushes
		size_t insti1 = 0;
		size_t insti2 = 0;

		static const sre::RenderQueue _DUMMY_QUEUE = {
			SIZE_MAX,
			reinterpret_cast<sre::Sampler*>(PTRDIFF_MAX),
			INT8_MAX,
			INT16_MAX,
			UINT32_MAX
		};

		const auto* last_queue = &_DUMMY_QUEUE;
		for (auto& queue : m.renderqueues)
		{
			if (queue.blendmode != engine.video.blendmode)
			{
				SRE_VIDEO(engine.video.vfptr, set_blendstate, static_cast<sre::blendMode>(queue.blendmode));
				engine.video.blendmode = queue.blendmode;
			}
			if (queue.clip_rect != engine.video.clip_rect)
			{
				SRE_VIDEO(engine.video.vfptr, set_clipstate, &queue.clip_rect);
				engine.video.clip_rect = queue.clip_rect;
			}

			sre::u32 switchflags = (queue.type != last_queue->type ? SRE_RENDER_SWITCHTYPE : 0)
								 | ((queue.flags&SRE_DRAWFLAG_CAMERA) != (last_queue->flags&SRE_DRAWFLAG_CAMERA) || last_queue == &_DUMMY_QUEUE ? SRE_RENDER_SWITCHCAMERA : 0 )
								 | (queue.texture != last_queue->texture ? SRE_RENDER_SWITCHTEXTURE : 0);
			queue.texture = queue.texture ? queue.texture+1 : NULL;

			last_queue = &queue;
			switch (queue.type)
			{
			case 1:
				assert(insti1 + queue.count <= m.rinst1cache.size());
				SRE_VIDEO(engine.video.vfptr,
							flush_queueinstances1,
							queue.texture,
							&m.rinst1cache.at(insti1),
							queue.count,
							queue.flags,
							switchflags
						);
				insti1 += queue.count;
				break;
			case 2:
				SRE_VIDEO(engine.video.vfptr,
							flush_queueinstances2,
							queue.texture,
							&reinterpret_cast<RenderInstance2&>(m.rinst2cache.at(insti2)),
							queue.count,
							queue.flags,
							switchflags
				);
				insti2 += queue.count*sizeof(RenderInstance2::points[0]) + sizeof(RenderInstance2::color) + sizeof(RenderInstance2::mode);
			default:
				break;
			}
		}
		m.rinst1cache.clear();
		m.rinst2cache.clear();
		m.renderqueues.clear();

		engine.video.blendmode = SRE_BLEND_DEFAULT;
	//

	#ifndef IMGUI_DISABLE
	ImGui::Render();
	if (imgui)
		imgui->imgui_renderdrawdata(ImGui::GetDrawData(), engine.video);
	#endif

	SRE_VIDEOV(engine.video.vfptr, present);
}

// All sre::render_ functions

void sre::render_clipreset() { engine.video.clip_rect = {0, 0, engine.osize_x, engine.osize_y}; }

void sre::render_clipset(sre::rect2Dut zone)
{
	engine.video.clip_rect = {
		static_cast<int>(zone.position.x * engine.scale),
		static_cast<int>(zone.position.y * engine.scale),
		static_cast<int>(zone.size.x * engine.scale),
		static_cast<int>(zone.size.y * engine.scale)
	};
}

void sre::render_draw1(sre::flags32 flags, const RenderInstance1 instances[], size_t instcount, Sampler* texture)
{
	m.rinst1cache.insert(m.rinst1cache.end(), instances, instances + instcount);
	
	auto* back = m.renderqueues.empty() ? NULL : &m.renderqueues.back();
	if (!back || back->texture != texture || back->type != 1 || back->flags != flags || back->blendmode != engine.video.blendmode || back->clip_rect != engine.video.clip_rect)
	{
		m.renderqueues.push_back({
			instcount,
			texture,
			1,
			engine.video.blendmode,
			flags,
			engine.video.clip_rect
		});
	}
	else
		back->count+= instcount;
}

void sre::render_draw2(sre::flags32 flags, sre::col4 color, const sre::RenderPoint points[], size_t pcount, sre::draw2mode mode, sre::Sampler* texture)
{
	sre::RenderInstance2 dummy{
		mode,
		color
	};
	m.rinst2cache.insert(m.rinst2cache.end(), reinterpret_cast<sre::byte*>(&dummy), reinterpret_cast<sre::byte*>(&dummy.points));
	m.rinst2cache.insert(m.rinst2cache.end(), reinterpret_cast<const sre::byte*>(points), reinterpret_cast<const sre::byte*>(points + pcount));
	m.renderqueues.push_back({
		pcount,
		texture,
		2,
		engine.video.blendmode,
		flags,
		engine.video.clip_rect
	});
}

void sre::render_blend(sre::blendMode blend) { engine.video.blendmode = blend; }

// C wrappers

extern "C" void sre_render_clipreset() { sre::render_clipreset(); }
extern "C" void sre_render_clipset(const sre_rect2Dut* zone) { sre::render_clipset(*zone); }

extern "C" void sre_render_blend(sre_blendMode blend) { sre::render_blend(blend); }

extern "C" void sre_render_draw1(sre_u32 flags, const sre_RenderInstance1 instances[], size_t instcount, sre_Sampler* texture) { sre::render_draw1(flags, instances, instcount, texture); }
extern "C" void sre_render_draw2(sre_u32 flags, const sre_col4* color, const sre_RenderPoint points[], size_t pcount, sre_draw2mode mode, sre_Sampler* texture) { sre::render_draw2(flags, *color, points, pcount, mode, texture); }

extern "C" void sre_render_fill(const sre_col4* color) { sre::render_fill(*color); }