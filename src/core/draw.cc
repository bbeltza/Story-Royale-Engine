#include <Core/Render.h>
#include <Core/Draw.h>
#include <Base/Log.h>

#include "../internal.h"

#include <utils/mem.h>

bool sre_draw(sre_DrawType type, const void* data)
{
	struct sre_RenderInterface* interface = engine.video;
	assert(interface != NULL);

	bool ret;

	switch (type)
	{
	case SRE_DRAW_CIRCLE:
		sre::log<sre::LOGCATEGORY_ERROR>("sre_draw: Unavailable option 'SRE_DRAW_CIRCLE'");
		ret = false;
		break;
	case SRE_DRAW_FILL:
		interface->draw1(0, { {
			{0, 0, 65536, 65536},
			{0, 0}, *static_cast<const sre::col4*>(data)
			}});
		break;
	case SRE_DRAW_LINES:
		ret = interface->draw_lines(engine.video, data);
		break;
	case SRE_DRAW_RECTANGLE:
		ret = interface->draw_rect(engine.video, data);
		break;
	case SRE_DRAW_RRECTANGLE:
		ret = interface->draw_rrect(engine.video, data);
		break;
	case SRE_DRAW_TEXTURE:
		ret = interface->draw_texture(engine.video, data);
		break;
	case SRE_DRAW_RTEXTURE:
		ret = interface->draw_rtexture(engine.video, data);
		break;
	default:
		sre_log(SRE_LOGCATEGORY_ERROR, "sre_draw: Invalid type");
		ret = false;
		break;
	}
	
	return ret;
}
