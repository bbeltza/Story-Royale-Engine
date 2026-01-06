#include "sdl_renderer.h"

#include <utils/logging.h>
#include <utils/mem.h>

static inline int sresdlrender_color(const sre_u8 color[4])
{
	return SDL_SetRenderDrawColor(sresdlrenderer_driver.renderer, color[0], color[1], color[2], color[3]);
	// Remarks: Accessing the `data` pointer for each color component might be slow, try storing it on the stack first and compare the results
	// Uhh it's probably not a problem, it's just offseting the `data` pointer and then dereferencing it, there's probably no change
	// Actually did see the assembly for it, it's pretty dumb... At least with no optimizations
		// it's litteraly setting 1 to a register and then multiplying it by another constant *..*
}

static inline void sresdlrender_coords(int cam, size_t count, const sre_unit* xy, float* out)
{
	float camcoords[2];
	if (cam)						
	{
		camcoords[0] = (float)(int)((sresdlrenderer_driver.video.camera_x - sresdlrenderer_driver.video.center_x) * sresdlrenderer_driver.video.scale);
		camcoords[1] = (float)(int)((sresdlrenderer_driver.video.camera_y - sresdlrenderer_driver.video.center_y) * sresdlrenderer_driver.video.scale);
	}
	else
	{
		camcoords[0] = 0;
		camcoords[1] = 0;
	}

	count *= 2;

	for (size_t i = 0; i < count; i += 2)
	{
		out[i + 0] = (float)(int)(xy[i + 0] * sresdlrenderer_driver.video.scale - camcoords[0]);
		out[i + 1] = (float)(int)(xy[i + 1] * sresdlrenderer_driver.video.scale - camcoords[1]);
	}
}

static inline void sresdlrender_coordsr(int cam, const sre_unit xywh[4], SDL_FRect* out, sre_unit anchor_x, sre_unit anchor_y)
{
	sre_unit camcoords[2];
	if (cam)
	{
		camcoords[0] = (float)(int)((sresdlrenderer_driver.video.camera_x - sresdlrenderer_driver.video.center_x) * sresdlrenderer_driver.video.scale);
		camcoords[1] = (float)(int)((sresdlrenderer_driver.video.camera_y - sresdlrenderer_driver.video.center_y) * sresdlrenderer_driver.video.scale);
	}
	else
	{
		camcoords[0] = 0;
		camcoords[1] = 0;
	}

	out->w = (float)(int)(xywh[2] * sresdlrenderer_driver.video.scale);
	out->h = (float)(int)(xywh[3] * sresdlrenderer_driver.video.scale);

	out->x = (float)(int)(xywh[0] * sresdlrenderer_driver.video.scale - out->w * anchor_x - camcoords[0]);
	out->y = (float)(int)(xywh[1] * sresdlrenderer_driver.video.scale - out->h * anchor_y - camcoords[1]);
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

int sresdlrenderer_draw_rrect(const sre_DDRRect* data)
{
	if (sresdlrender_color(data->rect.color)) return -1;

	int usecam = data->rect.flags & SRE_DRAWFLAGS_USECAM;
	int usestroke = data->rect.flags & SRE_DRAWFLAGS_STROKE;
	if (usestroke)
	{
		sre_unit pts_units[4][2] = { // TODO: Finish this :)
			{0, 0},
			{0, 0},
			{0, 0},
			{0, 0}
		};
		SDL_FPoint pts[5];
		sresdlrender_coords(usecam, 4, pts_units[0], (float*)pts);

		pts[4] = pts[0];
		return SDL_RenderDrawLinesF(sresdlrenderer_driver.renderer, pts, 5);
	}
	else
	{
		SDL_FRect render_rect;
		sresdlrender_coordsr(usecam, &data->rect.pos_x, &render_rect, data->rect.anchor_x, data->rect.anchor_y);
																				/// ^^^ TODO: Fix rotation anchor issues, calculate the top-left position correctly
		return SDL_RenderCopyExF(sresdlrenderer_driver.renderer, sresdlrenderer_driver.rect_tex, NULL, &render_rect, data->angle, NULL, 0);
	}
}

int sresdlrenderer_draw_texture(const sre_DDTexture* data)
{
	if (!data->texture) return -1;
	if (data->size_x == 0 || data->size_y == 0) return 0;
	if (data->size_x < 0 || data->size_y < 0) return -1; // Drawing with negative sizes to flip the texture if no longer supported, use SDL_DRAWFLAGS_FLIP

	int usecam = data->flags & SRE_DRAWFLAGS_USECAM;
	SDL_RendererFlip flip = (data->flags & SRE_DRAWFLAGS_FLIPX ? SDL_FLIP_HORIZONTAL : 0) | (data->flags & SRE_DRAWFLAGS_FLIPY ? SDL_FLIP_VERTICAL : 0);
	SDL_FRect render_rect;
	sresdlrender_coordsr(usecam, &data->pos_x, &render_rect, data->anchor_x, data->anchor_y);

	if (SDL_SetTextureColorMod(data->texture, data->modulate[0], data->modulate[1], data->modulate[2])) return -1;
	if (SDL_SetTextureAlphaMod(data->texture, data->modulate[3])) return -1;

	if (flip == SDL_FLIP_NONE) // SDL has to check for rotation too if using RenderCopy instead of RenderCopyEx, since we don't need it here, we do the check ourselves
		return SDL_RenderCopyF(sresdlrenderer_driver.renderer, data->texture, NULL, &render_rect);
	else															       // ^^^^
																		   // TODO: Support source coordinates for the texture
		return SDL_RenderCopyExF(sresdlrenderer_driver.renderer, data->texture, NULL, &render_rect, 0, NULL, flip);
}

int sresdlrenderer_draw_rtexture(const sre_DDRTexture* data)
{
	if (!data->texture.texture) return -1;
	if (data->texture.size_x == 0 || data->texture.size_y == 0) return 0;
	if (data->texture.size_x < 0 || data->texture.size_y < 0) return -1;

	int usecam = data->texture.flags & SRE_DRAWFLAGS_USECAM;
	SDL_RendererFlip flip = (data->texture.flags & SRE_DRAWFLAGS_FLIPX ? SDL_FLIP_HORIZONTAL : 0) | (data->texture.flags & SRE_DRAWFLAGS_FLIPY ? SDL_FLIP_VERTICAL : 0);
	SDL_FRect render_rect;
	sresdlrender_coordsr(usecam, &data->texture.pos_x, &render_rect, 0, 0);

	if (SDL_SetTextureColorMod(data->texture.texture, data->texture.modulate[0], data->texture.modulate[1], data->texture.modulate[2])) return -1;
	if (SDL_SetTextureAlphaMod(data->texture.texture, data->texture.modulate[3])) return -1;

	return SDL_RenderCopyExF(sresdlrenderer_driver.renderer, data->texture.texture, NULL, &render_rect, data->angle, NULL, flip);
}