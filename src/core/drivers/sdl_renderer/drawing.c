#include "sdl_renderer.h"

static inline int sresdlrender_color(const sre_u8 color[4])
{
	return SDL_SetRenderDrawColor(sresdlrenderer_driver.renderer, color[0], color[1], color[2], color[3]);
	// Remarks: Accessing the `data` pointer for each color component might be slow, try storing it on the stack first and compare the results
	// Uhh it's probably not a problem, it's just offseting the `data` pointer and then dereferencing it, there's probably no change
}

static inline void sresdlrender_coords(int cam, const sre_unit xy[2], float out[2])
{

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

	float xy1[2];
	float xy2[2];
	sresdlrender_coords(usecam, &data->pt1_x, xy1);
	sresdlrender_coords(usecam, &data->pt2_x, xy2);

	return SDL_RenderDrawLineF(sresdlrenderer_driver.renderer, xy1[0], xy1[1], xy2[0], xy2[1]);
}
int sresdlrenderer_draw_lines(const sre_DDLines* data)
{
	return -1;
}