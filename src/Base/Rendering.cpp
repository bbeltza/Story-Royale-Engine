#include <standard>
#include "../internal.h"

#include "Base/Display.hpp"
#include "Base/Texture.hpp"

#include "Game/World.hpp"

#include "config.h"

#include "utils/mem.h"

inline void real_coords(Vector2f& out_pos, const Vector2ut& in_pos, const Game::World* world)
{
    if (world == DISPLAY_DONT_CENTER)
    {
        out_pos.X = static_cast<float>(round(in_pos.X * engine.current_scale));
        out_pos.Y = static_cast<float>(round(in_pos.Y * engine.current_scale));
        return;
    }
    out_pos = Game::World::worldToScreen(in_pos.X, in_pos.Y, world ? &world->CurrentCamera : nullptr);
}

inline void real_coords(SDL_FRect& out_rect, const RectUt& in_rect, const Vector2f& anchor, const Game::World* world)
{
    out_rect.w = static_cast<float>(floor(in_rect.Size.X * engine.current_scale));
    out_rect.h = static_cast<float>(floor(in_rect.Size.Y * engine.current_scale));

    if (world == DISPLAY_DONT_CENTER)
    {
        out_rect.x = static_cast<float>(floor((in_rect.Position.X * engine.current_scale - in_rect.Size.X * anchor.X)));
        out_rect.y = static_cast<float>(floor((in_rect.Position.Y * engine.current_scale - in_rect.Size.Y * anchor.Y)));
        return;
    }
    reinterpret_cast<RectF*>(&out_rect)->Position = Game::World::worldToScreen(in_rect.Position.X - in_rect.Size.X * anchor.X, in_rect.Position.Y - in_rect.Size.Y * anchor.Y, world ? &world->CurrentCamera : nullptr);
}

Vector2ut Display::GetCenter() { return {engine.center_x, engine.center_y}; }
Vector2i Display::GetSize() { return {engine.viewport.w, engine.viewport.h}; }
Vector2i Display::GetAbsoluteSize() { return {engine.osize_x, engine.osize_y}; }
float Display::GetScale() { return engine.viewport_scale; }

#define START_DRAW SDL_LockMutex(engine.sdl_rendermutex);
#define END_DRAW SDL_UnlockMutex(engine.sdl_rendermutex);

void Display::DrawLine(const Color4 &Color, const Vector2ut &Pt1, const Vector2ut &Pt2, const Game::World* world)
{
    START_DRAW

    Vector2f _pt1, _pt2;
    real_coords(_pt1, Pt1, world);
    real_coords(_pt2, Pt2, world);

    SDL_SetRenderDrawColor(engine.sdl_rendererhndl, Color.r, Color.g, Color.b, Color.a);
    SDL_RenderDrawLineF(engine.sdl_rendererhndl, _pt1.X, _pt1.Y, _pt2.X, _pt2.Y);

    END_DRAW
}

void Display::DrawLines(const Color4 &Color, int Count, const Vector2ut *Pts, const Game::World* world)
{
    START_DRAW

    ut_dynsalloc(Vector2f, _pts, Count);

    for (int i = 0; i < Count; i++)
        real_coords(_pts[i], Pts[i], world);

    SDL_SetRenderDrawColor(engine.sdl_rendererhndl, Color.r, Color.g, Color.b, Color.a);
    SDL_RenderDrawLinesF(engine.sdl_rendererhndl, reinterpret_cast<const SDL_FPoint *>(_pts), Count);

    END_DRAW
}

void Display::DrawRectangle(const RectUt &Rectangle, const Color4 &Color, const Vector2f &AnchorPoint, DrawingMode Mode, const Game::World* world)
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

void Display::DrawRotatedRectangle(const RectUt &_Rectangle, const double _angle, const Color4 &_Col, DrawingMode _dm, const Game::World* world)
{
    START_DRAW

    if (_angle == 0)
        return DrawRectangle(_Rectangle, _Col, Vector2f::CENTER, _dm, world);

    switch (_dm)
    {
    case dm_Stroke:
    {
        Vector2ut points[4] =
            {
                _Rectangle.getTopLeftRotated(_angle),
                _Rectangle.getTopRightRotated(_angle),
                _Rectangle.getBottomRightRotated(_angle),
                _Rectangle.getBottomLeftRotated(_angle)};
        DrawLines(_Col, 4, points, world);
    }
    break;
    default:
    {
        SDL_FRect r;
        real_coords(r, _Rectangle, Vector2f::CENTER, world);

        SDL_SetTextureColorMod(engine.sdl_rectTex, _Col.r, _Col.g, _Col.b);
        SDL_SetTextureAlphaMod(engine.sdl_rectTex, _Col.a);
        SDL_RenderCopyExF(engine.sdl_rendererhndl, engine.sdl_rectTex, NULL, &r, _angle, NULL, SDL_FLIP_NONE);
    }
    break;
    }

    END_DRAW
}

void Display::DrawDebug(const Vector2ut& _pos)
{
    START_DRAW

    SDL_FPoint pos;
    real_coords(*reinterpret_cast<Vector2f*>(&pos), _pos, Game::World::Current());

    SDL_SetRenderDrawColor(engine.sdl_rendererhndl, 255, 64, 0, 255);
    SDL_RenderDrawLineF(engine.sdl_rendererhndl, pos.x - DRAW_ENTCENTER_LINESIZE, pos.y, pos.x + DRAW_ENTCENTER_LINESIZE, pos.y);
    SDL_RenderDrawLineF(engine.sdl_rendererhndl, pos.x, pos.y - DRAW_ENTCENTER_LINESIZE, pos.x, pos.y + DRAW_ENTCENTER_LINESIZE);

    END_DRAW
}

void Display::DrawCircle(const Vector2ut &pos, const Unit radius, const Color4 &_Col, DrawingMode _dm, const Game::World* world)
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

void Display::DrawTexture(const Texture &_Texture, RectUt Rectangle, const Color4 &Modulate, const Vector2f &AnchorPoint, const Game::World* world)
{
    START_DRAW

    if (Rectangle.Size.X == 0 || Rectangle.Size.Y == 0)
        return;

    if (!_Texture.texture)
        Texture::load_textures();
        
    SDL_FRect render_rect;
    const int flip = (Rectangle.Size.X < 0 ? ut_bit(0) : 0) | (Rectangle.Size.Y < 0 ? ut_bit(1) : 0);

    Rectangle.Size.X = abs(Rectangle.Size.X);
    Rectangle.Size.Y = abs(Rectangle.Size.Y);

    real_coords(render_rect, Rectangle, AnchorPoint, world);

    SDL_SetTextureColorMod((SDL_Texture *)_Texture.texture, Modulate.r, Modulate.g, Modulate.b);
    SDL_SetTextureAlphaMod((SDL_Texture *)_Texture.texture, Modulate.a);
    SDL_RenderCopyExF(engine.sdl_rendererhndl, static_cast<SDL_Texture *>(_Texture.texture), NULL, &render_rect, 0, NULL, static_cast<SDL_RendererFlip>(flip));

#if 0
    if (!SDL_GetRenderTarget(engine.sdl_rendererhndl))
    {
        SDL_SetRenderDrawColor(engine.sdl_rendererhndl, 255, 0, 0, 255);
        SDL_RenderDrawRectF(engine.sdl_rendererhndl, &render_rect);
    }
#endif

    END_DRAW
}

void Display::Fill(const Color4 &Color)
{
    START_DRAW

    SDL_SetRenderDrawColor(engine.sdl_rendererhndl, Color.r, Color.g, Color.b, Color.a);
    SDL_RenderFillRectF(engine.sdl_rendererhndl, NULL);

    END_DRAW
}
