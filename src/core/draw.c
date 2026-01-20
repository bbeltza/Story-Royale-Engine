#include <Base/Draw.h>
#include <utils/logging.h>

#include "../internal.h"
#include "drivers/drivers.h"

int sre_draw(sre_DrawType type, const void* data)
{
	SDL_LockMutex(engine.sdl_rendermutex);
	int ret;

	switch (type)
	{
	case SRE_DRAW_CIRCLE:
		ERROR("sre_draw: Unavailable option 'SRE_DRAW_CIRCLE'");
		ret = -1;
		break;
	case SRE_DRAW_FILL:
		ret = engine.video->draw_fill(engine.video, data);
		break;
	case SRE_DRAW_LINE:
		ret = engine.video->draw_line(engine.video, data);
		break;
	case SRE_DRAW_LINES:
		ret = engine.video->draw_lines(engine.video, data);
		break;
	case SRE_DRAW_RECTANGLE:
		ret = engine.video->draw_rect(engine.video, data);
		break;
	case SRE_DRAW_RRECTANGLE:
		ret = engine.video->draw_rrect(engine.video, data);
		break;
	case SRE_DRAW_TEXTURE:
		ret = engine.video->draw_texture(engine.video, data);
		break;
	case SRE_DRAW_RTEXTURE:
		ret = engine.video->draw_rtexture(engine.video, data);
		break;
	default:
		ERROR("sre_draw: Invalid type");
		ret = -1;
		break;
	}

	SDL_UnlockMutex(engine.sdl_rendermutex);
	return ret;
}