#include <Core/Render.h>
#include <Core/Display.hpp>
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
	enum RenderCmdType
	{
		CMD_BEGIN,
		CMD_DRAW1,
		CMD_DRAW2,

		CMD_STATE_SCISSOR,
		CMD_STATE_TEXTURE,
		CMD_STATE_CAMERA,
		CMD_STATE_BLEND
	};

	struct RenderCmd
	{
		RenderCmd(RenderCmdType type): type(type) {};
		RenderCmd(const RenderCmd& cpy) {
			SDL_memcpy(this, &cpy, sizeof(RenderCmd));
		}

		RenderCmdType type;
		union {
			struct {
				float clear[4];
			} begin;
			struct {
				// Positions from either `rinst1cache` or `rinst2cache` depending on `type`.
				// If `type` is CMD_DRAW1, then the units will be in `RenderInstance1` elements from `rinst1cache`
				// If `type` is CMD_DRAW2, then the units will be in BYTES, from `rinst2cache`
				// If it's none of them, then don't use this struct, the contents of it are undetermined 

				size_t begin;
				size_t size;
			} draw;
			struct {
				sre::rect2Di rect;
			} scissor;
			struct {
				void* texture;
			} texture;
			struct {
				sre::unit x;
				sre::unit y;
			} camera;
			struct {
				sre::blendMode mode;
			} blend;
		};
	};

	struct RenderVectors // What _vector_data in engine.video contains
	{
		std::vector<RenderInstance1> rinst1cache;
		std::vector<sre::byte> rinst2cache; // sre::byte to be more like a point buffer. RenderInstance2's size varies
            
        // Render cmd, what will be inserted in the draw functions
        std::vector<RenderCmd> rendercmds;
	};
}

#define NAME_RENDERDRIVER(x) sre##x
#define DECL_RENDERDRIVER(x) extern "C" const sre_RenderDriverData NAME_RENDERDRIVER(x)

DECL_RENDERDRIVER(swrender);
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
	&NAME_RENDERDRIVER(sdlrenderer),
	&NAME_RENDERDRIVER(swrender)

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
		case SRE_RENDERDRIVER_SOFTWARE: driver = &sreswrender; break;
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
		_CHECKFORFUNC(draw1);
		_CHECKFORFUNC(draw2);
		_CHECKFORFUNC(begin);
		_CHECKFORFUNC(end);
		_CHECKFORFUNC(set_viewportstate);
		_CHECKFORFUNC(set_vsync);
		_CHECKFORFUNC(set_texturestate);
		_CHECKFORFUNC(set_blendstate);
		_CHECKFORFUNC(set_camerastate);
		_CHECKFORFUNC(set_scissorstate);
		_CHECKFORFUNC(texture_setup);
		_CHECKFORFUNC(texture_update);
		_CHECKFORFUNC(texture_destroy);	
	#undef _CHECKFORFUNC

	new(engine.video._vector_data) sre::RenderVectors{};
	engine.video.texture_size = driverdata->texture_size;
	engine.video.flags = driverdata->flags;
	engine.video.state = {};
	engine.video.state.currentvsync = true;

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
	if (m.rendercmds.empty()) return;

	using namespace sre;

	// Perform flushes
		bool hasbegun = false;

		for (const auto& cmd : m.rendercmds)
		{
			if (!hasbegun)
			{
				assert(cmd.type == CMD_BEGIN && "The first command MUST be CMD_BEGIN");

				if (engine.video.state.desiredvsync != engine.video.state.currentvsync)
				{
					SRE_VIDEO(engine.video.driver, set_vsync, engine.video.state.desiredvsync);
					engine.video.state.currentvsync = engine.video.state.desiredvsync;
				}
				
				SRE_VIDEO(engine.video.driver, begin, cmd.begin.clear);
				SRE_VIDEO(engine.video.driver, set_blendstate, SRE_BLEND_DEFAULT);
				SRE_VIDEO(engine.video.driver, set_camerastate, 0, 0);
				hasbegun = true;
				continue;
			}

			switch (cmd.type)
			{
				case CMD_DRAW1:
					SRE_VIDEO(engine.video.driver, draw1,
						&m.rinst1cache.at(cmd.draw.begin),
						cmd.draw.size
					);
					break;
				case CMD_DRAW2:
					SRE_VIDEO(engine.video.driver, draw2,
						&reinterpret_cast<const sre_RenderInstance2&>(m.rinst2cache.at(cmd.draw.begin)),
						cmd.draw.size
					);
					break;
				case CMD_STATE_SCISSOR:
					SRE_VIDEO(engine.video.driver, set_scissorstate,
						&cmd.scissor.rect
					);
					break;
				case CMD_STATE_TEXTURE:
					SRE_VIDEO(engine.video.driver, set_texturestate,
						cmd.texture.texture
					);
					break;
				case CMD_STATE_CAMERA:
					SRE_VIDEO(engine.video.driver, set_camerastate,
						cmd.camera.x,
						cmd.camera.y
					);
					break;
				case CMD_STATE_BLEND:
					SRE_VIDEO(engine.video.driver, set_blendstate,
						cmd.blend.mode
					);
					break;
				default:
					assert(0 && "Unknown command type! Or command type is CMD_STATE_BEGIN");
			}
		}

		m.rinst1cache.clear();
		m.rinst2cache.clear();
		m.rendercmds.clear();
	//

	if (engine.imgui)
		__endframe_imgui();

	SRE_VIDEOV(engine.video.driver, end);
}

// All sre::render_ functions
bool sre::render::has_begun() {
	return !m.rendercmds.empty();
}
bool sre::render::begin(sre::col4 clear, sre::vec2ut camera)
{
	if (has_begun())
		return false;

	sre::vec2ut center = sre::display_center();
	engine.video.state.camera_x = ceil((-camera.x + center.x) * engine.scale);
	engine.video.state.camera_y = ceil((-camera.y + center.y) * engine.scale);
	engine.video.state.texture = reinterpret_cast<sre_Texture*>(UINTPTR_MAX);
	engine.video.state.blendmode = SRE_BLEND_DEFAULT;
	engine.video.state.lastdraw_flags = 0;

	RenderCmd cmd{ CMD_BEGIN };
	cmd.begin.clear[0] = clear.r / 255.0f;
	cmd.begin.clear[1] = clear.g / 255.0f;
	cmd.begin.clear[2] = clear.b / 255.0f;
	cmd.begin.clear[3] = 1.0f;
	m.rendercmds.emplace_back(cmd);
	return true;
}

#define RENDERCMDCHECK() assert(m.rendercmds.size() != 0 && "MUST call sre::render::begin before calling any render function!!")

void sre::render::set_vsync(bool enable) {
	engine.video.state.desiredvsync = enable;
}
bool sre::render::set_scissors(sre::rect2Dut zone) {
	if (engine.video.state.scissor == zone)
		return false;
	
	assert(zone.size.x >= 0);
	assert(zone.size.y >= 0);
	assert(zone.position.x >= 0);
	assert(zone.position.y >= 0);

	RENDERCMDCHECK();

	engine.video.state.scissor = zone;
	RenderCmd cmd{ CMD_STATE_SCISSOR };
	cmd.scissor.rect = {
		static_cast<int>(zone.position.x * engine.scale ),
		static_cast<int>(zone.position.y * engine.scale ),
		static_cast<int>(zone.size.x * engine.scale ),
		static_cast<int>(zone.size.y * engine.scale )
	};
	m.rendercmds.emplace_back(cmd);
	return true;
}
bool sre::render::set_blendmode(sre::blendMode mode)
{
	// TODO: Blend mode implementations should be reliant on blending src, dst operation parameters, etc...
	// 	 and not just a single blending enum. This would reduce backend overhead more. This enum will thus just become a wrapper around
	// 	 the aforementioned more advanced future blending function
	// Also you can see that BLEND_NONE isn't included anymore, it is pretty much gone, it's unecessary since BLEND_BLEND is what's mostly used
	//		although with the future more advance blending function you'll still be able to replicate it (having src as one and dst as zero)
	switch (mode)
	{
		case SRE_BLEND_BLEND:
		case SRE_BLEND_ADD:
		case SRE_BLEND_MOD:
		case SRE_BLEND_MUL:
			break;
		default:
			return false;
	}

	RENDERCMDCHECK();

	RenderCmd cmd{ CMD_STATE_BLEND };
	cmd.blend.mode = mode;
	m.rendercmds.emplace_back(cmd);
	return true;
}

void sre::render::reset_scissors() {
	sre::render::set_scissors({
		0.0_ut, 0.0_ut,
		engine.vsize_x, engine.vsize_y
	});
}

static void handle_render_switches(sre::flags32 flags, sre::Texture* texture)
{
	using namespace sre;
	if (engine.video.state.texture != texture)
	{
		RenderCmd cmd{ CMD_STATE_TEXTURE };
		cmd.texture.texture = texture ? texture + 1 : NULL;
		m.rendercmds.emplace_back(cmd);

		engine.video.state.texture = texture;
	}

	if (engine.video.state.lastdraw_flags != flags.get())
	{
		if ((engine.video.state.lastdraw_flags & SRE_DRAWFLAG_CAMERA) != (flags.get() & SRE_DRAWFLAG_CAMERA))
		{
			RenderCmd cmd{ CMD_STATE_CAMERA };
			cmd.camera.x = engine.video.state.camera_x * flags.has(SRE_DRAWFLAG_CAMERAX);
			cmd.camera.y = engine.video.state.camera_y * flags.has(SRE_DRAWFLAG_CAMERAY);
			m.rendercmds.emplace_back(cmd);
		}

		engine.video.state.lastdraw_flags = flags.get();
	}
}

void sre::render::draw1(sre::flags32 flags, const RenderInstance1 instances[], size_t instcount, Texture* texture)
{
	if (!instcount) // render_draw1 with `instcount` == 0 is a no-op
		return;

	if (!instances)
	{
		sre::error(SRE_ERR_INVALID_VALUE, "`instances` is NULL but `inscount` is non-zero.");
		return;
	}

	RENDERCMDCHECK();
	handle_render_switches(flags, texture);

	RenderCmd* lastcmd = &m.rendercmds.back();
	if (lastcmd->type != CMD_DRAW1)
	{
		RenderCmd cmd{ CMD_DRAW1 };
		cmd.draw.begin = m.rinst1cache.size();
		cmd.draw.size = 0;
		m.rendercmds.emplace_back(cmd);
		
		lastcmd = &m.rendercmds.back();
	}

	lastcmd->draw.size += instcount;
	m.rinst1cache.insert(m.rinst1cache.end(), instances, instances + instcount);
}

void sre::render::draw2(sre::flags32 flags, sre::col4 color, const sre::RenderPoint points[], size_t pcount, sre::draw2primitive mode, sre::Texture* texture)
{
	if (!pcount)
		return;

	if (!points)
	{
		sre::error(SRE_ERR_INVALID_VALUE, "`points` is NULL but `pcount` is non-zero.");
		return;
	}

	bool handle_lineloop = false;
	switch (mode)
	{
		case SRE_PRIMITIVE_TRIANGLES:
    	case SRE_PRIMITIVE_TRIANGLESTRIP:
    	case SRE_PRIMITIVE_LINEPERLINE:
    	case SRE_PRIMITIVE_LINESTRIP:
    	case SRE_PRIMITIVE_POINTS:
			break;
    	case SRE_PRIMITIVE_LINELOOP:
			if (engine.video.flags & SRE_RENDERBIT_SUPPORT_LINELOOP)
				break;
			mode = SRE_PRIMITIVE_LINESTRIP;
			handle_lineloop = true;
			break;
		default:
			sre::error(SRE_ERR_INVALID_PARAMETER, "`mode`: Unknown primitive mode.");
			return;
	}

	RENDERCMDCHECK();
	handle_render_switches(flags, texture);

	size_t bytebegin = m.rinst2cache.size();
	sre::RenderInstance2 instanceheader{
		mode,
		color
	};
	m.rinst2cache.insert(m.rinst2cache.end(), reinterpret_cast<sre::byte*>(&instanceheader), reinterpret_cast<sre::byte*>(&instanceheader.points));
	m.rinst2cache.insert(m.rinst2cache.end(), reinterpret_cast<const sre::byte*>(points), reinterpret_cast<const sre::byte*>(points + pcount));

	if (handle_lineloop)
	{
		m.rinst2cache.insert(m.rinst2cache.end(), reinterpret_cast<const sre::byte*>(points), reinterpret_cast<const sre::byte*>(points+1));
		pcount++;
	}

	RenderCmd cmd{ CMD_DRAW2 };
	cmd.draw.begin = bytebegin;
	cmd.draw.size = pcount;
	m.rendercmds.emplace_back(cmd);
}

// C wrappers

extern "C" void sre_render_set_vsync(bool enable) { sre::render::set_vsync(enable); }
extern "C" bool sre_render_set_scissors(const sre_rect2Dut* zone) { return sre::render::set_scissors(*zone); }
extern "C" bool sre_render_set_blendmode(sre_blendMode blendmode) { return sre::render::set_blendmode(blendmode); }
extern "C" void sre_render_reset_scissors() { sre::render::reset_scissors(); }
extern "C" void sre_render_draw1(sre_u32 flags, const sre_RenderInstance1 instances[], size_t instcount, sre_Texture* texture) {
	sre::render::draw1(flags, instances, instcount, texture);
}
extern "C" void sre_render_draw2(sre_u32 flags, const sre_col4* color, const sre_RenderPoint points[], size_t pcount, sre_draw2primitive mode, sre_Texture* texture) {
	sre::render::draw2(flags, *color, points, pcount, mode, texture);
}

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