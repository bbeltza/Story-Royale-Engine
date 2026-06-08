#include "glcommon.h"
#include <stdlib.h>

#undef SRE_GLGETERROR
#define SRE_GLGETERROR glfuncs->GetError

bool sregl_texture_setup(struct sregl_functions* glfuncs, sregl_texture* texture, sre_SDLpixelFormat format, int w, int h, sre_SDLpixelFormat* outformat)
{
    SRE_GLCALLF(glfuncs->GenTextures(1, &texture->gltex));
    SRE_GLCALLF(glfuncs->BindTexture(GL_TEXTURE_2D, texture->gltex));
    SRE_GLCALL(glfuncs->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    SRE_GLCALL(glfuncs->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    SRE_GLCALL(glfuncs->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
    SRE_GLCALL(glfuncs->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
    SRE_GLCALL(glfuncs->TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));

    *outformat = SDL_PIXELFORMAT_RGBA32;
    return true;
}

bool sregl_texture_update(struct sregl_functions* glfuncs, sregl_texture* texture, const sre_rect2Di* region, const void* pixels, int pitch)
{
    SRE_GLCALLF(glfuncs->BindTexture(GL_TEXTURE_2D, texture->gltex));
    SRE_GLCALLF(glfuncs->PixelStorei(GL_PACK_ROW_LENGTH, pitch / 4));

    SRE_GLCALLF(glfuncs->TexSubImage2D(GL_TEXTURE_2D, 0, region->x, region->y, region->w, region->h, GL_RGBA, GL_UNSIGNED_BYTE, pixels));

    return true;
}

void sregl_texture_destroy(struct sregl_functions* glfuncs, sregl_texture* texture)
{
    SRE_GLCALL(glfuncs->DeleteTextures(1, &texture->gltex));
}

//

void sregl_set_vsync(bool enable)
{
    if (!enable) goto DISABLE;

    static bool check = false;
    if (check || SDL_GL_SetSwapInterval(-1) != 0)
    {
        SDL_GL_SetSwapInterval(1);
        check = true;
    }

    return;
    DISABLE:
        SDL_GL_SetSwapInterval(0);
}

void sregl_set_scissorstate(struct sregl_functions* glfuncs, SDL_Window* window, const sre_rect2Di* rectangle)
{
    int w, h;
    SDL_GL_GetDrawableSize(window, &w, &h);

    //if (rectangle)
    //{
        SRE_GLCALL(glfuncs->Enable(GL_SCISSOR_TEST));
        SRE_GLCALL(glfuncs->Scissor(
            rectangle->x,
            h - rectangle->y - rectangle->h,
            rectangle->w,
            rectangle->h
        ));
    //}
    //else
    //{
    //    SRE_GLCALL(glfuncs->Disable(GL_SCISSOR_TEST));
    //}
}

void sregl_set_blendstate(struct sregl_functions* glfuncs, sre_blendMode blendmode)
{
    if (0)
    {
        SRE_GLCALL(glfuncs->Disable(GL_BLEND));
        return;
    }

    GLenum sfactor;
    GLenum dfactor;
    switch (blendmode)
    {
        case SRE_BLEND_BLEND:
            sfactor = GL_SRC_ALPHA;
            dfactor = GL_ONE_MINUS_SRC_ALPHA;
            break;
        case SRE_BLEND_ADD:
            sfactor = GL_SRC_ALPHA;
            dfactor = GL_ONE;
            break;
        case SRE_BLEND_MOD:
            sfactor = GL_DST_COLOR;
            dfactor = GL_ZERO;
            break;
        case SRE_BLEND_MUL:
            sfactor = GL_DST_COLOR;
            dfactor = GL_ONE_MINUS_SRC_ALPHA;
            break;
        default: abort(); return;
    }

    SRE_GLCALL(glfuncs->Enable(GL_BLEND));
    SRE_GLCALL(glfuncs->BlendFunc(sfactor, dfactor));
    return;
}