#include "gl21.h"

#include <math.h>

void sregl21_begin(void* _inst, const float clear[4])
{
    sregl21_inst* inst = _inst;
    SREGL_CLEAR(inst->glfuncs, clear);
}

void sregl21_end(void* _inst)
{
    sregl21_inst* inst = _inst;
    SREGL_PRESENT(inst->common21.common);
}

void sregl21_draw1(void* _inst,const sre_RenderInstance1* instances, size_t instance_count)
{
    sregl21_inst* inst = _inst;

    size_t vertex_count = instance_count*4;
    sregl21_vtassemblerreserve(inst, &inst->vtassembler, vertex_count);
    for (size_t i = 0; i < instance_count; i++)
    {
        const sre_RenderInstance1* dinst = &instances[i];

        sre_vec2ut v[4];

        // Vertex assembling code mostly taken from gl11/drawstate.c
        // Might want to make a better helper function to correctly assembling vertices if needed, so that render implementations don't have to bother a lot doing so
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

            v[0] = (sre_vec2ut){ minx, miny };
            v[1] = (sre_vec2ut){ maxx, miny };
            v[2] = (sre_vec2ut){ maxx, maxy };
            v[3] = (sre_vec2ut){ minx, maxy };
        }
        else
        {
            GLfloat minx = (0.0f - dinst->anchor.x) * dinst->rectangle.w;
            GLfloat miny = (0.0f - dinst->anchor.y) * dinst->rectangle.h;
            GLfloat maxx = (1.0f - dinst->anchor.x) * dinst->rectangle.w;
            GLfloat maxy = (1.0f - dinst->anchor.y) * dinst->rectangle.h;

            v[0] = (sre_vec2ut){ minx, miny };
            v[1] = (sre_vec2ut){ maxx, miny };
            v[2] = (sre_vec2ut){ maxx, maxy };
            v[3] = (sre_vec2ut){ minx, maxy };
            
            GLfloat c = cosf(dinst->angle);
            GLfloat s = sinf(dinst->angle);
            for (int i = 0; i < 4; i++)
            {
                sre_vec2ut pos = v[i];

                v[i].x = (c*pos.x - s*pos.y) + dinst->rectangle.x;
                v[i].y = (s*pos.x + c*pos.y) + dinst->rectangle.y;
            }
        }

        GLfloat minu = dinst->uv_offset.x;
        GLfloat minv = dinst->uv_offset.y;
        GLfloat maxu = minu + dinst->uv.x;
        GLfloat maxv = minv + dinst->uv.y;

        size_t ix4 = i*4;
        inst->vtassembler.arr[ix4 + 0] = (sre_RenderPoint){ v[0], { minu, minv }, dinst->color };
        inst->vtassembler.arr[ix4 + 1] = (sre_RenderPoint){ v[1], { maxu, minv }, dinst->color };
        inst->vtassembler.arr[ix4 + 2] = (sre_RenderPoint){ v[2], { maxu, maxv }, dinst->color };
        inst->vtassembler.arr[ix4 + 3] = (sre_RenderPoint){ v[3], { minu, maxv }, dinst->color };
    }

    sregl21_vtassemblerdata(inst, &inst->vtassembler, vertex_count);
    SRE_GLCALL(inst->glfuncs.DrawArrays(GL_QUADS, 0, (GLsizei)(vertex_count)));
}

void sregl21_draw2(void* _inst, const sre_RenderPoint* points, size_t point_count, sre_draw2primitive mode)
{
    sregl21_inst* inst = _inst;
    
    size_t size = point_count * sizeof(*points);
    if (size > inst->mainvbosize)
    {
        size_t bytesize = size * 2;
        SRE_GLCALL(inst->glfuncs21.BufferData(GL_ARRAY_BUFFER, (GLsizeiptr)bytesize, NULL, GL_DYNAMIC_DRAW));
        inst->mainvbosize = bytesize;
    }
    SRE_GLCALL(inst->glfuncs21.BufferSubData(GL_ARRAY_BUFFER, 0, size, points));

    GLenum glmode = sregl_mapmode(mode);
    SRE_GLCALL(inst->glfuncs.DrawArrays(glmode, 0, (GLsizei)point_count));
}