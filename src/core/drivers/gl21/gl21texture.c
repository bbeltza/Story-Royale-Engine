#include "gl21.h"

bool sregl21_setup_texture(void* _inst, sre_Sampler* texture, sre_pixelFormat format, int w, int h)
{
    sregl21_inst* inst = _inst;
    SRE_GLCTXMAKE(false);

    SRE_GLCALLF(inst->glfuncs.GenTextures(1, &texture->gltex));
    SRE_GLCALLF(inst->glfuncs.BindTexture(GL_TEXTURE_2D, texture->gltex));
    SRE_GLCALL(inst->glfuncs.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    SRE_GLCALL(inst->glfuncs.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    SRE_GLCALL(inst->glfuncs.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
    SRE_GLCALL(inst->glfuncs.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));

    texture->w = w;
    texture->h = h;
    return true;
}

bool sregl21_update_texture(void* _inst, sre_Sampler* texture, const void* pixels, int pitch)
{
    sregl21_inst* inst = _inst;
    SRE_GLCTXCHECK;

    SRE_GLCALLF(inst->glfuncs.BindTexture(GL_TEXTURE_2D, texture->gltex));
    SRE_GLCALLF(inst->glfuncs.TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->w, texture->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels));

    return true;
}

bool sregl21_query_texture(void* _inst, sre_Sampler* texture, int size[2], sre_pixelFormat* format)
{
    if (size)
    {
        size[0] = texture->w;
        size[1] = texture->h;
    }
    if (format)
        *format = SDL_PIXELFORMAT_ABGR8888;

    return true;
}

void sregl21_destroy_texture(void* _inst, sre_Sampler* texture)
{
    sregl21_inst* inst = _inst;

    SRE_GLCALL(inst->glfuncs.DeleteTextures(1, &texture->gltex));
}