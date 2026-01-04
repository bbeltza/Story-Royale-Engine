#include <Base/Draw.h>
#include <utils/logging.h>

#include "../internal.h"

int sre_draw(sre_DrawType type, const void* data)
{
	switch (type)
	{
	case SRE_DRAWMODE_FILL:
		//engine.video->draw_fill(data);
		break;
	default:
		ERROR("sre_draw: Invalid type");
		return -1;
	}

	return 0;
}