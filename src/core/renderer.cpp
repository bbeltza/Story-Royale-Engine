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
	enum RenderCmdType
	{
		CMD_BEGIN,
		CMD_DRAW1,
		CMD_DRAW2,

		CMD_STATE_SCISSOR,
		CMD_STATE_VIEWPORT,
		CMD_STATE_TEXTURE,
		CMD_STATE_CAMERA,
		CMD_STATE_BLEND
	};

	enum RenderUpdateBit 
	{
		UPD_SCISSOR = ut_bit(0),
		UPD_VIEWPORT = ut_bit(1),
		UPD_CAMERA = ut_bit(2),
		UPD_BLEND = ut_bit(3),

		UPD_ALL = UPD_SCISSOR | UPD_VIEWPORT | UPD_BLEND | UPD_CAMERA
	};

	struct RenderCmd
	{

		RenderCmd(RenderCmdType type): type(type) {};
		RenderCmd(const RenderCmd& cpy) {
			SDL_memcpy(this, &cpy, sizeof(RenderCmd));
		}

		RenderCmdType type;
		draw2primitive d2mode; // Only used in draw2 commands, cannot be bundled inside RenderCmd::draw due to empty padding on 64-bit platforms.
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
				sre::unit scale; // Only used in set_viewportstate
			} scissor_viewport;
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
		std::vector<RenderPoint> rinst2pcache;
        
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

extern "C" void __cleanup_textures();

void __cleanup_renderer()
{
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

	static_assert(sizeof(engine.video._vector_data) >= sizeof(sre::RenderVectors), "Please resize __engine_data.video._vector_data!!");
	new(engine.video._vector_data) sre::RenderVectors{};
	engine.video.texture_size = driverdata->texture_size;
	engine.video.flags = driverdata->flags;
	engine.video.state = {};
	engine.video.state.currentvsync = true;
	SRE_VIDEO(engine.video.driver, set_vsync, true);

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
	engine.scale_ratio = 1;
}

void __update_viewport(int w, int h)
{
	if (engine.osize.x == w && engine.osize.y == h)
		return;

	if (engine.autoscalex && engine.autoscaley)
	{
		int integer_scale = ut_min(w / engine.autoscalex, h / engine.autoscaley);
		integer_scale = integer_scale ? integer_scale : 1;
		engine.scale = static_cast<sre::unit>(integer_scale);
		engine.scale_ratio = 1 / engine.scale;
	}

	engine.osize = { w, h };
	engine.vsize = engine.osize * engine.scale_ratio;

	if (engine.video.driver->vfptr->resize_window) { // TODO: Handle `resize_window`'s boolean result
		SRE_VIDEO(engine.video.driver, resize_window, w, h);
	}
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
				// Removed the state update calls at the beginning. The necessary calls should be called before any draw thanks to new internal state flags
				// `handle_render_switches` sends all of the state commands now, it's in this source file

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
						&m.rinst2pcache.at(cmd.draw.begin),
						cmd.draw.size,
						cmd.d2mode
					);
					break;
				case CMD_STATE_VIEWPORT:
					SRE_VIDEO(engine.video.driver, set_viewportstate,
						&cmd.scissor_viewport.rect,
						 cmd.scissor_viewport.scale
					);
					break;
				case CMD_STATE_SCISSOR:
					SRE_VIDEO(engine.video.driver, set_scissorstate,
						&cmd.scissor_viewport.rect
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
		m.rinst2pcache.clear();
		m.rendercmds.clear();
	//

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

	// Reset and setup necessary state
	engine.video.state.texture = reinterpret_cast<sre_Texture*>(UINTPTR_MAX);
	engine.video.state.blendmode = SRE_BLEND_DEFAULT;
	engine.video.state.lastdraw_flags = 0;
	engine.video.state.scissor = { 0, engine.vsize };
	engine.video.state.viewport.area = engine.video.state.scissor;
	engine.video.state.viewport.center = engine.vsize * 0.5_ut;
	engine.video.state.viewport.scale = engine.scale;
	engine.video.state.camera = ((-camera + engine.video.state.viewport.center) * engine.scale).ceil();

	engine.video.state.state_update = UPD_ALL;

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

sre::vec2ut sre::calc_viewport_size(sre::rect2Dut zone, sre::unit scale) {
	if (zone.size.x > 0 && zone.size.y > 0)
		return zone.size;

	sre::unit ratio = !scale ? engine.scale_ratio : 1/scale;
	return sre::vec2ut{
		zone.size.x > 0 ? zone.size.x : ut::max(engine.osize.x*ratio + zone.size.x, 0.0_ut),
		zone.size.y > 0 ? zone.size.y : ut::max(engine.osize.y*ratio + zone.size.y, 0.0_ut)
	};
}

bool sre::render::set_viewport(sre::rect2Dut zone, sre::unit scale) {
	if (scale < 0)
		return false;
	if (scale == 0)
		scale = engine.scale;

	zone.size = sre::calc_viewport_size(zone, scale);
	if (engine.video.state.viewport.scale == scale && engine.video.state.viewport.area == zone)
		return false;

	// Viewport changes reset the scissor rects
	sre::render::reset_scissors();
	
	engine.video.state.camera /= engine.video.state.viewport.scale;
	engine.video.state.camera += engine.video.state.viewport.center;
	
	engine.video.state.viewport.area = zone;
	engine.video.state.viewport.scale = scale;
	engine.video.state.viewport.center = zone.size * 0.5_ut;
	engine.video.state.camera = ((-engine.video.state.camera + engine.video.state.viewport.center) * scale).ceil();

	engine.video.state.state_update |= (UPD_CAMERA | UPD_VIEWPORT | UPD_SCISSOR);
	return true;
}

bool sre::render::set_scissors(sre::rect2Dut zone) {
	if (engine.video.state.scissor == zone)
		return false;
	
	RENDERCMDCHECK();
	
	assert(zone.size.x >= 0);
	assert(zone.size.y >= 0);

	if (zone.position.x < 0) {
		zone.size.x += zone.position.x;
		zone.position.x = 0;
	}
	if (zone.position.y < 0) {
		zone.size.y += zone.position.y;
		zone.position.y = 0;
	}

	engine.video.state.scissor = zone;
	
	engine.video.state.state_update |= UPD_SCISSOR;
	return true;
}

void sre::render::reset_viewport() {
	bool result = sre::render::set_viewport({0, 0}, 0);
	(void)result;
}

void sre::render::reset_scissors() {
	bool result = sre::render::set_scissors({sre::vec2ut::ZERO, engine.vsize});
	(void)result;
}

bool sre::render::set_blendmode(sre::blendMode mode)
{
	// TODO: Blend mode implementations should be reliant on blending src, dst operation parameters, etc...
	// 	 and not just a single blending enum. This would reduce backend overhead more. This enum will thus just become a wrapper around
	// 	 the aforementioned more advanced future blending function
	// Also you can see that BLEND_NONE isn't included anymore, it is pretty much gone, it's unecessary since BLEND_BLEND is what's mostly used
	//		although with the future more advance blending function you'll still be able to replicate it (having src as one and dst as zero)
	if (mode == engine.video.state.blendmode)
		return false;

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

	engine.video.state.blendmode = mode;
	engine.video.state.state_update |= UPD_BLEND;
	return true;
}

//

bool sre::render::get_vsync(bool currently) {
	return currently ? engine.video.state.currentvsync : engine.video.state.desiredvsync;
}

sre::blendMode sre::render::get_blendmode() {
	if (!has_begun())
		return SRE_BLEND_DEFAULT; // Maybe return SRE_BLEND_UNKNOWN?

	return engine.video.state.blendmode;
}

sre::unit sre::render::get_viewport_scale() {
	if (!has_begun())
		return 0;
	
	return engine.video.state.viewport.scale;
}

sre::rect2Dut sre::render::get_viewport_area() {
	if (!has_begun())
		return { 0, 0 };
	
	return engine.video.state.viewport.area;
}

sre::vec2ut sre::render::get_viewport_center() {
	if (!has_begun())
		return sre::vec2ut::ZERO;
	
	return engine.video.state.viewport.center;
}

sre::rect2Dut sre::render::get_scissors_area(void) {
	if (!has_begun())
		return { 0, 0 };
	
	return engine.video.state.scissor;
}

//

static void handle_render_switches(sre::flags32 flags, sre::Texture* texture)
{
	using namespace sre;
	if (engine.video.state.texture != texture) {
		RenderCmd cmd{ CMD_STATE_TEXTURE };
		cmd.texture.texture = texture ? texture + 1 : NULL;
		m.rendercmds.emplace_back(cmd);

		engine.video.state.texture = texture;
	}

	if (engine.video.state.lastdraw_flags != flags.get()) {
		if ((engine.video.state.lastdraw_flags & SRE_DRAWFLAG_CAMERA) != (flags.get() & SRE_DRAWFLAG_CAMERA)) 
			engine.video.state.state_update |= UPD_CAMERA;

		engine.video.state.lastdraw_flags = flags.get();
	}

	int state = engine.video.state.state_update;
	if (state & UPD_BLEND) {
		RenderCmd cmd{ CMD_STATE_BLEND };
		cmd.blend.mode = engine.video.state.blendmode;
		m.rendercmds.emplace_back(cmd);
	}
	if (state & UPD_VIEWPORT) {
		const auto& zone = engine.video.state.viewport.area;
		const auto& scale = engine.video.state.viewport.scale;
		RenderCmd cmd{ CMD_STATE_VIEWPORT };
		cmd.scissor_viewport.rect = {
			static_cast<int>(zone.position.x * scale ),
			static_cast<int>(zone.position.y * scale ),
			static_cast<int>(zone.size.x * scale ),
			static_cast<int>(zone.size.y * scale )
		};
		cmd.scissor_viewport.scale = scale;
		m.rendercmds.emplace_back(cmd);
	}
	if (state & UPD_SCISSOR) {
		const auto& zone = engine.video.state.scissor;
		const auto& scale = engine.video.state.viewport.scale; // The current viewport's scale is needed for the upcoming operations
		RenderCmd cmd{ CMD_STATE_SCISSOR };
		cmd.scissor_viewport.rect = {
			static_cast<int>(zone.position.x * scale ),
			static_cast<int>(zone.position.y * scale ),
			static_cast<int>(zone.size.x * scale ),
			static_cast<int>(zone.size.y * scale )
		};
		m.rendercmds.emplace_back(cmd);
	}
	if (state & UPD_CAMERA) {
		RenderCmd cmd{ CMD_STATE_CAMERA };
		cmd.camera.x = engine.video.state.camera.x * flags.has(SRE_DRAWFLAG_CAMERAX);
		cmd.camera.y = engine.video.state.camera.y * flags.has(SRE_DRAWFLAG_CAMERAY);
		m.rendercmds.emplace_back(cmd);
	}

	engine.video.state.state_update = 0;
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

	#if 0 // Enable an optimizaion that removes any previous draw calls if the last instance is big enough (and opaque) to fill the entire screen
			// Note that if there's render::draw1 call with multiple instances passed in and one of the instances at the middle is enough to fill the screen, this check won't pass to that instance.

	// This is unreliable right now, with the support of viewports... Since this optimization can do wrong clearings with different viewport sizes and scales...
	// One very nice solution would be to track the last viewport command (Fallback to `begin` if there aren't any) and remove any drawcalls from that command
	sre::rect2Dut displayrect{ 0, engine.vsize };
	if (!flags && !texture && engine.video.state.blendmode == SRE_BLEND_BLEND && engine.video.state.scissor == engine.video.state.viewport.area && engine.video.state.viewport.scale == engine.scale && engine.video.state.scissor == displayrect ) {
		const RenderInstance1* lastinst = &instances[instcount-1];
		if (lastinst->color.a == 255 &&
			lastinst->rectangle.size.x >= engine.vsize.x &&
			lastinst->rectangle.size.y >= engine.vsize.y &&
			lastinst->rectangle.position.x - lastinst->anchor.x*lastinst->rectangle.size.x <= 0 &&
			lastinst->rectangle.position.y - lastinst->anchor.y*lastinst->rectangle.size.y <= 0)
		{
			m.rendercmds.erase(m.rendercmds.begin()+1, m.rendercmds.end());

			instances = lastinst;
			instcount = 1;

			engine.video.state.state_update |= UPD_ALL;
		}
	}

	#endif

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

void sre::render::draw2(sre::flags32 flags, const sre::RenderPoint points[], size_t pcount, sre::draw2primitive mode, sre::Texture* texture)
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

	size_t begin = m.rinst2pcache.size();
	m.rinst2pcache.insert(m.rinst2pcache.end(), points, points + pcount);

	if (handle_lineloop)
	{
		m.rinst2pcache.emplace_back(points[0]);
		pcount++;
	}

	auto& lastcmd = m.rendercmds.back();
	if (lastcmd.type == CMD_DRAW2 && lastcmd.d2mode == mode && (mode == SRE_PRIMITIVE_TRIANGLES || mode == SRE_PRIMITIVE_POINTS || mode == SRE_PRIMITIVE_LINEPERLINE)) {
		lastcmd.draw.size += pcount;
	}
	else {
		RenderCmd cmd{ CMD_DRAW2 };
		cmd.d2mode = mode;
		cmd.draw.begin = begin;
		cmd.draw.size = pcount;
		m.rendercmds.emplace_back(cmd);
	}
}

// C wrappers

extern "C" void sre_render_set_vsync(bool enable) { sre::render::set_vsync(enable); }
extern "C" bool sre_render_set_viewport(const sre_rect2Dut* zone, sre_unit scale) { return sre::render::set_viewport(*zone, scale); }
extern "C" bool sre_render_set_scissors(const sre_rect2Dut* zone) { return sre::render::set_scissors(*zone); }
extern "C" bool sre_render_set_blendmode(sre_blendMode blendmode) { return sre::render::set_blendmode(blendmode); }

extern "C" bool sre_render_get_viewport(sre_rect2Dut* area, sre_vec2ut* center, sre_unit* scale) {
	if (!area && !center && !scale)
		return false;

	if (!sre::render::has_begun()) {
		return false;
	}

	if (area) {
		*area = engine.video.state.viewport.area;
	}
	if (center) {
		*center = engine.video.state.viewport.center;
	}
	if (scale) {
		*scale = engine.video.state.viewport.scale;
	}

	return true;
}

extern "C" bool sre_render_get_scissors(sre_rect2Dut* area) {
	if (!area)
		return false;

	if (!sre::render::has_begun()) {
		return false;
	}

	*area = engine.video.state.scissor;
	return true;
}
extern "C" bool sre_render_get_vsync(bool currently) { return sre::render::get_vsync(); }
extern "C" sre_blendMode sre_render_get_blendmode(void) { return sre::render::get_blendmode(); }

extern "C" void sre_render_reset_viewport() { sre::render::reset_viewport(); }
extern "C" void sre_render_reset_scissors() { sre::render::reset_scissors(); }

extern "C" void sre_render_draw1(sre_u32 flags, const sre_RenderInstance1 instances[], size_t instcount, sre_Texture* texture) {
	sre::render::draw1(flags, instances, instcount, texture);
}
extern "C" void sre_render_draw2(sre_u32 flags, const sre_RenderPoint points[], size_t pcount, sre_draw2primitive mode, sre_Texture* texture) {
	sre::render::draw2(flags, points, pcount, mode, texture);
}