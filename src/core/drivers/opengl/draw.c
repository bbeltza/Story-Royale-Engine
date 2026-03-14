#include "opengl.h"

#define NORM_RGBA(x, f) (x).r/255.0##f, (x).g/255.0##f, (x).b/255.0##f, (x).a/255.0##f

bool sreopengl_drawfill(const sre_videodriver* video, const sre_DDFill* data)
{
    const sre_videoOpenGL* inst = video->userdata;
    bool res = true;

    SRE_GL_CALL(inst->funcs2.glUseProgram(inst->basicfill_program), return false;);
    SRE_GL_CALL(inst->funcs2.glUniform4f(inst->basicfill_program_uniform_color, NORM_RGBA(data->color, f)), abort(););
        
    SRE_GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, NULL), res = false;);

    SRE_GL_CALL(inst->funcs2.glUseProgram(inst->basic_program));

    return res;
}

static const GLfloat IDENTITY[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
static bool common_drawrect(const sre_videoOpenGL* inst, const sre_DDRect* data)
{
    const GLfloat mat[16] = {
        data->rect.size.x, 0.0f, 0.0f, 0.0f,
        0.0f, data->rect.size.y, 0.0f, 0.0f,
        0.0f, 0.0f,              1.0f, 0.0f,
        data->rect.position.x, data->rect.position.y, 0.0f, 1.0f
    };

    const GLfloat* cam;
    if (data->flags & SRE_DRAWFLAGS_USECAM)
        cam = inst->camera_view;
    else
        cam = IDENTITY;
    
    SRE_GL_CALL(inst->funcs2.glUniformMatrix4fv(inst->basic_program_state_cameraview, 1, GL_FALSE, cam));

    SRE_GL_CALL(inst->funcs2.glUniform4f(inst->basic_program_uniform_color, NORM_RGBA(data->color, f)));
    SRE_GL_CALL(inst->funcs2.glUniform2fv(inst->basic_program_uniform_anchor, 1, &data->anchor.x));
    SRE_GL_CALL(inst->funcs2.glUniformMatrix4fv(inst->basic_program_uniform_model, 1, GL_FALSE, mat));
    
    SRE_GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, NULL), return false;);
    return true;
}

bool common_setrotation(const sre_videoOpenGL* inst, double angle)
{
    const GLfloat rad = (float)ut_rad(angle);
    const GLfloat ctheta = cosf(rad);
    const GLfloat stheta = sinf(rad);
    const GLfloat rotation[16] = {
        ctheta, -stheta, 0, 0,
        stheta, ctheta, 0, 0,
        0, 0,           1, 0,
        0, 0,           0, 1
    };

    SRE_GL_CALL(inst->funcs2.glUniformMatrix4fv(inst->basic_program_uniform_rotation, 1, GL_FALSE, rotation), return false;);
    return true;
}

bool common_drawtex(const sre_videoOpenGL* inst, const sre_DDTexture* data)
{
    const sre_GLtexture* texture = sre_get_texture(data->texture);
    if (!texture)
        return false;

    SRE_GL_CALL(glBindTexture(GL_TEXTURE_2D, texture->id));

    SRE_GL_CALL(inst->funcs2.glUniform2f(inst->basic_program_uniform_flip,
                data->flags & SRE_DRAWFLAGS_FLIPX ? -1.0f : 1.0f,
                data->flags & SRE_DRAWFLAGS_FLIPY ? -1.0f : 1.0f
        ));

    if (!data->region.w && !data->region.h)
    {
        SRE_GL_CALL(inst->funcs2.glUniform4f(inst->basic_program_uniform_region,
                0.0f,
                0.0f,
                1.0f,
                1.0f
        ));
    }
    else
    {
        float wf = (float)texture->w;
        float hf = (float)texture->h;

        float rwf = data->region.w ? (float)data->region.w : wf;
        float rhf = data->region.h ? (float)data->region.h : hf;

        SRE_GL_CALL(inst->funcs2.glUniform4f(inst->basic_program_uniform_region,
                data->region.x / wf,
                data->region.y / hf,
                rwf / wf,
                rhf / hf
    ));
    }

    return true;
}

bool sreopengl_drawrect(const sre_videodriver* video, const sre_DDRect* data)
{
    const sre_videoOpenGL* inst = video->userdata;
    SRE_GL_CALL(inst->funcs2.glUniformMatrix4fv(inst->basic_program_uniform_rotation, 1, GL_FALSE, IDENTITY));
    return common_drawrect(inst, data);
}

bool sreopengl_drawrrect(const sre_videodriver* video, const sre_DDRRect* data)
{
    const sre_videoOpenGL* inst = video->userdata;

    return common_setrotation(inst, data->angle) && common_drawrect(inst, &data->rect);
}

bool sreopengl_drawline(const sre_videodriver* video, const sre_DDLine* data)
{
    sre_DDLines linesdata;
    linesdata.color = data->color;
    linesdata.flags = data->flags;
    linesdata.pts = &data->pt1;
    linesdata.count = 2;
    linesdata._unused = 0;
    return sreopengl_drawlines(video, &linesdata);
}

bool sreopengl_drawlines(const sre_videodriver* video, const sre_DDLines* data)
{
    const sre_videoOpenGL* inst = video->userdata;
    SRE_GL_CALL(inst->funcs2.glBindBuffer(GL_ARRAY_BUFFER, inst->line_vbo), return false;);
    if (inst->line_vao)
    {
        SRE_GL_CALL(inst->funcs3.glBindVertexArray(inst->line_vao), return false;);
    }
    else
    {
        if (!sreopengl_bindva2_1(inst))
            abort();
    }
    SRE_GL_CALL(inst->funcs2.glUseProgram(inst->line_program));

    SRE_GL_CALL(inst->funcs2.glUniform4f(inst->line_program_uniform_color, NORM_RGBA(data->color, f)));
    
    const GLfloat* cam;
    if (data->flags & SRE_DRAWFLAGS_USECAM)
        cam = inst->camera_view;
    else
        cam = IDENTITY;
    SRE_GL_CALL(inst->funcs2.glUniformMatrix4fv(inst->line_program_uniform_cameraview, 1, GL_FALSE, cam));
    
    SRE_GL_CALL(inst->funcs2.glBufferSubData(GL_ARRAY_BUFFER, 0, data->count * sizeof(sre_vec2ut), data->pts));
    SRE_GL_CALL(glDrawArrays(GL_LINES, 0, data->count));
    
    SRE_GL_CALL(inst->funcs2.glUseProgram(inst->basic_program));
    if (inst->basic_vao)
    {
        SRE_GL_CALL(inst->funcs3.glBindVertexArray(inst->basic_vao), return false;);
    }
    else
    {
        SRE_GL_CALL(inst->funcs2.glBindBuffer(GL_ARRAY_BUFFER, inst->basic_vbo), return false;);
        if (!sreopengl_bindva2_1(inst))
            abort();
    }
    return true;
}

bool sreopengl_drawtexture(const sre_videodriver* video, const sre_DDTexture* data)
{
    const sre_videoOpenGL* inst = video->userdata; 
    const sre_DDRect rect_data = {
        .flags = data->flags & SRE_DRAWFLAGS_USECAM,
        .color = data->modulate,
        .rect = data->rect,
        .anchor = data->anchor
    };

    bool res = common_drawtex(inst, data) && sreopengl_drawrect(video, &rect_data);
    SRE_GL_CALL(glBindTexture(GL_TEXTURE_2D, inst->basic_texture));
    return res;
}

bool sreopengl_drawrtexture(const sre_videodriver* video, const sre_DDRTexture* data)
{
    const sre_videoOpenGL* inst = video->userdata; 
    const sre_DDRRect rect_data = {
        {
            .flags = data->texture.flags & SRE_DRAWFLAGS_USECAM,
            .color = data->texture.modulate,
            .rect = data->texture.rect,
            .anchor = data->texture.anchor
        },
        data->angle        
    };

    bool res = common_drawtex(inst, &data->texture) && sreopengl_drawrrect(video, &rect_data);
    SRE_GL_CALL(glBindTexture(GL_TEXTURE_2D, inst->basic_texture));
    return res;    
}