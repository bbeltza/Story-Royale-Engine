#include "gl11.h"
#include <utils/math.h>
#include <math.h>
#include <assert.h>

// state

void sregl11_set_viewportstate(void* _inst, const sre_rect2Di* rectangle, sre_unit scale)
{
    sregl11_inst* inst = _inst;
    sregl_set_viewportstate(&inst->glfuncs, inst->common.window, rectangle);

    SRE_GLCALL(inst->glfuncs11.MatrixMode(GL_PROJECTION));
    SRE_GLCALL(inst->glfuncs11.LoadIdentity());
    SRE_GLCALL(inst->glfuncs11.Ortho(0, rectangle->w, rectangle->h, 0, 0, 1));
    SRE_GLCALL(inst->glfuncs11.MatrixMode(GL_MODELVIEW));
    inst->scale_cache = scale;
}

void sregl11_set_camerastate(void* _inst, sre_unit x, sre_unit y)
{
    sregl11_inst* inst = _inst;
    
    SRE_GLCALL(inst->glfuncs11.LoadIdentity()); 
    SRE_GLCALL(inst->glfuncs11.Translatef(x, y, 0.0f));                                                                        
    SRE_GLCALL(inst->glfuncs11.Scalef(inst->scale_cache, inst->scale_cache, 1));
}

void sregl11_set_texturestate(void* _inst, void* _texture)
{
    sregl11_inst* inst = _inst;
    sregl11_texture* texture = _texture;
    
    SRE_GLCALL(inst->glfuncs.BindTexture(GL_TEXTURE_2D, texture ? texture->texture.gltex : 0));
    inst->curtexture = texture;
}

// draw                     

void sregl11_draw1(void* _inst, const sre_RenderInstance1* instances, size_t instance_count)
{
    sregl11_inst* inst = _inst;
    sregl11_texture* texture = inst->curtexture;

    // Use matrices instead of calculating everything individiually (this one was made before and was found a lot slower than the next one)
    //      Since the other one calls glBegin/glEnd once, it prepares all of the necessary vertex data and sends it
    #if 0
    for (size_t i = 0; i < instance_count; i++)
    {
        SRE_GLCALL(inst->glfuncs11.PushMatrix());

        const sre_RenderInstance1* dinst = &instances[i];
        GLfloat minx = 0.0f - dinst->anchor.x;
        GLfloat miny = 0.0f - dinst->anchor.y;
        GLfloat maxx = 1.0f - dinst->anchor.x;
        GLfloat maxy = 1.0f - dinst->anchor.y;

        SRE_GLCALL(inst->glfuncs11.Translatef(dinst->rectangle.x, dinst->rectangle.y, 0));
        SRE_GLCALL(inst->glfuncs11.Rotatef((GLfloat)ut_deg(dinst->angle), 0, 0, 1));
        SRE_GLCALL(inst->glfuncs11.Scalef(dinst->rectangle.w, dinst->rectangle.h, 1));

        inst->glfuncs11.Begin(GL_QUADS);
            inst->glfuncs11.Color4ubv(&dinst->color.r);
            
            if (texture)
            {
                GLfloat minu = dinst->uv_offset.x;
                GLfloat minv = dinst->uv_offset.y;
                GLfloat maxu = minu + dinst->uv.x;
                GLfloat maxv = minv + dinst->uv.y;

                inst->glfuncs11.TexCoord2f(minu, minv);
                inst->glfuncs11.Vertex2f(minx, miny);
                inst->glfuncs11.TexCoord2f(maxu, minv);
                inst->glfuncs11.Vertex2f(maxx, miny);
                inst->glfuncs11.TexCoord2f(maxu, maxv);
                inst->glfuncs11.Vertex2f(maxx, maxy);
                inst->glfuncs11.TexCoord2f(minu, maxv);
                inst->glfuncs11.Vertex2f(minx, maxy);
            }
            else
            {
                inst->glfuncs11.Vertex2f(minx, miny);
                inst->glfuncs11.Vertex2f(maxx, miny);
                inst->glfuncs11.Vertex2f(maxx, maxy);
                inst->glfuncs11.Vertex2f(minx, maxy);
            }
            
        SRE_GLCALL(inst->glfuncs11.End());
        SRE_GLCALL(inst->glfuncs11.PopMatrix());
    }

    #else

    inst->glfuncs11.Begin(GL_QUADS);

    for (size_t i = 0; i < instance_count; i++)
    {
        const sre_RenderInstance1* dinst = &instances[i];

        GLfloat v[8];

        if (dinst->angle/360 == (int)dinst->angle/360)
        {
            GLfloat minx;
            GLfloat miny;
            GLfloat maxx;
            GLfloat maxy;
            minx = dinst->rectangle.x - dinst->anchor.x * dinst->rectangle.w;        
            miny = dinst->rectangle.y - dinst->anchor.y * dinst->rectangle.h;
            maxx = minx + dinst->rectangle.w;
            maxy = miny + dinst->rectangle.h;

            v[0] = minx;
            v[1] = miny;
            v[2] = maxx;
            v[3] = miny;
            v[4] = maxx;
            v[5] = maxy;
            v[6] = minx;
            v[7] = maxy;
        }
        else
        {
            GLfloat minx = (0.0f - dinst->anchor.x) * dinst->rectangle.w;
            GLfloat miny = (0.0f - dinst->anchor.y) * dinst->rectangle.h;
            GLfloat maxx = (1.0f - dinst->anchor.x) * dinst->rectangle.w;
            GLfloat maxy = (1.0f - dinst->anchor.y) * dinst->rectangle.h;

            v[0] = minx;
            v[1] = miny;
            v[2] = maxx;
            v[3] = miny;
            v[4] = maxx;
            v[5] = maxy;
            v[6] = minx;
            v[7] = maxy;
            
            GLfloat c = cosf(dinst->angle);
            GLfloat s = sinf(dinst->angle);
            for (int i = 0; i < 8; i+=2)
            {
                GLfloat x = v[i+0];
                GLfloat y = v[i+1];

                v[i+0] = (c*x - s*y) + dinst->rectangle.x;
                v[i+1] = (s*x + c*y) + dinst->rectangle.y;
            }
        }

        inst->glfuncs11.Color4ubv(&dinst->color.r);
        if (texture)
        {
            GLfloat minu = dinst->uv_offset.x;
            GLfloat minv = dinst->uv_offset.y;
            GLfloat maxu = minu + dinst->uv.x;
            GLfloat maxv = minv + dinst->uv.y;

            minu *= texture->xrange;
            maxu *= texture->xrange;
            minv *= texture->yrange;
            maxv *= texture->yrange;

            inst->glfuncs11.TexCoord2f(minu, minv);
            inst->glfuncs11.Vertex2f(v[0], v[1]);
            inst->glfuncs11.TexCoord2f(maxu, minv);
            inst->glfuncs11.Vertex2f(v[2], v[3]);
            inst->glfuncs11.TexCoord2f(maxu, maxv);
            inst->glfuncs11.Vertex2f(v[4], v[5]);
            inst->glfuncs11.TexCoord2f(minu, maxv);
            inst->glfuncs11.Vertex2f(v[6], v[7]);
        }
        else
        {
            inst->glfuncs11.Vertex2f(v[0], v[1]);
            inst->glfuncs11.Vertex2f(v[2], v[3]);
            inst->glfuncs11.Vertex2f(v[4], v[5]);
            inst->glfuncs11.Vertex2f(v[6], v[7]);
        }
    }

    SRE_GLCALL(inst->glfuncs11.End());
    #endif
}

void sregl11_draw2(void* _inst, const sre_RenderPoint* points, size_t point_count, sre_draw2primitive mode)
{
    sregl11_inst* inst = _inst;
    sregl11_texture* texture = inst->curtexture;

    GLenum primitivemode = sregl_mapmode(mode);
    inst->glfuncs11.Begin(primitivemode);

    float rangex = texture ? texture->xrange : 1;
    float rangey = texture ? texture->yrange : 1;

    for (size_t i = 0; i < point_count; i++)
    {
        const sre_RenderPoint* pt = &points[i];
        inst->glfuncs11.TexCoord2f(pt->uv.x * rangex, pt->uv.y * rangey);
        inst->glfuncs11.Color4ubv(&pt->color.r);
        inst->glfuncs11.Vertex2f(pt->pos.x, pt->pos.y);
    }

    SRE_GLCALL(inst->glfuncs11.End());

    #if 0
    const GLubyte col[4] = { 255, 0, 0, 255 };

    inst->glfuncs11.Begin(GL_LINE_LOOP);
    inst->glfuncs11.Color4ubv(col);
    for (size_t i = 0; i < point_count; i++)
    {
        const sre_RenderPoint* pt = &points[i];
        inst->glfuncs11.Vertex2f(pt->pos.x, pt->pos.y);
    }
    SRE_GLCALL(inst->glfuncs11.End());
    #endif
}
