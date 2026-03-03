#include <Core/Draw.h>
#include <Base/Log.h>

#include "../internal.h"
#include "drivers/drivers.h"

#include <utils/mem.h>

bool sre_draw(sre_DrawType type, const void* data)
{
	bool ret;

	switch (type)
	{
	case SRE_DRAW_CIRCLE:
		sre_log(SRE_LOGCATEGORY_ERROR, "sre_draw: Unavailable option 'SRE_DRAW_CIRCLE'");
		ret = false;
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
		sre_log(SRE_LOGCATEGORY_ERROR, "sre_draw: Invalid type");
		ret = false;
		break;
	}
	
	return ret;
}

bool sre_draw_clipbegin(const sre_rect2Dut* _rect)
{
	sre_rect2Dut rect = *_rect;

	if (!engine.video->clipstack_base)
	{
		static bool unsupport = false;
		if (!unsupport)
		{
			sre_log(SRE_LOGCATEGORY_ERROR, "sre_draw_clipbegin: Unsupported");
			unsupport = true;
		}
		return false;
	}
	assert(engine.video->draw_clip != NULL);
	
	if (engine.video->clipstack_pos != 0)
	{
		sre_rect2Dut curr = engine.video->clipstack_base[engine.video->clipstack_pos];
		rect.x = rect.x > curr.x ? rect.x : curr.x;
		rect.y = rect.y > curr.y ? rect.y : curr.y;
		
		sre_unit diffw = (rect.x + rect.w) - (curr.x + curr.w);
		sre_unit diffh = (rect.y + rect.h) - (curr.y + curr.h);
		rect.w = diffw > 0 ? rect.w	- diffw : rect.w;
		rect.h = diffh > 0 ? rect.h	- diffh : rect.h;
		
		rect.w = rect.w > 0 ? rect.w : 0;
		rect.h = rect.h > 0 ? rect.h : 0;
	}

	(*(size_t*)&engine.video->clipstack_pos)++;
	if (engine.video->clipstack_pos > engine.video->clipstack_size)
	{
		size_t new_size = engine.video->clipstack_size * 2;
		sre_rect2Dut* new_ptr = sre_new(new_size * sizeof(sre_rect2Dut));
		new_ptr = memcpy(new_ptr, engine.video->clipstack_base, engine.video->clipstack_size);
		assert(new_ptr != NULL);

		sre_delete((void*)engine.video->clipstack_base);
		(*(size_t*)&engine.video->clipstack_size) = new_size;
		engine.video->clipstack_base = new_ptr;
	}


	memcpy((sre_rect2Dut*)engine.video->clipstack_base + engine.video->clipstack_pos, &rect, sizeof(sre_rect2Dut));
	return engine.video->draw_clip(engine.video, &rect);
}

void sre_draw_clipend()
{
	if (!engine.video->clipstack_base) return;
	assert(engine.video->draw_clip != NULL);

	if (engine.video->clipstack_pos == 0) return;
	
	(*(size_t*)&engine.video->clipstack_pos)--;
	engine.video->draw_clip(engine.video, engine.video->clipstack_pos != 0 ? &engine.video->clipstack_base[engine.video->clipstack_pos] : NULL);
}

bool sre_draw_blend(sre_DrawBlending blend)
{
	return engine.video->blend(engine.video, blend);
}