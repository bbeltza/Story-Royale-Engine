#include <standard.h>

#include "ColorFunctions.h"
#include "Engine.h"
#include "ECS.h"
#include "GuiLayer.h"
#include "Components.h"
#include "GuiComponents.h"
#include "DrawingContext.h"

void DrawingDevice::getScreenCenter(unsigned int *x, unsigned int *y)
{
    if (x)
        *x = m_viewport.x;
    if (y)
        *y = m_viewport.y;
}

#define CHECK_LOCK \
    if (m_Locked)  \
        return;

void DrawingDevice::DrawRectangle(const Rect<int> &_Rectangle, const Color4 &_Col, DrawingMode _mode)
{
    CHECK_LOCK
    SDL_Rect r{
        _Rectangle.Position.X - _Rectangle.Size.X / 2,
        _Rectangle.Position.Y - _Rectangle.Size.Y / 2,
        _Rectangle.Size.X,
        _Rectangle.Size.Y};

    SDL_SetRenderDrawColor(sdl_renderer, _Col.r, _Col.g, _Col.b, _Col.a);
    if (_mode == dm_Stroke)
        SDL_RenderDrawRect(sdl_renderer, &r);
    else
        SDL_RenderFillRect(sdl_renderer, &r);
};

void DrawingDevice::DrawRectangleAtWorld(const RectI &_Rectangle, const Color4 &_Col, DrawingMode _mode)
{
    CHECK_LOCK
    Vector2i target_pos = _Rectangle.getTopLeft();
    if (Game::currentWorld)
        target_pos = Game::currentWorld->worldToScreenSpace(target_pos.X, target_pos.Y);
    else
        target_pos = Game::currentWorld->worldToScreen(target_pos.X, target_pos.Y); // If there's no world, then draw at the center, using a static function so it shouldn't crash

    SDL_Rect r{
        target_pos.X,
        target_pos.Y,
        _Rectangle.Size.X,
        _Rectangle.Size.Y};

    SDL_SetRenderDrawColor(sdl_renderer, _Col.r, _Col.g, _Col.b, _Col.a);
    if (_mode == dm_Stroke)
        SDL_RenderDrawRect(sdl_renderer, &r);
    else
        SDL_RenderFillRect(sdl_renderer, &r);
}

void DrawingDevice::DrawRotatedRectangle(const RectI &_Rectangle, const double _angle, const Color4 &_Col)
{
    CHECK_LOCK
    if (_angle == 0)
        return DrawRectangle(_Rectangle, _Col);

    SDL_Rect r{
        _Rectangle.getLeft(),
        _Rectangle.getTop(),
        _Rectangle.Size.X,
        _Rectangle.Size.Y};

    SDL_SetTextureColorMod(sdl_rectTexture, _Col.r, _Col.g, _Col.b);
    SDL_SetTextureAlphaMod(sdl_rectTexture, _Col.a);
    SDL_RenderCopyEx(sdl_renderer, sdl_rectTexture, NULL, &r, _angle, NULL, SDL_FLIP_NONE);
}

void DrawingDevice::DrawRotatedRectangleAtWorld(const RectI &_Rectangle, const double _angle, const Color4 &_Col)
{
    CHECK_LOCK
    Vector2i target_pos(_Rectangle.Position);
    if (Game::currentWorld)
        target_pos = Game::currentWorld->worldToScreenSpace(target_pos.X, target_pos.Y);
    else
        target_pos = Game::currentWorld->worldToScreen(target_pos.X, target_pos.Y); // If there's no world, then draw at the center, using a static function so it shouldn't crash

    return DrawRotatedRectangle(RectI(target_pos.X, target_pos.Y, _Rectangle.Size.X, _Rectangle.Size.Y), _angle, _Col);
}

void DrawingDevice::DrawDebug(Vector2i pos) // Sounds weird to not pass as a reference, but it will allow us to get a copy to convert it into world coordinates
{
    if (Game::currentWorld)
        pos = Game::currentWorld->worldToScreenSpace(pos.X, pos.Y);
    else
        pos = Game::currentWorld->worldToScreen(pos.X, pos.Y);

    SDL_SetRenderDrawColor(sdl_renderer, 255, 64, 0, 255);
    SDL_RenderDrawLineF(sdl_renderer, pos.X - ENTITY_DBGLINESIZE, pos.Y, pos.X + ENTITY_DBGLINESIZE, pos.Y);
    SDL_RenderDrawLineF(sdl_renderer, pos.X, pos.Y - ENTITY_DBGLINESIZE, pos.X, pos.Y + ENTITY_DBGLINESIZE);
}

void DrawingDevice::render()
{
    m_switchLock();
    tr();

    // Render current world
    if (Game::currentWorld)
        renderCurrentWorld();
    else
    {
        Game::currentWorld->render();
        SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 0);
        SDL_RenderClear(sdl_renderer);
    }

    m_Engine->BeforeRender.Fire(0);

    // Drawing the Gui layer
    if (Game::currentGuiLayer)
        renderCurrentUI();

    m_Engine->AfterRender.Fire(0);

    // Present the screen
    m_switchLock();
    SDL_RenderPresent(sdl_renderer);
}

void DrawingDevice::renderCurrentWorld()
{
    //// Aliases for the background and the foreground (so that typing Game::currentWorld wouldn't be necessary)
    Color3 &bg = Game::currentWorld->Background;
    Color4 &fg = Game::currentWorld->Foreground;

    //// Clearing the screen with the background color
    SDL_SetRenderDrawColor(sdl_renderer, bg.r, bg.g, bg.b, 255);
    SDL_RenderClear(sdl_renderer);

    //// Drawing all the entities (doesn't run if the foreground is full opaque)
    if (fg.a < 255)
    {
        Game::currentWorld->render();
    }

    //// Finally, filling the foreground (doesn't run if the foreground is invisible)
    // printf("%u, %u, %u, %u / %u, %u, %u\n", fg.r, fg.g, fg.b, fg.a, bg.r, bg.g, bg.b);
    if (fg.a)
    {
        SDL_SetRenderDrawColor(sdl_renderer, fg.r, fg.g, fg.b, fg.a);
        SDL_RenderFillRect(sdl_renderer, NULL);
    }
}

void DrawingDevice::renderCurrentUI()
{
    Color4 &uifg = Game::currentGuiLayer->Foreground;

    if (uifg.a < 255)
    {
        Game::currentGuiLayer->preRender();
        Game::currentGuiLayer->_render();
        Game::currentGuiLayer->postRender();
    }
    if (uifg.a)
    {
        SDL_SetRenderDrawColor(sdl_renderer, uifg.r, uifg.g, uifg.b, uifg.a);
        SDL_RenderFillRect(sdl_renderer, NULL);
    }
}

void DrawingDevice::DrawTexture(const RectI &_Rectangle, File &_File)
{
    if (!LoadFileTexture(_File))
        return;

    SDL_Rect render_rect{_Rectangle.getLeft(), _Rectangle.getTop(), _Rectangle.Size.X, _Rectangle.Size.Y};
    SDL_RenderCopy(sdl_renderer, (SDL_Texture *)_File.m_userdata, NULL, &render_rect);
}

bool DrawingDevice::LoadFileTexture(File &_File)
{
    //printf("%p\n", _File.m_userdata);
    if (_File.m_userdata) return true;

    if (_File.m_type != File::Type::Image)
    {
        printf("Invalid type, type must be IMAGE, current type is %d\n", _File.m_type);
        return false;
    }

    if (SDL_Texture *target_texture = m_LoadedTextures[_File.m_filepath])
    {
        _File.m_userdata = target_texture;
        return true;
    }
    else
    {
        SDL_RWops *temp_rw = SDL_RWFromConstMem(_File.m_info.data, _File.m_info.size);
        if (!temp_rw) goto err;
        m_LoadedTextures[_File.m_filepath] = IMG_LoadTexture_RW(sdl_renderer, temp_rw, 1);
        if (!m_LoadedTextures[_File.m_filepath]) goto err;
        _File.m_userdata = m_LoadedTextures[_File.m_filepath];
        return true;      
    }

    err:
    printf("%s\n", SDL_GetError());
    return false;
}

//

//

static Game::Camera zeroCam;

Vector2f Game::World::screenToWorld(int x, int y, Camera *cam)
{
    if (!cam)
        return screenToWorld(x, y, &zeroCam);
    int &cx = center[0], cy = center[1];
    return Vector2f(x - cx + cam->x, y - cy + cam->y);
}

Vector2i Game::World::worldToScreen(float x, float y, Camera *cam)
{
    if (!cam)
        return worldToScreen(x, y, &zeroCam);
    int &cx = center[0], cy = center[1];
    return Vector2i(x + cx - cam->x, y + cy - cam->y);
}

void DrawingDevice::processViewport()
{
    // Updating the viewport rect
    SDL_RenderGetViewport(sdl_renderer, &m_viewport);
    m_viewport.x = m_viewport.w / 2;
    m_viewport.y = m_viewport.h / 2;

    if (!Game::currentGuiLayer)
        return;
    Game::currentGuiLayer->p_absolute.w = (float)m_viewport.w;
    Game::currentGuiLayer->p_absolute.h = (float)m_viewport.h;

    Game::currentGuiLayer->_processchildren();
}