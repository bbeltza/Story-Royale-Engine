#include <Components.h>

Game::Components::CustomTexture::CustomTexture()
{
    p_flags = p_Render;
}
Game::Components::CustomTexture::~CustomTexture()
{
    for (auto texturedata : m_FinalTextures)
        SDL_DestroyTexture(texturedata.texture);
}

Vector2i Game::Components::CustomTexture::getEntireSize()
{
    // Calculate the distance between the beginning side of the textures and the end side of the textures
    int minX = 0, minY = 0, maxX = 0, maxY = 0;
    for (auto texturedata : m_FinalTextures)
    {
        minX = minX < texturedata.offset.X ? minX : texturedata.offset.X;
        minY = minY < texturedata.offset.Y ? minY : texturedata.offset.Y;

        int w, h;

        SDL_QueryTexture(texturedata.texture, NULL, NULL, &w, &h);
        w += texturedata.offset.X;
        h += texturedata.offset.Y;


        maxX = maxX > w ? maxX : w;
        maxY = maxY > h ? maxY : h;
    }

    return {maxX - minX, maxY - minY};
}

void Game::Components::CustomTexture::AddFile(const char *path, std::list<Vector2f>& Offsets)
{
    File _f;
    _f.Load(path);
    if (!Engine->DrawingContext.LoadFileTexture(_f))
    {
        printf("Failed adding a file to CustomTexture. File is not a texture or hasn't been found\n");
        return;
    }

    int width, height;
    Uint32 format;
    SDL_QueryTexture((SDL_Texture*)_f.GetUserData(), &format, NULL, &width, &height);

    Vector2f *minXvec, *minYvec, *maxXvec, *maxYvec;
    minXvec = &Offsets.back();
    minYvec = &Offsets.back();
    maxXvec = &Offsets.back();
    maxYvec = &Offsets.back();

    for (auto& vec : Offsets)
    {
        if (minXvec->X > vec.X)
            minXvec = &vec;
        if (minYvec->Y > vec.Y)
            minYvec = &vec;

        if (maxXvec->X < vec.X)
            maxXvec = &vec;
        if (maxYvec->Y < vec.Y)
            maxYvec = &vec;
    }
    minXvec->Print();
    maxXvec->Print();
    minYvec->Print();
    maxYvec->Print();
    
    Vector2i textureSize = {
        width + (maxXvec->X - minXvec->X),
        height + (maxYvec->Y - minYvec->Y)
    };


    SDL_Texture* render_target = SDL_CreateTexture(Engine->DrawingContext.sdl_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, textureSize.X, textureSize.Y);
    SDL_SetTextureBlendMode(render_target, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(Engine->DrawingContext.sdl_renderer, render_target);
    SDL_SetRenderDrawColor(Engine->DrawingContext.sdl_renderer, 0, 0, 0, 0);
    SDL_RenderClear(Engine->DrawingContext.sdl_renderer);

    SDL_Rect r{0, 0, width, height};
    for (auto& vec : Offsets)
    {
        r.x = vec.X + (int)maxXvec->X;
        r.y = vec.Y + (int)maxYvec->Y;
        SDL_RenderCopy(Engine->DrawingContext.sdl_renderer, (SDL_Texture*)_f.GetUserData(), NULL, &r);
    }

    uint64_t *pixeldata = new uint64_t[textureSize.X * textureSize.Y];
    SDL_RenderReadPixels(Engine->DrawingContext.sdl_renderer, NULL, SDL_PIXELFORMAT_RGBA8888, pixeldata, textureSize.X*4);
    SDL_SetRenderTarget(Engine->DrawingContext.sdl_renderer, NULL);

    {
        SDL_Texture* target_texture = SDL_CreateTexture(Engine->DrawingContext.sdl_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, textureSize.X, textureSize.Y);
        m_FinalTextures.emplace_back(target_texture, minXvec->X, minYvec->Y);
    }

    SDL_UpdateTexture(m_FinalTextures.back().texture, NULL, pixeldata, textureSize.X*4);
    SDL_SetTextureBlendMode(m_FinalTextures.back().texture, SDL_BLENDMODE_BLEND);
    
    SDL_DestroyTexture(render_target);
    delete[] pixeldata;        
}

void Game::Components::CustomTexture::Render(Entity* _entity)
{
    Vector2f screenToWorld = Game::currentWorld->screenToWorldSpace(_entity->x, _entity->y);

    SDL_FRect r{0, 0, 0, 0};
    for (auto texturedata : m_FinalTextures)
    {
        r.x = texturedata.offset.X - screenToWorld.X;
        r.y = texturedata.offset.Y - screenToWorld.Y;
        int w, h;
        SDL_QueryTexture(texturedata.texture, NULL, NULL, &w, &h);
        r.w = w; r.h = h;

        SDL_RenderCopyF(Engine->DrawingContext.sdl_renderer, texturedata.texture, NULL, &r); 
    }
}