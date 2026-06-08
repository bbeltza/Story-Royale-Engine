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

        sre_vec2f v[4];

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

            v[0] = (sre_vec2f){ minx, miny };
            v[1] = (sre_vec2f){ maxx, miny };
            v[2] = (sre_vec2f){ maxx, maxy };
            v[3] = (sre_vec2f){ minx, maxy };
        }
        else
        {
            GLfloat minx = (0.0f - dinst->anchor.x) * dinst->rectangle.w;
            GLfloat miny = (0.0f - dinst->anchor.y) * dinst->rectangle.h;
            GLfloat maxx = (1.0f - dinst->anchor.x) * dinst->rectangle.w;
            GLfloat maxy = (1.0f - dinst->anchor.y) * dinst->rectangle.h;

            v[0] = (sre_vec2f){ minx, miny };
            v[1] = (sre_vec2f){ maxx, miny };
            v[2] = (sre_vec2f){ maxx, maxy };
            v[3] = (sre_vec2f){ minx, maxy };
            
            GLfloat c = cosf(dinst->angle);
            GLfloat s = sinf(dinst->angle);
            for (int i = 0; i < 4; i++)
            {
                sre_vec2f pos = v[i];

                v[i].x = (c*pos.x - s*pos.y) + dinst->rectangle.x;
                v[i].y = (s*pos.x + c*pos.y) + dinst->rectangle.y;
            }
        }

        GLfloat minu = dinst->uv_offset.x;
        GLfloat minv = dinst->uv_offset.y;
        GLfloat maxu = minu + dinst->uv.x;
        GLfloat maxv = minv + dinst->uv.y;

        size_t ix4 = i*4;
        inst->vtassembler.arr[ix4 + 0] = (sregl21_vertex){ v[0], { minu, minv }, dinst->color };
        inst->vtassembler.arr[ix4 + 1] = (sregl21_vertex){ v[1], { maxu, minv }, dinst->color };
        inst->vtassembler.arr[ix4 + 2] = (sregl21_vertex){ v[2], { maxu, maxv }, dinst->color };
        inst->vtassembler.arr[ix4 + 3] = (sregl21_vertex){ v[3], { minu, maxv }, dinst->color };
    }

    sregl21_vtassemblerdata(inst, &inst->vtassembler, vertex_count);
    SRE_GLCALL(inst->glfuncs.DrawArrays(GL_QUADS, 0, (GLsizei)(vertex_count)));
}

void sregl21_draw2(void* _inst, const sre_RenderInstance2* instance, size_t point_count)
{
    sregl21_inst* inst = _inst;
    
    sregl21_vtassemblerreserve(inst, &inst->vtassembler, point_count);
    for (size_t i = 0; i < point_count; i++)
    {
        inst->vtassembler.arr[i] = (sregl21_vertex){ {instance->points[i].pos.x, instance->points[i].pos.y}, instance->points[i].uv, instance->color };
    }

    sregl21_vtassemblerdata(inst, &inst->vtassembler, point_count);

    GLenum mode = sregl_mapmode(instance->mode);
    SRE_GLCALL(inst->glfuncs.DrawArrays(mode, 0, (GLsizei)point_count));
}