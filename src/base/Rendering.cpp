#include <standard>
#include "../internal.h"

#include "Base/Display.hpp"
#include "Base/Texture.hpp"

#include "Game/World.hpp"

#include "utils/mem.h"

#include "GameSettings.h"
#define DRAW_ENTCENTER_LINESIZE sre::game_settings.DebugOptions.EntityDebugLineSize

inline void real_coords(sre::vec2f& out_pos, const sre::vec2ut& in_pos, const Game::World* world)
{
    if (world == DISPLAY_DONT_CENTER)
    {
        out_pos.x = static_cast<float>(round(in_pos.x * engine.current_scale));
        out_pos.y = static_cast<float>(round(in_pos.y * engine.current_scale));
        return;
    }
    out_pos = Game::World::worldToScreen(in_pos.x, in_pos.y, world ? &world->CurrentCamera : nullptr);
}

inline void real_coords(SDL_FRect& out_rect, const sre::rect2Dut& in_rect, const sre::vec2f& anchor, const Game::World* world)
{
    out_rect.w = static_cast<float>(floor(in_rect.size.x * engine.current_scale));
    out_rect.h = static_cast<float>(floor(in_rect.size.y * engine.current_scale));

    if (world == DISPLAY_DONT_CENTER)
    {
        out_rect.x = static_cast<float>(floor((in_rect.position.x * engine.current_scale - in_rect.size.x * anchor.x)));
        out_rect.y = static_cast<float>(floor((in_rect.position.y * engine.current_scale - in_rect.size.y * anchor.y)));
        return;
    }
    reinterpret_cast<sre::rect2Df*>(&out_rect)->position = Game::World::worldToScreen(in_rect.position.x - in_rect.size.x * anchor.y, in_rect.position.y - in_rect.size.y * anchor.y, world ? &world->CurrentCamera : nullptr);
}

sre::vec2ut Display::GetCenter() { return {engine.center_x, engine.center_y}; }
sre::vec2i Display::GetSize() { return {engine.viewport.w, engine.viewport.h}; }
sre::vec2i Display::GetAbsoluteSize() { return {engine.osize_x, engine.osize_y}; }
float Display::GetScale() { return engine.viewport_scale; }

#define START_DRAW SDL_LockMutex(engine.sdl_rendermutex);
#define END_DRAW SDL_UnlockMutex(engine.sdl_rendermutex);

void Display::DrawLine(const sre::col4 &Color, const sre::vec2ut &Pt1, const sre::vec2ut &Pt2, const Game::World* world)
{
    START_DRAW

    sre::vec2f _pt1, _pt2;
    real_coords(_pt1, Pt1, world);
    real_coords(_pt2, Pt2, world);

    SDL_SetRenderDrawColor(engine.sdl_rendererhndl, Color.r, Color.g, Color.b, Color.a);
    SDL_RenderDrawLineF(engine.sdl_rendererhndl, _pt1.x, _pt1.y, _pt2.x, _pt2.y);

    END_DRAW
}

void Display::DrawLines(const sre::col4 &Color, int Count, const sre::vec2ut *Pts, const Game::World* world)
{
    START_DRAW

    ut_dynsalloc(sre::vec2f, _pts, Count);

    for (int i = 0; i < Count; i++)
        real_coords(_pts[i], Pts[i], world);

    SDL_SetRenderDrawColor(engine.sdl_rendererhndl, Color.r, Color.g, Color.b, Color.a);
    SDL_RenderDrawLinesF(engine.sdl_rendererhndl, reinterpret_cast<const SDL_FPoint *>(_pts), Count);

    END_DRAW
}

void Display::DrawRectangle(const sre::rect2Dut &Rectangle, const sre::col4 &Color, const sre::vec2f &AnchorPoint, DrawingMode Mode, const Game::World* world)
{
    START_DRAW

    SDL_FRect r;
    real_coords(r, Rectangle, AnchorPoint, world);

    SDL_SetRenderDrawColor(engine.sdl_rendererhndl, Color.r, Color.g, Color.b, Color.a);
    switch (Mode)
    {
    case dm_Stroke:
        SDL_RenderDrawRectF(engine.sdl_rendererhndl, &r);
        break;

    default:
        SDL_RenderFillRectF(engine.sdl_rendererhndl, &r);
        break;
    }

    END_DRAW
};

void Display::DrawRotatedRectangle(const sre::rect2Dut &_Rectangle, const double _angle, const sre::col4 &_Col, DrawingMode _dm, const Game::World* world)
{
    START_DRAW

    if (_angle == 0)
        return DrawRectangle(_Rectangle, _Col, sre::vec2f::CENTER, _dm, world);

    switch (_dm)
    {
    case dm_Stroke:
    {
        sre::vec2ut points[5] =
            {
                _Rectangle.rotated_topleft(_angle),
                _Rectangle.rotated_topright(_angle),
                _Rectangle.rotated_bottomright(_angle),
                _Rectangle.rotated_bottomleft(_angle),
                points[0]
        };
        DrawLines(_Col, 5, points, world);
    }
    break;
    default:
    {
        SDL_FRect r;
        real_coords(r, _Rectangle, sre::vec2f::CENTER, world);

        SDL_SetTextureColorMod(engine.sdl_rectTex, _Col.r, _Col.g, _Col.b);
        SDL_SetTextureAlphaMod(engine.sdl_rectTex, _Col.a);
        SDL_RenderCopyExF(engine.sdl_rendererhndl, engine.sdl_rectTex, NULL, &r, _angle, NULL, SDL_FLIP_NONE);
    }
    break;
    }

    END_DRAW
}

void Display::DrawDebug(const sre::vec2ut& _pos)
{
    START_DRAW

    SDL_FPoint pos;
    real_coords(*reinterpret_cast<sre::vec2f*>(&pos), _pos, Game::World::Current());

    SDL_SetRenderDrawColor(engine.sdl_rendererhndl, 255, 64, 0, 255);

    SDL_RenderDrawLineF(engine.sdl_rendererhndl, pos.x - DRAW_ENTCENTER_LINESIZE , pos.y, pos.x + DRAW_ENTCENTER_LINESIZE, pos.y);
    SDL_RenderDrawLineF(engine.sdl_rendererhndl, pos.x, pos.y - DRAW_ENTCENTER_LINESIZE, pos.x, pos.y + DRAW_ENTCENTER_LINESIZE);

    END_DRAW
}

void Display::DrawCircle(const sre::vec2ut &pos, const sre::unit radius, const sre::col4 &_Col, DrawingMode _dm, const Game::World* world)
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

void Display::DrawTexture(const Texture &_Texture, sre::rect2Dut Rectangle, const sre::col4 &Modulate, const sre::vec2f &AnchorPoint, const Game::World* world)
{
    START_DRAW

    if (Rectangle.size.x == 0 || Rectangle.size.y == 0)
        return;
        
    SDL_FRect render_rect;
    const int flip = (Rectangle.size.x < 0 ? ut_bit(0) : 0) | (Rectangle.size.y < 0 ? ut_bit(1) : 0);

    Rectangle.size.x = abs(Rectangle.size.x);
    Rectangle.size.y = abs(Rectangle.size.y);

    real_coords(render_rect, Rectangle, AnchorPoint, world);

    SDL_SetTextureColorMod((SDL_Texture *)_Texture.texture, Modulate.r, Modulate.g, Modulate.b);
    SDL_SetTextureAlphaMod((SDL_Texture *)_Texture.texture, Modulate.a);
    SDL_RenderCopyExF(engine.sdl_rendererhndl, static_cast<SDL_Texture *>(_Texture.texture), NULL, &render_rect, 0, NULL, static_cast<SDL_RendererFlip>(flip));

#if 00
    if (!SDL_GetRenderTarget(engine.sdl_rendererhndl))
    {
        SDL_SetRenderDrawColor(engine.sdl_rendererhndl, 255, 0, 0, 255);
        SDL_RenderDrawRectF(engine.sdl_rendererhndl, &render_rect);
    }
#endif

    END_DRAW
}

void Display::Fill(const sre::col4 &Color)
{
    START_DRAW

    SDL_SetRenderDrawColor(engine.sdl_rendererhndl, Color.r, Color.g, Color.b, Color.a);
    SDL_RenderFillRectF(engine.sdl_rendererhndl, NULL);

    END_DRAW
}
