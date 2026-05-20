#include <Core/Render.h>
#include <Core/Runtime.hpp>
#include <Base/Error.h>
#include <Base/Log.h>

#include "../internal.h"

#include <ECS/Scene.hpp>
#include <GUI/Object.hpp>
#include <Hints.h>

#include <utils/mem.h>

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

#define NAME_RENDERDRIVER(x) sre##x
#define DECL_RENDERDRIVER(x) extern "C" const sre_RenderDriverData NAME_RENDERDRIVER(x)

DECL_RENDERDRIVER(sdlrenderer);
DECL_RENDERDRIVER(gl11);
DECL_RENDERDRIVER(gl21);
DECL_RENDERDRIVER(gl32);
DECL_RENDERDRIVER(d3d9);
DECL_RENDERDRIVER(d3d11);
DECL_RENDERDRIVER(d3d12);

static const sre_RenderDriverData* render_drivers[] = {
	#if _WIN32
		&NAME_RENDERDRIVER(d3d11),
		&NAME_RENDERDRIVER(d3d9),
	#endif
	&NAME_RENDERDRIVER(gl32),
	&NAME_RENDERDRIVER(gl21),
	&NAME_RENDERDRIVER(gl11),
	&NAME_RENDERDRIVER(sdlrenderer)

	#ifdef SRE_USEDRIVER_D3D12
	,	&NAME_RENDERDRIVER(d3d12),
	#endif
};

static void render_panic()
{
	sre::log("Press any key to continue...", SRE_LOG_INFO);
	getchar();
	exit(-1);
}

static int render_setupenv(const sre_RenderDriverData* driver)
{
	assert(driver->initialize != NULL);
	assert(driver->renderer_size >= sizeof(void*));
	assert(driver->texture_size >= sizeof(int));

	operator delete(engine.video.driver);
	engine.video.driver = static_cast<decltype(engine.video.driver)>(operator new(sizeof(void*) + driver->renderer_size));
	engine.video.driver->vfptr = NULL;

	return driver->initialize(&engine.video.driver->vfptr, engine.video.driver->driverdata, engine.sdl_windowhndl);
}

static const sre_RenderDriverData* render_choosedriver(int hint)
{
	const sre_RenderDriverData* driver = NULL;
	switch (hint)
	{
		case SRE_RENDERDRIVER_SDLRENDERER: driver = &sresdlrenderer; break;
		case SRE_RENDERDRIVER_OPENGL_11: driver = &sregl11; break;
		case SRE_RENDERDRIVER_OPENGL_21: driver = &sregl21; break;
		case SRE_RENDERDRIVER_OPENGL_32: driver = &sregl32; break;

	#ifndef _WIN32
		case SRE_RENDERDRIVER_DIRECTX_9:
		case SRE_RENDERDRIVER_DIRECTX_11:
		case SRE_RENDERDRIVER_DIRECTX_12:
			sre::error(SRE_ERR_CORE, "DirectX render drivers are only supported on Windows");
			break;
	#else
		case SRE_RENDERDRIVER_DIRECTX_9: driver = &sred3d9; break;
		case SRE_RENDERDRIVER_DIRECTX_11: driver = &sred3d11; break;
		case SRE_RENDERDRIVER_DIRECTX_12:
		#ifdef SRE_USEDRIVER_D3D12
			driver = &sred3d12;
		#else
			sre::error(SRE_ERR_CORE, "Direct3D12 is not supported in the current build");
		#endif
			break;
	#endif
		case -1: break;
		default:
			sre::error(SRE_ERR_INVALID_HINT, "RENDERDRIVER");
			break;
	}

	if (driver)
	{
		int status = render_setupenv(driver);
		switch (status)
		{
			case SRE_RENDERSTATUS_SUCCEEDED: goto SUCCEED;
			case SRE_RENDERSTATUS_FAILED: goto FAIL;
			case SRE_RENDERSTATUS_UNSUPPORTED:
				sre::log(SRE_LOG_WARN "Render driver '%s' is declared as the hint, but was found unsupported. Switching to the default drivers...", driver->name);
				break;
			default:
				sre::log(SRE_LOG_WARN "Unknown status: '%d'. Failing...", status);
				goto FAIL;
		}
	}

	for (int i = 0; i < ut_arrcount(render_drivers); i++)
	{
		driver = render_drivers[i];
		int status = render_setupenv(driver);
		switch (status)
		{
			case SRE_RENDERSTATUS_SUCCEEDED: goto SUCCEED;
			case SRE_RENDERSTATUS_FAILED: goto FAIL;
			case SRE_RENDERSTATUS_UNSUPPORTED:
				sre::log(SRE_LOG_WARN "Render driver '%s' is unsupported", driver->name);
				break;
			default:
				sre::log(SRE_LOG_WARN "Unknown status: '%d'. Failing...", status);
				goto FAIL;
		}
	}

		sre::error(SRE_ERR_CORE, "Could not find any supported video drivers... This should not happen assuming that every piece of hardware supports SDL Renderer with its software fallback...");
		return NULL;
	SUCCEED:
		sre::log(SRE_LOG_INFO "Using render driver: %s", driver->name);
		return driver;
	FAIL:
		sre::log(SRE_LOG_INFO "Driver initialization for '%s' failed!", driver->name);
		return NULL;
}

static void __shutdown_imgui();
static void __endframe_imgui();

extern "C" void __cleanup_textures();

void __cleanup_renderer()
{
	if (engine.imgui)
		__shutdown_imgui();

	__cleanup_textures();
	SRE_VIDEOV(engine.video.driver, destructor);
	operator delete(engine.video.driver);
	engine.video.driver = NULL;
}

void __setup_renderer()
{
	int renderhint = -1;
	const sre_RenderDriverData* driverdata = NULL;
	{
		auto externalhint = static_cast<const sre_RenderDriverData*const*>(sre::gethint("EXTERN_RENDERDRIVER"));
		if (externalhint)
		{
			driverdata = *externalhint;
			if (!driverdata)
				sre::critical(SRE_ERR_CORE, "SRE_HINT_EXTERN_RENDERDRIVER is NULL...");

			sre::log("FOUND external render driver: \"%s\"", driverdata->name);
			int status = render_setupenv(driverdata);
			
			if (status == SRE_RENDERSTATUS_FAILED)
				sre::critical(SRE_ERR_FAIL, "External render driver failed to initialize!");

			if (status == SRE_RENDERSTATUS_UNSUPPORTED)
			{
				sre::log(SRE_LOG_WARN "External render driver '%s' is unsupported", driverdata->name);
				driverdata = NULL;
			}
		}

		// Look for the existing render drivers now
		if (!driverdata)
		{
			const void* hint = sre::gethint("RENDERDRIVER");
			if (hint)
			{
				unsigned hintvalue = *static_cast<const unsigned*>(hint);
				renderhint = hintvalue;
			}
			
			driverdata = render_choosedriver(renderhint);
		}
		else
			engine.video.index = -1;
	}

	if (!driverdata)
		render_panic();

	assert(engine.video.driver->vfptr != NULL && "Maybe forgot to set up the vft? (virtual function table)");

	#define _CHECKFORFUNC(x) if (!engine.video.driver->vfptr->x) { sre::error(SRE_ERR_CORE, "Render driver setup error: function '" #x "' is missing"); render_panic(); }
		_CHECKFORFUNC(destructor);
		_CHECKFORFUNC(flush_queueinstances1);
		_CHECKFORFUNC(flush_queueinstances2);
		_CHECKFORFUNC(present);
		_CHECKFORFUNC(clear);
		_CHECKFORFUNC(set_viewportstate);
		_CHECKFORFUNC(set_blendstate);
		_CHECKFORFUNC(set_camerastate);
		_CHECKFORFUNC(set_clipstate);
		_CHECKFORFUNC(set_vsync);
		_CHECKFORFUNC(texture_setup);
		_CHECKFORFUNC(texture_update);
		_CHECKFORFUNC(texture_destroy);
	#undef _CHECKFORFUNC

	new(engine.video._vector_data) sre::RenderVectors{};
	engine.video.texture_size = driverdata->texture_size;
	engine.video.blendmode = SRE_BLEND_DEFAULT;

	if (engine.video.index == 0)
	{
		engine.video.index = -1;
		for (int i = 0; i < ut_arrcount(render_drivers); i++) // Very fancy
		{
			if (render_drivers[i] == driverdata)
			{
				engine.video.index = i;
				break;
			}
		} assert(engine.video.index >= 0);
	}

	engine.scale = 1;

	SRE_VIDEO(engine.video.driver, set_vsync, true);
}

void __update_viewport(int w, int h)
{
	int integer_scale;

	if (engine.osize_x == w && engine.osize_y == h) // Have to check for any scale changes
		return;

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

	SRE_VIDEO(engine.video.driver, set_viewportstate, w, h, scale);
}

#define m reinterpret_cast<sre::RenderVectors&>(engine.video._vector_data)

void __render_flush()
{
	assert(SDL_GetWindowFlags(engine.sdl_windowhndl) & SDL_WINDOW_SHOWN);

	if (!engine.video.wantclear && m.renderqueues.empty()) return;
	engine.video.wantclear = false;

	using namespace sre;

	float bg[4] = {0, 0, 0, 1};
	if (engine.current_world)
	{
		bg[0] = currscn->background.r / 255.0f;
		bg[1] = currscn->background.g / 255.0f;
		bg[2] = currscn->background.b / 255.0f;
	}

	sre::vec2ut camoffset = sre::vec2ut{engine.vcenter_x, engine.vcenter_y} - (currscn != NULL ? currscn->camera.processed_position() : 0);
	camoffset = (camoffset*engine.scale).ceil();

	SRE_VIDEO(engine.video.driver, clear, bg);
	SRE_VIDEO(engine.video.driver, set_camerastate, camoffset.x, camoffset.y);
	
	engine.video.blendmode = -1;
	engine.video.clip_rect = { 0, 0 };
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
				SRE_VIDEO(engine.video.driver, set_blendstate, static_cast<sre::blendMode>(queue.blendmode));
				engine.video.blendmode = queue.blendmode;
			}
			if (queue.clip_rect != engine.video.clip_rect)
			{
				SRE_VIDEO(engine.video.driver, set_clipstate, &queue.clip_rect);
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
				SRE_VIDEO(engine.video.driver,
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
				SRE_VIDEO(engine.video.driver,
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

	if (engine.imgui)
		__endframe_imgui();

	SRE_VIDEOV(engine.video.driver, present);
}

// All sre::render_ functions

void sre::render_clipreset() { engine.video.clip_rect = {0, 0, engine.osize_x, engine.osize_y}; }

void sre::render_clipset(sre::rect2Dut zone)
{
	engine.video.clip_rect = {
		static_cast<int>(ceil(zone.position.x * engine.scale)),
		static_cast<int>(ceil(zone.position.y * engine.scale)),
		static_cast<int>(ceil(zone.size.x * engine.scale)),
		static_cast<int>(ceil(zone.size.y * engine.scale))
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

// ImGui setup

#include <ImGui.hpp>

extern "C" void __initialize_imgui(void* _imgui)
{
	sre::ImGuiInterface* imgui_interface = static_cast<sre::ImGuiInterface*>(_imgui);

	int status = imgui_interface->initialize(engine.sdl_windowhndl, engine.video.driver->driverdata, engine.video.index);
	switch (status)
	{
		case SRE_RENDERSTATUS_SUCCEEDED: break;
		case SRE_RENDERSTATUS_UNSUPPORTED:
			sre::logmsg("ImGui is unsupported in the current render driver", SRE_LOG_WARN);
			return;
		case SRE_RENDERSTATUS_FAILED:
			sre::critical(SRE_ERR_CORE, "Failed initializing ImGui");
			return;
		default:
			abort();
	}

	sre::logmsg("ImGui has successfully been initialized!", SRE_LOG_INFO);
	engine.imgui = imgui_interface;
}

extern "C" bool __onevent_imgui(SDL_Event* ev) { assert(engine.imgui != NULL); return SRE_pIMGUI->on_event(ev); }

static void __shutdown_imgui() { assert(engine.imgui != NULL); SRE_pIMGUI->shutdown(); }
static void __endframe_imgui() { assert(engine.imgui != NULL); SRE_pIMGUI->end_frame(); }
void __beginframe_imgui() { assert(engine.imgui != NULL); SRE_pIMGUI->begin_frame(); }