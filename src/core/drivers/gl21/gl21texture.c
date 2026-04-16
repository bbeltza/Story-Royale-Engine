#include "gl21.h"

bool sregl21_setup_texture(void* _inst, void* _texture, sre_pixelFormat format, int w, int h, sre_pixelFormat* outformat)
{
    sregl21_inst* inst = _inst;
    sregl21_texture* texture = _texture;
    SRE_GLCTXMAKE(false);

    SRE_GLCALLF(inst->glfuncs.GenTextures(1, &texture->gltex));
    SRE_GLCALLF(inst->glfuncs.BindTexture(GL_TEXTURE_2D, texture->gltex));
    SRE_GLCALL(inst->glfuncs.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    SRE_GLCALL(inst->glfuncs.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    SRE_GLCALL(inst->glfuncs.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
    SRE_GLCALL(inst->glfuncs.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
    SRE_GLCALL(inst->glfuncs.TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));

    *outformat = SDL_PIXELFORMAT_RGBA32;
    return true;
}

bool sregl21_update_texture(void* _inst, void* _texture, const void* pixels, int pitch)
{
    sregl21_inst* inst = _inst;
    sregl21_texture* texture = _texture;
    SRE_GLCTXCHECK;

    int w, h;
    SRE_GLCALLF(inst->glfuncs.GetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w));
    SRE_GLCALLF(inst->glfuncs.GetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h));

    SRE_GLCALLF(inst->glfuncs.BindTexture(GL_TEXTURE_2D, texture->gltex));
    SRE_GLCALLF(inst->glfuncs.TexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels));

    return true;
}

void sregl21_destroy_texture(void* _inst, void* _texture)
{
    sregl21_inst* inst = _inst;
    sregl21_texture* texture = _texture;

    SRE_GLCALL(inst->glfuncs.DeleteTextures(1, &texture->gltex));
}