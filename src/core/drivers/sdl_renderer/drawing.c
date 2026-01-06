#include "sdl_renderer.h"

#include <utils/logging.h>
#include <utils/mem.h>

static inline int sresdlrender_color(const sre_u8 color[4])
{
	return SDL_SetRenderDrawColor(sresdlrenderer_driver.renderer, color[0], color[1], color[2], color[3]);
	// Remarks: Accessing the `data` pointer for each color component might be slow, try storing it on the stack first and compare the results
	// Uhh it's probably not a problem, it's just offseting the `data` pointer and then dereferencing it, there's probably no change
}

static inline void sresdlrender_coords(int cam, size_t count, const sre_unit* xy, float* out)
{
	sre_unit camcoords[2];
	if (cam)						
	{
		camcoords[0] = sresdlrenderer_driver.video.camera_x - sresdlrenderer_driver.video.center_x;
		camcoords[1] = sresdlrenderer_driver.video.camera_y - sresdlrenderer_driver.video.center_y;
	}
	else
	{
		camcoords[0] = 0;
		camcoords[1] = 0;
	}

	count *= 2;

	for (size_t i = 0; i < count; i += 2)
	{
		out[i + 0] = (float)(int)((xy[i + 0] - camcoords[0]) * sresdlrenderer_driver.video.scale);
		out[i + 1] = (float)(int)((xy[i + 1] - camcoords[1]) * sresdlrenderer_driver.video.scale);
	}
}

static inline void sresdlrender_coordsr(int cam, const sre_unit xywh[4], SDL_FRect* out, sre_unit anchor_x, sre_unit anchor_y)
{
	sre_unit camcoords[2];
	if (cam)
	{
		camcoords[0] = sresdlrenderer_driver.video.camera_x - sresdlrenderer_driver.video.center_x;
		camcoords[1] = sresdlrenderer_driver.video.camera_y - sresdlrenderer_driver.video.center_y;
	}
	else
	{
		camcoords[0] = 0;
		camcoords[1] = 0;
	}

	out->w = (float)(int)(xywh[2] * sresdlrenderer_driver.video.scale);
	out->h = (float)(int)(xywh[3] * sresdlrenderer_driver.video.scale);

	out->x = (float)(int)((xywh[0] - camcoords[0]) * sresdlrenderer_driver.video.scale - out->w * anchor_x);
	out->y = (float)(int)((xywh[1] - camcoords[1]) * sresdlrenderer_driver.video.scale - out->h * anchor_y);
}

//

int sresdlrenderer_draw_fill(const sre_DDFill* data)
{
	if (sresdlrender_color(data->color)) return -1;
	
	return SDL_RenderFillRectF(sresdlrenderer_driver.renderer, NULL);
}
int sresdlrenderer_draw_line(const sre_DDLine* data)
{
	if (sresdlrender_color(data->color)) return -1;

	int usecam = data->flags & SRE_DRAWFLAGS_USECAM;

	float xy[2][2];
	
	sresdlrender_coords(usecam, 2, &data->pt1_x, (float*)xy);

	return SDL_RenderDrawLineF(sresdlrenderer_driver.renderer, xy[0][0], xy[0][1], xy[1][0], xy[1][1]);
}

int sresdlrenderer_draw_lines(const sre_DDLines* data)
{
	if (sresdlrender_color(data->color)) return -1;

	int usecam = data->flags & SRE_DRAWFLAGS_USECAM;

	ut_dynsalloc(float, pts, data->count * 2);
	sresdlrender_coords(usecam, data->count, data->pts[0], pts);

	return SDL_RenderDrawLinesF(sresdlrenderer_driver.renderer, (SDL_FPoint*)pts, data->count);
}

int sresdlrenderer_draw_rect(const sre_DDRect* data)
{
	if (sresdlrender_color(data->color)) return -1;

	int usecam = data->flags & SRE_DRAWFLAGS_USECAM;
	int usestroke = data->flags & SRE_DRAWFLAGS_STROKE;
	SDL_FRect render_rect;
	sresdlrender_coordsr(usecam, &data->pos_x, &render_rect, data->anchor_x, data->anchor_y);

	if (usestroke)
		return SDL_RenderDrawRectF(sresdlrenderer_driver.renderer, &render_rect);
	else
		return SDL_RenderFillRectF(sresdlrenderer_driver.renderer, &render_rect);
}