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
		ret = engine.video->draw_fill(data);
		break;
	case SRE_DRAW_LINE:
		ret = engine.video->draw_line(data);
		break;
	case SRE_DRAW_LINES:
		ret = engine.video->draw_lines(data);
		break;
	case SRE_DRAW_RECTANGLE:
		ret = engine.video->draw_rect(data);
		break;
	case SRE_DRAW_RRECTANGLE:
		ret = engine.video->draw_rrect(data);
		break;
	case SRE_DRAW_TEXTURE:
		ret = engine.video->draw_texture(data);
		break;
	case SRE_DRAW_RTEXTURE:
		ret = engine.video->draw_rtexture(data);
		break;
	default:
		ERROR("sre_draw: Invalid type");
		ret = -1;
		break;
	}

	SDL_UnlockMutex(engine.sdl_rendermutex);
	return ret;
}