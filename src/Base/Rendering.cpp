#include <standard>
#include "../internal.h"

#include "Base/Display.hpp"
#include "Base/Texture.hpp"

#include "Game/World.hpp"

#include "config.h"

Vector2f Display::GetCenter() { return {engine.center_x, engine.center_y}; }
Vector2i Display::GetSize() { return {engine.viewport.w, engine.viewport.h}; }
Vector2i Display::GetAbsoluteSize() { return {engine.osize_x, engine.osize_y}; }

#define START_DRAW SDL_LockMutex(engine.sdl_rendermutex);
#define END_DRAW SDL_UnlockMutex(engine.sdl_rendermutex);

void Display::DrawLine(const Color4 &Color, const Vector2f &Pt1, const Vector2f &Pt2)
{
    START_DRAW

    SDL_SetRenderDrawColor(engine.sdl_rendererhndl, Color.r, Color.g, Color.b, Color.a);
    SDL_RenderDrawLineF(engine.sdl_rendererhndl, Pt1.X, Pt1.Y, Pt2.X, Pt2.Y);

    END_DRAW
}

void Display::DrawLines(const Color4 &Color, int Count, const Vector2f *Pts)
{
    START_DRAW

    SDL_SetRenderDrawColor(engine.sdl_rendererhndl, Color.r, Color.g, Color.b, Color.a);
    SDL_RenderDrawLinesF(engine.sdl_rendererhndl, reinterpret_cast<const SDL_FPoint *>(Pts), Count);

    END_DRAW
}

void Display::DrawRectangle(const RectF &Rectangle, const Color4 &Color, const Color4 &Modulate, const Vector2f &AnchorPoint, DrawingMode Mode)
{
    START_DRAW

    SDL_FRect r{
        Rectangle.Position.X - Rectangle.Size.X * AnchorPoint.X,
        Rectangle.Position.Y - Rectangle.Size.Y * AnchorPoint.Y,
        Rectangle.Size.X,
        Rectangle.Size.Y};
    Color4 abs_col = Color * Modulate;

    SDL_SetRenderDrawColor(engine.sdl_rendererhndl, abs_col.r, abs_col.g, abs_col.b, abs_col.a);
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

void Display::DrawRectangleAtWorld(RectF Rectangle, const Color4 &Color, const Color4 &Modulate, const Vector2f &AnchorPoint, DrawingMode Mode)
{
    START_DRAW

    Rectangle.Position = Game::World::worldToScreen(Rectangle.Position.X, Rectangle.Position.Y, Game::World::currentCamera());
    DrawRectangle(Rectangle, Color, Modulate, AnchorPoint, Mode);

    END_DRAW
}

void Display::DrawRotatedRectangle(const RectF &_Rectangle, const double _angle, const Color4 &_Col, DrawingMode _dm)
{
    START_DRAW

    if (_angle == 0)
        return DrawRectangle(_Rectangle, _Col);

    switch (_dm)
    {
    case dm_Stroke:
    {
        Vector2f points[4] =
            {
                _Rectangle.getTopLeftRotated(_angle),
                _Rectangle.getTopRightRotated(_angle),
                _Rectangle.getBottomRightRotated(_angle),
                _Rectangle.getBottomLeftRotated(_angle)};
        SDL_RenderDrawLinesF(engine.sdl_rendererhndl, (SDL_FPoint *)points, 4);
    }
    break;
    default:
    {
        SDL_FRect r{
            _Rectangle.getLeft(),
            _Rectangle.getTop(),
            _Rectangle.Size.X,
            _Rectangle.Size.Y};

        SDL_SetTextureColorMod(engine.sdl_rectTex, _Col.r, _Col.g, _Col.b);
        SDL_SetTextureAlphaMod(engine.sdl_rectTex, _Col.a);
        SDL_RenderCopyExF(engine.sdl_rendererhndl, engine.sdl_rectTex, NULL, &r, _angle, NULL, SDL_FLIP_NONE);
    }
    break;
    }

    END_DRAW
}

void Display::DrawRotatedRectangleAtWorld(const RectF &_Rectangle, const double _angle, const Color4 &_Col, DrawingMode _dm)
{
    START_DRAW

    Vector2f target_pos(_Rectangle.Position);
    target_pos = Game::World::worldToScreen(target_pos.X, target_pos.Y, Game::World::currentCamera());

    DrawRotatedRectangle(RectF(target_pos.X, target_pos.Y, _Rectangle.Size.X, _Rectangle.Size.Y), _angle, _Col, _dm);

    END_DRAW
}

void Display::DrawDebug(Vector2f pos) // Sounds weird to not pass as a reference, but it will allow us to get a copy to convert it into screen coordinates
{
    START_DRAW

    pos = Game::World::worldToScreen(pos.X, pos.Y, Game::World::currentCamera());

    SDL_SetRenderDrawColor(engine.sdl_rendererhndl, 255, 64, 0, 255);
    SDL_RenderDrawLineF(engine.sdl_rendererhndl, pos.X - DRAW_ENTCENTER_LINESIZE, pos.Y, pos.X + DRAW_ENTCENTER_LINESIZE, pos.Y);
    SDL_RenderDrawLineF(engine.sdl_rendererhndl, pos.X, pos.Y - DRAW_ENTCENTER_LINESIZE, pos.X, pos.Y + DRAW_ENTCENTER_LINESIZE);

    END_DRAW
}

void Display::DrawCircle(const Vector2f &pos, const float radius, const Color4 &_Col, DrawingMode _dm)
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

void Display::DrawTexture(Texture &_Texture, const RectF &Rectangle, const Color4 &Modulate, const Vector2f &AnchorPoint)
{
    START_DRAW

    if (Rectangle.Size.X == 0 || Rectangle.Size.Y == 0)
        return;

    if (!_Texture.texture)
        Texture::load_textures();

    float absW = abs(Rectangle.Size.X);
    float absH = abs(Rectangle.Size.Y);

    float rl = Rectangle.Position.X - absW * AnchorPoint.X;
    float rt = Rectangle.Position.Y - absH * AnchorPoint.Y;

    float left = roundf(rl * engine.integer_scale) / engine.integer_scale;
    float top = roundf(rt * engine.integer_scale) / engine.integer_scale;
    SDL_FRect render_rect{left, top, absW, absH};
    int flip = (Rectangle.Size.X < 0 ? ut_bit(0) : 0) | (Rectangle.Size.Y < 0 ? ut_bit(1) : 0);

    SDL_SetTextureColorMod((SDL_Texture *)_Texture.texture, Modulate.r, Modulate.g, Modulate.b);
    SDL_SetTextureAlphaMod((SDL_Texture *)_Texture.texture, Modulate.a);
    SDL_RenderCopyExF(engine.sdl_rendererhndl, (SDL_Texture *)_Texture.texture, NULL, &render_rect, 0, NULL, (SDL_RendererFlip)flip);

#if 0
    SDL_SetRenderDrawColor(engine.sdl_rendererhndl, 255, 0, 0, 255);
    SDL_RenderDrawRectF(engine.sdl_rendererhndl, &render_rect);
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