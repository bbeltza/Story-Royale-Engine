#include "GameSettings.hpp"
#include "Engine.hpp"
#include "Game/GuiLayer.hpp"
#include "ECS.hpp"

#include "Datatypes/Flags.hpp"

static Flags32 rendererFlags;

DrawingDevice::DrawingDevice(EngineClass* engine): m_Engine(engine)
{
    if (GameSettings::ScalingResolution)
    {
        Vector2i s = GameSettings::StartResolution / GameSettings::ScalingResolution;
        scale = s.getMin();
    }
    
    if (GameSettings::WindowOptions.VSync)
        rendererFlags.ToggleOn(SDL_RENDERER_PRESENTVSYNC);
}

DrawingDevice::~DrawingDevice()
{
    SDL_DestroyMutex(m_lockmutex);
    SDL_DestroyTexture(sdl_rectTexture);
}

void DrawingDevice::Setup()
{
    sdl_renderer = SDL_CreateRenderer(m_Engine->Window.sdl_window, -1, rendererFlags);
    SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_BLEND);
    
    sdl_rectTexture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, 1, 1);
    {
        uint64_t WHITE = UINT64_MAX;
        SDL_UpdateTexture(sdl_rectTexture, NULL, &WHITE, 4);
    }

    SDL_TryLockMutex(m_lockmutex);
}

void DrawingDevice::render()
{
#if 0
    load_textures();

    SDL_UnlockMutex(m_lockmutex);

    // Render current world
    if (Game::World::m_Current)
        renderCurrentWorld();
    else
    {
        Game::World::call_render();
        SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 0);
        SDL_RenderClear(sdl_renderer);
    }

    m_Engine->BeforeRender.Fire();

    // Drawing the Gui layer
    if (Game::GuiLayer::Current())
        renderCurrentUI();

    m_Engine->AfterRender.Fire();

    // Present the screen
    SDL_LockMutex(m_lockmutex);

    SDL_RenderPresent(sdl_renderer);
#endif
}

void DrawingDevice::processViewport()
{
    Vector2u outsize;
    SDL_GetRendererOutputSize(sdl_renderer, (int *)&outsize.X, (int *)&outsize.Y);

    if (GameSettings::ScalingResolution)
        scale = (outsize / GameSettings::ScalingResolution).getMin();
    scale = scale ? scale : 1;
    SDL_RenderSetScale(sdl_renderer, (float)scale, (float)scale);

    // Updating the viewport rect
    int oldw = m_viewport.w, oldh = m_viewport.h;
    SDL_RenderGetViewport(sdl_renderer, &m_viewport);
    if (oldw < m_viewport.w || oldh < m_viewport.h)
        reset_targets();
    
    auto layer = Game::GuiLayer::Current();
    if (!layer) return;
    
    layer->m_absolute.Size.X = (float)m_viewport.w;
    layer->m_absolute.Size.Y = (float)m_viewport.h;

    layer->_processchildren();
}

void DrawingDevice::load_textures()
{
    if (!Texture::to_load) return;
    while (!Texture::to_load->empty())
    {
        Texture& tex = *Texture::to_load->front();
        Texture::to_load->pop_front();

        tex.texture = SDL_CreateTextureFromSurface(sdl_renderer, (SDL_Surface*)tex.file_surface);
    }
}

void DrawingDevice::reset_targets()
{
    while (target_textures.size() > 1)
    {
        SDL_DestroyTexture(target_textures.back());
        target_textures.pop_back();
    }
}