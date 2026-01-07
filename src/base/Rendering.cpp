#include <standard>
#include "../internal.h"

#include "Base/Draw.hpp"
#include "Base/Display.hpp"
#include "Base/Texture.hpp"

#include "ECS/scene.hpp"

#include "utils/mem.h"

#include "GameSettings.h"
#define DRAW_ENTCENTER_LINESIZE sre::game_settings.DebugOptions.EntityDebugLineSize

inline void real_coords(sre::vec2f& out_pos, const sre::vec2ut& in_pos, const sreECS::Scene* world)
{
    if (world == DISPLAY_DONT_CENTER)
    {
        out_pos.x = static_cast<float>(round(in_pos.x * engine.current_scale));
        out_pos.y = static_cast<float>(round(in_pos.y * engine.current_scale));
        return;
    }
    out_pos = world->camera.toScreenSpace(in_pos); // NOTE: World might be NULL, see definition for Camera::toScreenSpace(sre::vec2ut)
}

inline void real_coords(SDL_FRect& out_rect, const sre::rect2Dut& in_rect, const sre::vec2f& anchor, const sreECS::Scene* world)
{
    out_rect.w = static_cast<float>(floor(in_rect.size.x * engine.current_scale));
    out_rect.h = static_cast<float>(floor(in_rect.size.y * engine.current_scale));

    if (world == DISPLAY_DONT_CENTER)
    {
        out_rect.x = static_cast<float>(floor((in_rect.position.x * engine.current_scale - in_rect.size.x * anchor.x)));
        out_rect.y = static_cast<float>(floor((in_rect.position.y * engine.current_scale - in_rect.size.y * anchor.y)));
        return;
    }
    reinterpret_cast<sre::rect2Df*>(&out_rect)->position = world->camera.toScreenSpace(in_rect.position - in_rect.size * anchor);
}

#define START_DRAW SDL_LockMutex(engine.sdl_rendermutex);
#define END_DRAW SDL_UnlockMutex(engine.sdl_rendermutex);

void Display::DrawLine(const sre::col4 &Color, const sre::vec2ut &Pt1, const sre::vec2ut &Pt2, const sreECS::Scene* world)
{
    START_DRAW

    sre_DDLine data;
    data.color[0] = Color.r;
    data.color[1] = Color.g;
    data.color[2] = Color.b;
    data.color[3] = Color.a;
    data.pt1_x = Pt1.x;
    data.pt1_y = Pt1.y;
    data.pt2_x = Pt2.x;
    data.pt2_y = Pt2.y;
    data.flags = world != DISPLAY_DONT_CENTER ? SRE_DRAWFLAGS_USECAM : 0;
    sre_draw(SRE_DRAW_LINE, &data);

    END_DRAW
}

void Display::DrawLines(const sre::col4 &Color, int Count, const sre::vec2ut *Pts, const sreECS::Scene* world)
{
    START_DRAW

    sre_DDLines data;
    data.count = Count;
    data.color[0] = Color.r;
    data.color[1] = Color.g;
    data.color[2] = Color.b;
    data.color[3] = Color.a;
    data.pts = reinterpret_cast<const sre::unit(*)[2]>(Pts);
    data.flags = (world != DISPLAY_DONT_CENTER ? SRE_DRAWFLAGS_USECAM : 0);

    sre_draw(SRE_DRAW_LINES, &data);

    END_DRAW
}

void Display::DrawRectangle(const sre::rect2Dut &Rectangle, const sre::col4 &Color, const sre::vec2f &AnchorPoint, DrawingMode Mode, const sreECS::Scene* world)
{
    START_DRAW

    sre_DDRect data;
    data.anchor_x = AnchorPoint.x;
    data.anchor_y = AnchorPoint.y;
    data.color[0] = Color.r;
    data.color[1] = Color.g;
    data.color[2] = Color.b;
    data.color[3] = Color.a;
    data.pos_x = Rectangle.position.x;
    data.pos_y = Rectangle.position.y;
    data.size_x = Rectangle.size.x;
    data.size_y = Rectangle.size.y;
    data.flags = (Mode == Display::M_STROKE ? SRE_DRAWFLAGS_STROKE : 0) | (world != DISPLAY_DONT_CENTER ? SRE_DRAWFLAGS_USECAM : 0);
    sre_draw(SRE_DRAW_RECTANGLE, &data);

    END_DRAW
};

void Display::DrawRotatedRectangle(const sre::rect2Dut &rectangle, const double angle, const sre::col4 &col, DrawingMode mode, const sreECS::Scene* world)
{
    sre_DDRRect data;
    data.rect = { 
        (mode == Display::M_STROKE ? SRE_DRAWFLAGS_STROKE : 0) | (world != DISPLAY_DONT_CENTER ? SRE_DRAWFLAGS_USECAM : 0),
        {col.r, col.g, col.b, col.a},

        rectangle.position.x,
        rectangle.position.y,
        rectangle.size.x,
        rectangle.size.y,
        0.5_ut,
        0.5_ut
    };
    data.angle = static_cast<float>(angle);

    sre_draw(SRE_DRAW_RRECTANGLE, &data);
}

void Display::DrawCircle(const sre::vec2ut &pos, const sre::unit radius, const sre::col4 &_Col, DrawingMode _dm, const sreECS::Scene* world)
#if 0
{
    START_DRAW

    SDL_Rect v;
    SDL_RenderGetViewport(engine.sdl_rendererhndl, &v);

    float _x = pos.X;
    float _y = pos.Y;

    if (_x + radius < 0 || _y + radius < 0 || _x - radius > v.w || _y - radius > v.h) goto end;
    {
        float x = radius;
        float y = 0;
        float radiusError = 1 - x;
        while (x >= y)
        {
            SDL_RenderDrawLineF(engine.sdl_rendererhndl, x + _x, y + _y, -x + _x, y + _y);
            SDL_RenderDrawLineF(engine.sdl_rendererhndl, y + _x, x + _y, -y + _x, x + _y);
            SDL_RenderDrawLineF(engine.sdl_rendererhndl, -x + _x, -y + _y, x + _x, -y + _y);
            SDL_RenderDrawLineF(engine.sdl_rendererhndl, -y + _x, -x + _y, y + _x, -x + _y);
            y++;
            if (radiusError < 0)
                radiusError += 2 * y + 1;
            else
            {
                x--;
                radiusError += 2 * (y - x + 1);
            }
        }
    }

end:
    END_DRAW
}
#else
{
    // Use dummy DrawCircle definition untill I'll need it
    assert(0 && "Display::DrawCircle isn't implemented");
}
#endif

void Display::DrawTexture(const Texture &texture, sre::rect2Dut rect, const sre::col4 &mod, const sre::vec2f &anchor, const sreECS::Scene* world)
{
    START_DRAW

    sre_DDTexture data;
    data.flags = (world != DISPLAY_DONT_CENTER ? SRE_DRAWFLAGS_USECAM : 0) | (rect.size.x < 0 ? SRE_DRAWFLAGS_FLIPX : 0) | (rect.size.y < 0 ? SRE_DRAWFLAGS_FLIPY : 0);
    data.anchor_x = anchor.x;
    data.anchor_y = anchor.y;
    data.modulate[0] = mod.r;
    data.modulate[1] = mod.g;
    data.modulate[2] = mod.b;
    data.modulate[3] = mod.a;
    data.texture = *reinterpret_cast<void*const*>(&texture); // Silly way to get the texture on a function that's a friend of Texture!
    data.pos_x = rect.position.x;
    data.pos_y = rect.position.y;
    data.size_x = abs(rect.size.x);
    data.size_y = abs(rect.size.y);

    sre_draw(SRE_DRAW_TEXTURE, &data);

    END_DRAW
}

// Already testing current available draw functions

void Display::Fill(const sre::col4 &color)
{
    sre_draw(SRE_DRAW_FILL, &color);
}
