#include <standard.h>

#include "Engine.h"
#include "Game/Camera.h"
#include "Game/World.h"
#include "Game/GuiLayer.h"
#include "Game/GuiComponent.h"

#include "Game/GuiComponents/Text.h"

#include "GameSettings.h"

#include "config.h"

Vector2f DrawingDevice::getScreenCenter()
{
    return {m_viewport.w / 2.0f, m_viewport.h / 2.0f};
}

#define CHECK_LOCK \
    if (m_Locked)  \
        return;

void DrawingDevice::DrawRectangle(const RectF &_Rectangle, const Color4 &_Col, DrawingMode _mode)
{
    CHECK_LOCK
    SDL_FRect r{
        _Rectangle.getLeft(),
        _Rectangle.getTop(),
        _Rectangle.Size.X,
        _Rectangle.Size.Y};

    SDL_SetRenderDrawColor(sdl_renderer, _Col.r, _Col.g, _Col.b, _Col.a);
    if (_mode == dm_Stroke)
        SDL_RenderDrawRectF(sdl_renderer, &r);
    else
        SDL_RenderFillRectF(sdl_renderer, &r);
};

void DrawingDevice::DrawRectangleAtWorld(const RectF &_Rectangle, const Color4 &_Col, DrawingMode _mode)
{
    CHECK_LOCK
    Vector2f target_pos = _Rectangle.getTopLeft();
    if (Game::World::m_Current)
        target_pos = Game::World::m_Current->worldToScreenSpace(target_pos.X, target_pos.Y);
    else
        target_pos = Game::World::worldToScreen(target_pos.X, target_pos.Y); // If there's no world, then draw at the center, this function is static

    SDL_FRect r{
        target_pos.X,
        target_pos.Y,
        _Rectangle.Size.X,
        _Rectangle.Size.Y};

    SDL_SetRenderDrawColor(sdl_renderer, _Col.r, _Col.g, _Col.b, _Col.a);
    if (_mode == dm_Stroke)
        SDL_RenderDrawRectF(sdl_renderer, &r);
    else
        SDL_RenderFillRectF(sdl_renderer, &r);
}

void DrawingDevice::DrawRotatedRectangle(const RectF &_Rectangle, const double _angle, const Color4 &_Col, DrawingMode _dm)
{
    CHECK_LOCK
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
                _Rectangle.getBottomLeftRotated(_angle)
            };
            SDL_RenderDrawLinesF(sdl_renderer, (SDL_FPoint*)points, 4);
        }
        break;
    default:
        {
            SDL_FRect r{
            _Rectangle.getLeft(),
            _Rectangle.getTop(),
            _Rectangle.Size.X,
            _Rectangle.Size.Y};

            SDL_SetTextureColorMod(sdl_rectTexture, _Col.r, _Col.g, _Col.b);
            SDL_SetTextureAlphaMod(sdl_rectTexture, _Col.a);
            SDL_RenderCopyExF(sdl_renderer, sdl_rectTexture, NULL, &r, _angle, NULL, SDL_FLIP_NONE);
        }
        break;
    }
}

void DrawingDevice::DrawRotatedRectangleAtWorld(const RectF &_Rectangle, const double _angle, const Color4 &_Col, DrawingMode _dm)
{
    CHECK_LOCK
    Vector2f target_pos(_Rectangle.Position);
    if (Game::World::m_Current)
        target_pos = Game::World::m_Current->worldToScreenSpace(target_pos.X, target_pos.Y);
    else
        target_pos = Game::World::worldToScreen(target_pos.X, target_pos.Y);

    return DrawRotatedRectangle(RectF(target_pos.X, target_pos.Y, _Rectangle.Size.X, _Rectangle.Size.Y), _angle, _Col, _dm);
}

void DrawingDevice::DrawDebug(Vector2f pos) // Sounds weird to not pass as a reference, but it will allow us to get a copy to convert it into screen coordinates
{
    CHECK_LOCK
    if (Game::World::m_Current)
        pos = Game::World::m_Current->worldToScreenSpace(pos.X, pos.Y);
    else
        pos = Game::World::worldToScreen(pos.X, pos.Y);

    SDL_SetRenderDrawColor(sdl_renderer, 255, 64, 0, 255);
    SDL_RenderDrawLineF(sdl_renderer, pos.X - DRAW_ENTCENTER_LINESIZE, pos.Y, pos.X + DRAW_ENTCENTER_LINESIZE, pos.Y);
    SDL_RenderDrawLineF(sdl_renderer, pos.X, pos.Y - DRAW_ENTCENTER_LINESIZE, pos.X, pos.Y + DRAW_ENTCENTER_LINESIZE);
}

void DrawingDevice::DrawCircle(const Vector2f& _Pos, const float _Radius, const Color4& _Col, DrawingMode _dm)
{
    CHECK_LOCK;

    SDL_RenderFillCircleF(sdl_renderer, _Pos.X, _Pos.Y, _Radius);
}

void DrawingDevice::render()
{
    m_switchLock();
    tr();

    // Render current world
    if (Game::World::m_Current)
        renderCurrentWorld();
    else
    {
        Game::World::call_render();
        SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 0);
        SDL_RenderClear(sdl_renderer);
    }

    m_Engine->BeforeRender.Fire(0);

    // Drawing the Gui layer
    if (Game::GuiLayer::Current())
        renderCurrentUI();

    m_Engine->AfterRender.Fire(0);

    // Present the screen
    m_switchLock();
    SDL_RenderPresent(sdl_renderer);
}

void DrawingDevice::renderCurrentWorld()
{
    auto world = Game::World::m_Current;

    //// Aliases for the background and the foreground (so that typing Game::currentWorld wouldn't be necessary)
    Color3 &bg = world->Background;
    Color4 &fg = world->Foreground;

    //// Clearing the screen with the background color
    SDL_SetRenderDrawColor(sdl_renderer, bg.r, bg.g, bg.b, 255);
    SDL_RenderClear(sdl_renderer);

    //// Drawing all the entities (doesn't run if the foreground is full opaque)
    if (fg.a < 255)
    {
        world->call_render();
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
    auto layer = Game::GuiLayer::Current();

    Color4 &uifg = layer->Foreground;

    if (uifg.a < 255)
    {
        layer->preRender();
        layer->_renderchildren();
        layer->postRender();
    }
    if (uifg.a)
    {
        SDL_SetRenderDrawColor(sdl_renderer, uifg.r, uifg.g, uifg.b, uifg.a);
        SDL_RenderFillRect(sdl_renderer, NULL);
    }
}

void DrawingDevice::DrawTexture(const RectF& _Rectangle, File &_File)
{
    if (!LoadFileTexture(_File))
        return;
    
    float left = roundf(_Rectangle.getLeft() * scale) / scale;
    float top = roundf(_Rectangle.getTop() * scale) / scale;
    SDL_FRect render_rect{left, top, _Rectangle.Size.X, _Rectangle.Size.Y};
    SDL_RenderCopyF(sdl_renderer, (SDL_Texture *)_File.m_userdata, NULL, &render_rect);
}

void DrawingDevice::DrawFont(const SDL_Rect *_Bounds, File &_FontFile, const char *text, int count, uint8_t alignment)
{
    static char font_characters[96];
    if (!font_characters[0])
    {
        for (char i = 32; i < 127; i++)
        {
            font_characters[i - 32] = i;
        }
        font_characters[95] = 0;
    }

    if (_FontFile.m_type != File::Type::Font)
        return;

    if (TTF_Font *target_font = m_LoadedFonts[_FontFile.m_filepath])
    {
        _FontFile.m_userdata = target_font;
    }
    else
    {
        SDL_RWops *temp_rw = SDL_RWFromConstMem(_FontFile.getInfo().data, _FontFile.getInfo().size);
        m_LoadedFonts[_FontFile.m_filepath] = TTF_OpenFontRW(temp_rw, 1, 12);

        SDL_Surface *temp_surf = TTF_RenderText_Solid(m_LoadedFonts[_FontFile.m_filepath],
                                                      font_characters, {255, 255, 255});
        m_LoadedTextures[_FontFile.m_filepath] = SDL_CreateTextureFromSurface(sdl_renderer, temp_surf);
        SDL_FreeSurface(temp_surf);
        _FontFile.m_userdata = m_LoadedFonts[_FontFile.m_filepath];
    }
    
    int h, max_w;
    SDL_QueryTexture(m_LoadedTextures[_FontFile.m_filepath], NULL, NULL, &max_w, &h);

    int max_count, linelength;
    TTF_MeasureTextSpaced(m_LoadedFonts[_FontFile.m_filepath], text, _Bounds->w, &linelength, &max_count);

    char *linetest = new char[strlen(text) + 1];

    strncpy(linetest, text, max_count);
    linetest[max_count] = 0;
    TTF_MeasureTextSpaced(m_LoadedFonts[_FontFile.m_filepath], linetest, _Bounds->w, &linelength, NULL);

    int x = 0;
    int y = 0;
    char constchar[2] = {0, 0};

    unsigned int i = 0;
    unsigned int c = count;
    while (text[i] && (i < c))
    {
        const char &character = text[i];
        if (i > unsigned(max_count))
        {
        new_line:
            x = 0;
            y += h;

            TTF_MeasureTextSpaced(m_LoadedFonts[_FontFile.m_filepath], text + i, _Bounds->w, NULL, &max_count);

            strncpy(linetest, text + i, max_count);
            linetest[max_count] = 0;
            TTF_MeasureTextSpaced(m_LoadedFonts[_FontFile.m_filepath], linetest, _Bounds->w, &linelength, &max_count);
            max_count += i - 1;
            continue;
        }
        switch (character)
        {
        case ' ':
            x += 3;
            i++;
            continue;
        case '\n':
            i++;
            goto new_line;
            continue;
        default:
            break;
        };
        SDL_Rect src = {0, 0, 0, 0}, dest = {0, 0, 0, 0};
        src.h = h;
        dest.h = h;

        constchar[0] = character;
        TTF_MeasureText(m_LoadedFonts[_FontFile.m_filepath], constchar, max_w, &src.w, NULL);
        if (!src.w)
            continue;

        font_characters[character - 32] = 0;
        TTF_MeasureText(m_LoadedFonts[_FontFile.m_filepath], font_characters, max_w, &src.x, NULL);
        font_characters[character - 32] = character;

        if (character == 'i' || character == 'n')
            --src.w;
        dest.w = src.w;

        dest.x = _Bounds->x + x;
        switch (alignment)
        {
        case GuiComponents::Text::AlCentered:
            dest.x += (_Bounds->w - linelength) / 2;
            break;
        case GuiComponents::Text::AlRight:
            dest.x += (_Bounds->w - linelength);
            break;
        default:
            break;
        }
        dest.y = _Bounds->y + y;

        SDL_RenderCopy(sdl_renderer, m_LoadedTextures[_FontFile.m_filepath], &src, &dest);

        SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 255, 255);

        x += src.w;
        i++;
    }

    delete[] linetest;
}

bool DrawingDevice::LoadFileTexture(File &_File)
{
    // printf("%p\n", _File.m_userdata);
    if (_File.m_userdata)
        return true;

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
        SDL_RWops *temp_rw = SDL_RWFromConstMem(_File.getInfo().data, _File.getInfo().size);
        if (!temp_rw)
            goto err;
        m_LoadedTextures[_File.m_filepath] = IMG_LoadTexture_RW(sdl_renderer, temp_rw, 1);
        if (!m_LoadedTextures[_File.m_filepath])
            goto err;
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

Vector2f Game::World::screenToWorld(float x, float y, Camera *cam)
{
    if (!cam)
        return screenToWorld(x, y, &zeroCam);
    return Vector2f(x - center.X + cam->x, y - center.Y + cam->y);
}

Vector2f Game::World::worldToScreen(float x, float y, Camera *cam)
{
    if (!cam)
        return worldToScreen(x, y, &zeroCam);
    return Vector2f(x + center.X - cam->x, y + center.Y - cam->y);
}

void DrawingDevice::processViewport()
{
    // Updating the viewport rect
    SDL_RenderGetViewport(sdl_renderer, &m_viewport);

    Vector2u viewportsize;
    SDL_GetRendererOutputSize(sdl_renderer, (int *)&viewportsize.X, (int *)&viewportsize.Y);

    if (GameSettings::ScalingResolution)
        scale = (viewportsize / GameSettings::ScalingResolution).getMin();
    scale = scale ? scale : 1;
    SDL_RenderSetScale(sdl_renderer, (float)scale, (float)scale);
    
    auto layer = Game::GuiLayer::Current();
    if (!layer) return;
    
    layer->m_absolute.Size.X = (float)m_viewport.w;
    layer->m_absolute.Size.Y = (float)m_viewport.h;

    layer->_processchildren();
}