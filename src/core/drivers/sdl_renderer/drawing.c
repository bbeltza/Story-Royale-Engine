#include "sdl_renderer.h"

#include <utils/logging.h>
#include <utils/mem.h>
#include <utils/math.h>

#define sresdlrender_color(col) if (SDL_SetRenderDrawColor(video->userdata, col.r, col.g, col.b, col.a) < 0) return -1
#define sresdlrender_texture(tex) \
SDL_Texture* texture;			   \
{									\
	SDL_Texture** tex_ptr = (SDL_Texture**)sre_get_texture(tex);	\
	texture = tex_ptr ? *tex_ptr : NULL;									 \
}


static inline void sresdlrender_coords(const sre_videodriver* video, int cam, size_t count, const sre_vec2ut* coords, SDL_FPoint* out)
{
	float camcoords[2];
	if (cam)						
	{
		camcoords[0] = (float)(int)((video->camera.x - video->center.x) * video->scale);
		camcoords[1] = (float)(int)((video->camera.y - video->center.y) * video->scale);
	}
	else
	{
		camcoords[0] = 0;
		camcoords[1] = 0;
	}

	for (size_t i = 0; i < count; i++)
	{
		out[i].x = (float)(int)(coords[i].x * video->scale - camcoords[0]);
		out[i].y = (float)(int)(coords[i].y * video->scale - camcoords[1]);
	}
}

static inline void sresdlrender_coordsr(const sre_videodriver* video, int cam, const sre_rect2Dut* rect, SDL_FRect* out, sre_vec2ut anchor)
{
	float camcoords[2];
	if (cam)
	{
		camcoords[0] = (float)(int)((video->camera.x - video->center.x) * video->scale);
		camcoords[1] = (float)(int)((video->camera.y - video->center.y) * video->scale);
	}
	else
	{
		camcoords[0] = 0;
		camcoords[1] = 0;
	}

	out->w = (float)(int)(rect->w * video->scale);
	out->h = (float)(int)(rect->h * video->scale);

	out->x = (float)(int)(rect->x * video->scale - out->w * anchor.x - camcoords[0]);
	out->y = (float)(int)(rect->y * video->scale - out->h * anchor.y - camcoords[1]);
}

//

int sresdlrenderer_draw_clear(const sre_videodriver* video, const sre_col4* color)
{	
	assert(color != NULL);
	if (SDL_SetRenderDrawColor(video->userdata, color->r, color->g, color->b, color->a)) return -1;
	return SDL_RenderClear(video->userdata);
}

//

int sresdlrenderer_draw_fill(const sre_videodriver* video, const sre_DDFill* data)
{
	sresdlrender_color(data->color);
	
	return SDL_RenderFillRectF(video->userdata, NULL);
}
int sresdlrenderer_draw_line(const sre_videodriver* video, const sre_DDLine* data)
{
	sresdlrender_color(data->color);

	int usecam = data->flags & SRE_DRAWFLAGS_USECAM;

	SDL_FPoint xy[2];
	
	sresdlrender_coords(video, usecam, 2, &data->pt1, xy);

	return SDL_RenderDrawLinesF(video->userdata, xy, 2);
}

int sresdlrenderer_draw_lines(const sre_videodriver* video, const sre_DDLines* data)
{
	sresdlrender_color(data->color);

	int usecam = data->flags & SRE_DRAWFLAGS_USECAM;

	ut_dynsalloc(SDL_FPoint, pts, data->count);
	sresdlrender_coords(video, usecam, data->count, data->pts, pts);

	return SDL_RenderDrawLinesF(video->userdata, pts, data->count);
}

int sresdlrenderer_draw_rect(const sre_videodriver* video, const sre_DDRect* data)
{
	sresdlrender_color(data->color);

	int usecam = data->flags & SRE_DRAWFLAGS_USECAM;
	int usestroke = data->flags & SRE_DRAWFLAGS_STROKE;
	SDL_FRect render_rect;
	sresdlrender_coordsr(video, usecam, &data->rect, &render_rect, data->anchor);

	if (usestroke)
		return SDL_RenderDrawRectsF(video->userdata, &render_rect, 1);
	else
		return SDL_RenderFillRectsF(video->userdata, &render_rect, 1);
}

int sresdlrenderer_draw_rrect(const sre_videodriver* video, const sre_DDRRect* data)
{
	if (data->angle / 360 == (int)data->angle / 360)
		return sresdlrenderer_draw_rect(video, &data->rect);

	sresdlrender_color(data->rect.color);

	int usecam = data->rect.flags & SRE_DRAWFLAGS_USECAM;
	int usestroke = data->rect.flags & SRE_DRAWFLAGS_STROKE;

	const double rads = ut_rad(data->angle);
	const double rads90 = ut_rad(data->angle + 90);
	sre_vec2ut hsize = {
		data->rect.rect.w / 2,
		data->rect.rect.h / 2
	};

	sre_vec2ut xoffset = { 
		(sre_unit)(hsize.x * cos(rads)),
		(sre_unit)(hsize.x * sin(rads))
	};


	sre_vec2ut yoffset = {
		(sre_unit)(hsize.y * cos(rads90)),
		(sre_unit)(hsize.y * sin(rads90))
	};

	sre_vec2ut pts_units[4] = {
		{data->rect.rect.x - xoffset.x - yoffset.x, data->rect.rect.y - xoffset.y - yoffset.y},
		{data->rect.rect.x + xoffset.x - yoffset.x, data->rect.rect.y + xoffset.y - yoffset.y},
		{data->rect.rect.x + xoffset.x + yoffset.x, data->rect.rect.y + xoffset.y + yoffset.y},
		{data->rect.rect.x - xoffset.x + yoffset.x, data->rect.rect.y - xoffset.y + yoffset.y}
	};
	if (usestroke)
	{
		SDL_FPoint pts[5];
		sresdlrender_coords(video, usecam, 4, pts_units, pts);

		pts[4] = pts[0];
		return SDL_RenderDrawLinesF(video->userdata, pts, 5);
	}
	else
	{
		SDL_FPoint vertices[4];
		sresdlrender_coords(video, usecam, 4, pts_units, vertices);

		sre_u8 indices[6] = {
			0, 1, 2,
			0, 2, 3
		};
		return SDL_RenderGeometryRaw(video->userdata, NULL, (const float*)vertices, sizeof(float) * 2, (const SDL_Color*)&data->rect.color, 0, NULL, 0, 4, indices, 6, 1);
	}
}

int sresdlrenderer_draw_texture(const sre_videodriver* video, const sre_DDTexture* data)
{
	sresdlrender_texture(data->texture)

	if (!texture) return -1;
	if (data->rect.h == 0 || data->rect.h == 0) return 0;
	if (data->rect.w < 0 || data->rect.h < 0) return -1; // Drawing with negative sizes to flip the texture if no longer supported, use SDL_DRAWFLAGS_FLIP

	int usecam = data->flags & SRE_DRAWFLAGS_USECAM;
	SDL_RendererFlip flip = (data->flags & SRE_DRAWFLAGS_FLIPX ? SDL_FLIP_HORIZONTAL : 0) | (data->flags & SRE_DRAWFLAGS_FLIPY ? SDL_FLIP_VERTICAL : 0);
	SDL_FRect render_rect;
	sresdlrender_coordsr(video, usecam, &data->rect, &render_rect, data->anchor);
	
	if (SDL_SetTextureColorMod(texture, data->modulate.r, data->modulate.g, data->modulate.b)) return -1;
	if (SDL_SetTextureAlphaMod(texture, data->modulate.a)) return -1;
	
	SDL_Rect region = data->region;
	if (!region.w && SDL_QueryTexture(texture, NULL, NULL, &region.w, NULL) < 0) return -1;
	if (!region.h && SDL_QueryTexture(texture, NULL, NULL, NULL, &region.h) < 0) return -1;

	if (flip == SDL_FLIP_NONE) // SDL has to check for rotation too if using RenderCopy instead of RenderCopyEx, since we don't need it here, we do the check ourselves
		return SDL_RenderCopyF(video->userdata, texture, &region, &render_rect);
	else															       
		return SDL_RenderCopyExF(video->userdata, texture, &region, &render_rect, 0, NULL, flip);
}

static const sre_vec2ut VEC2_ZERO = SRE_VEC2SCALAR(0);

int sresdlrenderer_draw_rtexture(const sre_videodriver* video, const sre_DDRTexture* data)
{
	sresdlrender_texture(data->texture.texture)

	if (!texture) return -1;
	if (data->texture.rect.w == 0 || data->texture.rect.h == 0) return 0;
	if (data->texture.rect.w < 0 || data->texture.rect.h < 0) return -1;

	int usecam = data->texture.flags & SRE_DRAWFLAGS_USECAM;
	SDL_RendererFlip flip = (data->texture.flags & SRE_DRAWFLAGS_FLIPX ? SDL_FLIP_HORIZONTAL : 0) | (data->texture.flags & SRE_DRAWFLAGS_FLIPY ? SDL_FLIP_VERTICAL : 0);
	SDL_FRect render_rect;
	sresdlrender_coordsr(video, usecam, &data->texture.rect, &render_rect, VEC2_ZERO);

	if (SDL_SetTextureColorMod(texture, data->texture.modulate.r, data->texture.modulate.g, data->texture.modulate.b)) return -1;
	if (SDL_SetTextureAlphaMod(texture, data->texture.modulate.a)) return -1;

	SDL_Rect region = data->texture.region;
	if (!region.w && SDL_QueryTexture(texture, NULL, NULL, &region.w, NULL) < 0) return -1;
	if (!region.h && SDL_QueryTexture(texture, NULL, NULL, NULL, &region.h) < 0) return -1;

	return SDL_RenderCopyExF(video->userdata, texture, &region, &render_rect, data->angle, NULL, flip);
}

int sresdlrenderer_clip(const sre_videodriver* video, const sre_rect2Dut* rect)
{
	if (!rect)
		return SDL_RenderSetClipRect(video->userdata, NULL);

	SDL_FRect clip_frect;
	SDL_Rect clip_rect;
	sresdlrender_coordsr(video, false, rect, &clip_frect, VEC2_ZERO);

	clip_rect.x = (int)clip_frect.x;
	clip_rect.y = (int)clip_frect.y;
	clip_rect.w = (int)clip_frect.w;
	clip_rect.h = (int)clip_frect.h;

	return SDL_RenderSetClipRect(video->userdata, &clip_rect);
}