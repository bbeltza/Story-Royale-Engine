#include "opengl.h"

bool sreopengl_texcreate(const sre_videodriver* video, void* _texture, int w, int h, SDL_PixelFormatEnum format)
{
    const sre_videoOpenGL* inst = video->userdata;
    sre_GLtexture* texture = _texture;

    SRE_GL_CALL(glGenTextures(1, &texture->id), return false;);
    SRE_GL_CALL(glBindTexture(GL_TEXTURE_2D, texture->id));
    SRE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    SRE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    SRE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
    SRE_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
    
    texture->w = w;
    texture->h = h;
    texture->format = SDL_PIXELFORMAT_ARGB8888; // Hmmm custom format??
    
    //sre_log(SRE_LOGCATEGORY_DEBUG, "%s", SDL_GetPixelFormatName(format));
    SRE_GL_CALL(glBindTexture(GL_TEXTURE_2D, inst->basic_texture));

    return true;
}

bool sreopengl_texupdate(const sre_videodriver* video, void* _texture, const void* pixels, int pitch)
{
    const sre_videoOpenGL* inst = video->userdata;
    sre_GLtexture* texture = _texture;

    SRE_GL_CALL(glBindTexture(GL_TEXTURE_2D, texture->id));
                                                // Don't see this... I don't know, when I saw RGB5 I thought it was cool
    SRE_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->w, texture->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels), return false;);
    
    SRE_GL_CALL(glBindTexture(GL_TEXTURE_2D, inst->basic_texture));
    return true;
}

void sreopengl_texdestroy(const sre_videodriver* video, void* _texture)
{
    sre_GLtexture* texture = _texture;
    SRE_GL_CALL(glDeleteTextures(1, &texture->id));
}

bool sreopengl_texsize(const sre_videodriver* video, void* _texture, int* w, int* h)
{
    sre_GLtexture* texture = _texture;

    if (w)
        *w = texture->w;
    if (h)
        *h = texture->h;
    
    return true;
}

SDL_PixelFormatEnum sreopengl_texformat(const sre_videodriver* video, void* _texture)
{
    sre_GLtexture* texture = _texture;
    
    return texture->format;
}