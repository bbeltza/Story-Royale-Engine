#include <standard.h>

#include "Engine.h"
#include "Game/Camera.h"
#include "Game/World.h"
#include "Game/GuiLayer.h"
#include "Game/GuiComponent.h"

#include "Game/GuiComponents/Text.h"

#include "GameSettings.h"
#include "Sys.h"

#include "config.h"

Vector2f DrawingDevice::getScreenCenter()
{
    return {m_viewport.w / 2.0f, m_viewport.h / 2.0f};
}

#define START_DRAW SDL_LockMutex(m_lockmutex);
#define END_DRAW SDL_UnlockMutex(m_lockmutex);

void DrawingDevice::DrawRectangle(const RectF& Rectangle, const Color4& Color, const Color4& Modulate, const Vector2f &AnchorPoint, DrawingMode Mode)
{
    START_DRAW

    SDL_FRect r{
        Rectangle.Position.X - Rectangle.Size.X * AnchorPoint.X,
        Rectangle.Position.Y - Rectangle.Size.Y * AnchorPoint.Y,
        Rectangle.Size.X,
        Rectangle.Size.Y};

    SDL_SetRenderDrawColorMod(sdl_renderer, (SDL_Color*)&Color, (SDL_Color*)&Modulate);
    switch (Mode)
    {
    case dm_Stroke:
        SDL_RenderDrawRectF(sdl_renderer, &r);
        break;
    
    default:
        SDL_RenderFillRectF(sdl_renderer, &r);
        break;
    }

    END_DRAW
};

void DrawingDevice::DrawRectangleAtWorld(RectF Rectangle, const Color4& Color, const Color4& Modulate, const Vector2f &AnchorPoint, DrawingMode Mode)
{
    START_DRAW

    Rectangle.Position = Game::World::worldToScreen(Rectangle.Position.X, Rectangle.Position.Y, Game::World::currentCamera());
    DrawRectangle(Rectangle, Color, Modulate, AnchorPoint, Mode);

    END_DRAW
}

void DrawingDevice::DrawRotatedRectangle(const RectF &_Rectangle, const double _angle, const Color4 &_Col, DrawingMode _dm)
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

    END_DRAW
}

void DrawingDevice::DrawRotatedRectangleAtWorld(const RectF &_Rectangle, const double _angle, const Color4 &_Col, DrawingMode _dm)
{
    START_DRAW

    Vector2f target_pos(_Rectangle.Position);
    if (Game::World::m_Current)
        target_pos = Game::World::m_Current->worldToScreenSpace(target_pos.X, target_pos.Y);
    else
        target_pos = Game::World::worldToScreen(target_pos.X, target_pos.Y);

    DrawRotatedRectangle(RectF(target_pos.X, target_pos.Y, _Rectangle.Size.X, _Rectangle.Size.Y), _angle, _Col, _dm);

    END_DRAW
}

void DrawingDevice::DrawDebug(Vector2f pos) // Sounds weird to not pass as a reference, but it will allow us to get a copy to convert it into screen coordinates
{
    START_DRAW

    if (Game::World::m_Current)
        pos = Game::World::m_Current->worldToScreenSpace(pos.X, pos.Y);
    else
        pos = Game::World::worldToScreen(pos.X, pos.Y);

    SDL_SetRenderDrawColor(sdl_renderer, 255, 64, 0, 255);
    SDL_RenderDrawLineF(sdl_renderer, pos.X - DRAW_ENTCENTER_LINESIZE, pos.Y, pos.X + DRAW_ENTCENTER_LINESIZE, pos.Y);
    SDL_RenderDrawLineF(sdl_renderer, pos.X, pos.Y - DRAW_ENTCENTER_LINESIZE, pos.X, pos.Y + DRAW_ENTCENTER_LINESIZE);

    END_DRAW
}

void DrawingDevice::DrawCircle(const Vector2f& _Pos, const float _Radius, const Color4& _Col, DrawingMode _dm)
{
    START_DRAW

    SDL_RenderFillCircleF(sdl_renderer, _Pos.X, _Pos.Y, _Radius);

    END_DRAW
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

void DrawingDevice::DrawTexture(Texture& _Texture, const RectF& Rectangle, const Color4& Modulate, const Vector2f& AnchorPoint)
{
    START_DRAW

    if (Rectangle.Size.X == 0 || Rectangle.Size.Y == 0)
        return;

    float absW = abs(Rectangle.Size.X);
    float absH = abs(Rectangle.Size.Y);

    float rl = Rectangle.Position.X - absW * AnchorPoint.X;
    float rt = Rectangle.Position.Y - absH * AnchorPoint.Y;

    float left = roundf(rl * scale) / scale;
    float top = roundf(rt * scale) / scale;
    SDL_FRect render_rect{left, top, absW, absH};
    int flip = (Rectangle.Size.X < 0 ? bit(0) : 0) | (Rectangle.Size.Y < 0 ? bit(1) : 0);

    SDL_RenderCopyExF(sdl_renderer, (SDL_Texture *)_Texture.texture, NULL, &render_rect, 0, NULL, (SDL_RendererFlip)flip);

    #if 0
    SDL_SetRenderDrawColor(sdl_renderer, 255, 0, 0, 255);
    SDL_RenderDrawRectF(sdl_renderer, &render_rect);
    #endif

    END_DRAW
}

void DrawingDevice::DrawFont(const SDL_Rect *_Bounds, const Color3& Color, File &_FontFile, const char *text, int count, uint8_t alignment)
{
    START_DRAW

    static char font_characters[96];
    if (!font_characters[0])
    {
        for (char i = 32; i < 127; i++)
        {
            font_characters[i - 32] = i;
        }
        font_characters[95] = 0;
    }

    if (TTF_Font *target_font = m_LoadedFonts[_FontFile.m_filepath])
    {
        _FontFile.m_userdata = target_font;
    }
    else
    {
        SDL_RWops *temp_rw = SDL_RWFromConstMem(_FontFile.getInfo().data, _FontFile.getInfo().size);
        TTF_Font* font = TTF_OpenFontRW(temp_rw, 1, 12);
        if (!font)
            {syserror(SDL_ERROR, "Failed loading font", SDL_GetError()); return;}
        m_LoadedFonts[_FontFile.m_filepath] = font;


        _FontFile.m_type = File::T_TTF;

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

        SDL_SetTextureColorMod(m_LoadedTextures[_FontFile.m_filepath], Color.r, Color.g, Color.b);
        SDL_RenderCopy(sdl_renderer, m_LoadedTextures[_FontFile.m_filepath], &src, &dest);

        SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 255, 255);

        x += src.w;
        i++;
    }

    delete[] linetest;

    END_DRAW
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

// LEGACY

bool DrawingDevice::LoadFileTexture(File &_File)
{
    // printf("%p\n", _File.m_userdata);
    if (_File.m_userdata)
        return true;

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

        if (IMG_isPNG(temp_rw))
            _File.m_type = File::T_PNG;

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

void DrawingDevice::LegacyDrawTexture(const RectF& _Rectangle, File &_File)
{
    if (!LoadFileTexture(_File))
        return;
    if (_Rectangle.Size.X == 0 || _Rectangle.Size.Y == 0)
        return;

    START_DRAW
    
    float left = roundf(_Rectangle.getLeft() * scale) / scale;
    float top = roundf(_Rectangle.getTop() * scale) / scale;
    SDL_FRect render_rect{left, top, abs(_Rectangle.Size.X), abs(_Rectangle.Size.Y)};
    int flip = (_Rectangle.Size.X < 0 ? bit(0) : 0) | (_Rectangle.Size.Y < 0 ? bit(1) : 0);

    SDL_RenderCopyExF(sdl_renderer, (SDL_Texture *)_File.m_userdata, NULL, &render_rect, 0, NULL, (SDL_RendererFlip)flip);

    END_DRAW
}
