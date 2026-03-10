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

bool sreopengl_drawrect(const sre_videodriver* video, const sre_DDRect* data)
{
    static const GLfloat IDENTITY[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    const sre_videoOpenGL* inst = video->userdata;
    const sre_unit mat[16] = {
        data->rect.size.x, 0, 0, 0,
        0, data->rect.size.y, 0, 0,
        0, 0, 1, 0,
        data->rect.position.x, data->rect.position.y, 0, 1
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

bool sreopengl_drawline(const sre_videodriver* video, const sre_DDLine* data)
{
    return false;
}

#include <assert.h>
bool sreopengl_drawtexture(const sre_videodriver* video, const sre_DDTexture* data)
{
    const sre_videoOpenGL* inst = video->userdata;
    const sre_GLtexture* texture = sre_get_texture(data->texture);
    if (!texture)
        return false;

    SRE_GL_CALL(glBindTexture(GL_TEXTURE_2D, texture->id));

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
    
    const sre_DDRect rect_data = {
        .flags = data->flags & SRE_DRAWFLAGS_USECAM,
        .color = data->modulate,
        .rect = data->rect,
        .anchor = data->anchor
    };

    bool res = sreopengl_drawrect(video, &rect_data);
    SRE_GL_CALL(glBindTexture(GL_TEXTURE_2D, inst->basic_texture));
    return res;    
}