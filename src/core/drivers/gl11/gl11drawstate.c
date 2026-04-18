#include "gl11.h"
#include <utils/math.h>

// state

bool sregl11_set_camerastate(void* _inst, sre_unit x, sre_unit y)
{
    sregl11_inst* inst = _inst;
    inst->camera_cache[0] = x;
    inst->camera_cache[1] = y;
}

bool sregl11_set_viewportstate(void* _inst, int w, int h, sre_unit scale)
{
    sregl11_inst* inst = _inst;
    SRE_GLCALLF(inst->glfuncs.Viewport(0, 0, w, h));

    SRE_GLCALLF(glMatrixMode(GL_PROJECTION));
    SRE_GLCALLF(glLoadIdentity());
    SRE_GLCALLF(glOrtho(0, w, h, 0, 0, 1));
    SRE_GLCALLF(glMatrixMode(GL_MODELVIEW));
    inst->scale_cache = scale;
}

// draw

#define SREGL11_CHECK_FORSWITCHES() if (switch_flags & SRE_RENDER_SWITCHCAMERA)                                             \
                                    {                                                                                       \
                                        SRE_GLCALL(glLoadIdentity());                                                       \
                                        if (flags & SRE_DRAWFLAG_CAMERA)                                                    \
                                        {                                                                                   \
                                            SRE_GLCALL(glTranslatef(inst->camera_cache[0], inst->camera_cache[1], 0));      \
                                        }                                                                                   \
                                        SRE_GLCALL(glScalef(inst->scale_cache, inst->scale_cache, 1));                      \
                                                                                                                            \
                                    }                                                                                       \
                                                                                                                            \
                                    if (switch_flags & SRE_RENDER_SWITCHTEXTURE)                                            \
                                    {                                                                                       \
                                        SRE_GLCALL(inst->glfuncs.BindTexture(GL_TEXTURE_2D, texture ? texture->gltex : 0)); \
                                    }                       

void sregl11_flush_queueinstances1(void* _inst, void* _texture, const sre_RenderInstance1* instances, size_t instance_count, sre_u32 flags, sre_u32 switch_flags)
{
    sregl11_inst* inst = _inst;
    sregl_texture* texture = _texture;

    SREGL11_CHECK_FORSWITCHES()

    // Use matrices instead of calculating everything individiually (this one was made before and was found a lot slower than the next one)
    //      Since the other one calls glBegin/glEnd once, it prepares all of the necessary vertex data and sends it
    #if 0
    for (size_t i = 0; i < instance_count; i++)
    {
        SRE_GLCALL(glPushMatrix());

        const sre_RenderInstance1* dinst = &instances[i];
        GLfloat minx = 0.0f - dinst->anchor.x;
        GLfloat miny = 0.0f - dinst->anchor.y;
        GLfloat maxx = 1.0f - dinst->anchor.x;
        GLfloat maxy = 1.0f - dinst->anchor.y;

        SRE_GLCALL(glTranslatef(dinst->rectangle.x, dinst->rectangle.y, 0));
        SRE_GLCALL(glRotatef(ut_deg(dinst->angle), 0, 0, 1));
        SRE_GLCALL(glScalef(dinst->rectangle.w, dinst->rectangle.h, 1));

        glBegin(GL_QUADS);
            glColor4ubv(&dinst->color.r);
            
            if (texture)
            {
                GLfloat minu = dinst->uv_offset.x;
                GLfloat minv = dinst->uv_offset.y;
                GLfloat maxu = minu + dinst->uv.x;
                GLfloat maxv = minv + dinst->uv.y;

                glTexCoord2f(minu, minv);
                glVertex2f(minx, miny);
                glTexCoord2f(maxu, minv);
                glVertex2f(maxx, miny);
                glTexCoord2f(maxu, maxv);
                glVertex2f(maxx, maxy);
                glTexCoord2f(minu, maxv);
                glVertex2f(minx, maxy);
            }
            else
            {
                glVertex2f(minx, miny);
                glVertex2f(maxx, miny);
                glVertex2f(maxx, maxy);
                glVertex2f(minx, maxy);
            }
            
        SRE_GLCALL(glEnd());
        SRE_GLCALL(glPopMatrix());
    }

    #else

    glBegin(GL_QUADS);

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

        glColor4ubv(&dinst->color.r);
        if (texture)
        {
            GLfloat minu = dinst->uv_offset.x;
            GLfloat minv = dinst->uv_offset.y;
            GLfloat maxu = minu + dinst->uv.x;
            GLfloat maxv = minv + dinst->uv.y;

            glTexCoord2f(minu, minv);
            glVertex2fv(v + 0);
            glTexCoord2f(maxu, minv);
            glVertex2fv(v + 2);
            glTexCoord2f(maxu, maxv);
            glVertex2fv(v + 4);
            glTexCoord2f(minu, maxv);
            glVertex2fv(v + 6);
        }
        else
        {
            glVertex2fv(v + 0);
            glVertex2fv(v + 2);
            glVertex2fv(v + 4);
            glVertex2fv(v + 6);
        }
    }

    SRE_GLCALL(glEnd());
    #endif
}

void sregl11_flush_queueinstances2(void* _inst, void* _texture, const sre_RenderInstance2* instance, size_t point_count, sre_u32 flags, sre_u32 switch_flags)
{
    sregl11_inst* inst = _inst;
    sregl_texture* texture = _texture;

    SREGL11_CHECK_FORSWITCHES()

    switch (instance->mode)
    {
        case SRE_DRAW2_JOINED: glBegin(GL_TRIANGLE_FAN); break;
        case SRE_DRAW2_STRIP: glBegin(GL_TRIANGLE_STRIP); break;
        case SRE_DRAW2_TRIANGLE: glBegin(GL_TRIANGLES); break;
        default: abort(); return;
    }

    glColor4ubv(&instance->color.r);
    for (size_t i = 0; i < point_count; i++)
    {
        const sre_RenderPoint* pt = &instance->points[i];
        glTexCoord2f(pt->uv.x, pt->uv.y);
        glVertex2f(pt->pos.x, pt->pos.y);
    }

    SRE_GLCALL(glEnd());
}
