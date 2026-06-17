#include "gl21.h"
#include <utils/mem.h>

#ifndef NDEBUG
    #define NL "\n"
#else
    #define NL
#endif

static const GLchar* COMMON_FS =
                        "#version 120\n"
                        ""
                        "uniform sampler2D u_sampler;"
                        "varying vec2 o_uv;"
                        "varying vec4 o_col;"
                        "void main() {"
                            "gl_FragColor = texture2D(u_sampler, o_uv) * o_col;"
                        "}";
static const GLchar* COMMON_VS = 
                        "#version 120\n"
                        ""
                        "uniform mat4 u_viewport;"
                        "uniform vec2 u_camera;"
                        ""
                        "attribute vec4 i_pos;"
                        "attribute vec4 i_col;"
                        "attribute vec2 i_uv;"
                        ""
                        "varying vec4 o_col;"
                        "varying vec2 o_uv;"
                        "void main() {"
                            "o_col = i_col;"
                            "o_uv = i_uv;"
                            ""
                            "gl_Position = i_pos*u_viewport[2][2];"
                            "gl_Position.xy += u_camera;"
                            "gl_Position.w = 1;"
                            "gl_Position = u_viewport * gl_Position;"
                        "}";

bool SREGL21_CHECKSHADER(struct sregl_functions21* funcs, GLuint shader, const char* tag)
{
    GLint status;
    funcs->GetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE)
        return true;

    GLint len;
    funcs->GetShaderiv(shader, GL_INFO_LOG_LENGTH, &len); assert(len != 0);

    GLchar* info = malloc(len);
    funcs->GetShaderInfoLog(shader, len, NULL, info);
    SRE_GLLOG("[OpenGL error]: Failed compiling GLSL shader (%s):\n%s", tag, info);

    free(info);
    return false;
}

bool SREGL21_CHECKPROGRAM(struct sregl_functions21* funcs, GLuint program, const char* tag)
{
    GLint status;
    funcs->GetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_TRUE)
        goto VALIDATE;

    GLint len;
    funcs->GetProgramiv(program, GL_INFO_LOG_LENGTH, &len); assert(len != 0);

    GLchar* info = malloc(len);
    funcs->GetProgramInfoLog(program, len, NULL, info);

    SRE_GLLOG("[OpenGL error]: Failed linking GLSL shader program (%s):\n%s", tag, info);
    free(info);
    return false;

    VALIDATE:
    funcs->ValidateProgram(program);
    funcs->GetProgramiv(program, GL_VALIDATE_STATUS, &status);
    if (status == GL_TRUE)
        return true;

    funcs->GetProgramiv(program, GL_INFO_LOG_LENGTH, &len); assert(len != 0);
    info = malloc(len);
    funcs->GetProgramInfoLog(program, len, NULL, info);

    SRE_GLLOG("[OpenGL error]: Failed validating GLSL shader program (%s):\n%s", tag, info);
    free(info);
    return false;
}

bool sregl21_vtassemblersetup(sregl21_inst* inst, struct sregl21_vtassembler* vtasm, size_t initial_size)
{
    assert(vtasm != NULL);

    vtasm->arr = NULL;
    vtasm->size = 0;
    sregl21_vtassemblerreserve(inst, vtasm, initial_size);
    return true;
}

//

void sregl21_vtassemblerreserve(sregl21_inst* inst, struct sregl21_vtassembler* vtasm, size_t size)
{
    if (size <= vtasm->size)
    {
        assert(size <= inst->mainvbosize);
        return;
    }

    size_t bytesize = size * sizeof(sre_RenderPoint);
    sre_delete(vtasm->arr);
    vtasm->arr = sre_new(bytesize);
    vtasm->size = size;

    if (size > inst->mainvbosize)
    {
        SRE_GLCALL(inst->glfuncs21.BufferData(GL_ARRAY_BUFFER, bytesize, NULL, GL_DYNAMIC_DRAW));
        inst->mainvbosize = bytesize;
    }
}

void sregl21_vtassemblerdata(sregl21_inst* inst, struct sregl21_vtassembler* vtasm, size_t size)
{
    assert(size <= vtasm->size);
    assert(size <= inst->mainvbosize);

    SRE_GLCALL(inst->glfuncs21.BufferSubData(GL_ARRAY_BUFFER, 0, size*sizeof(sre_RenderPoint), vtasm->arr));
}

void sregl21_vtassemblerfree(struct sregl21_vtassembler* vtasm)
{
    sre_delete(vtasm->arr);
    vtasm->arr = NULL;
    vtasm->size = 0;
}

//

bool sregl21setupbuffers(sregl21_inst* inst)
{
    // This is the main program's vertex buffer, and it'll remain bound up until the end of it.
    SRE_GLCALLF(inst->glfuncs21.GenBuffers(1, &inst->mainvbo));
    SRE_GLCALLF(inst->glfuncs21.BindBuffer(GL_ARRAY_BUFFER, inst->mainvbo));
    inst->mainvbosize = 0;

    SRE_GLCALLF(inst->glfuncs21.EnableVertexAttribArray(0));
    SRE_GLCALLF(inst->glfuncs21.EnableVertexAttribArray(1));
    SRE_GLCALLF(inst->glfuncs21.EnableVertexAttribArray(2));

    SRE_GLCALLF(inst->glfuncs21.VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(sre_RenderPoint), (const void*)offsetof(sre_RenderPoint, pos)));
    SRE_GLCALLF(inst->glfuncs21.VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(sre_RenderPoint), (const void*)offsetof(sre_RenderPoint, uv)));
    SRE_GLCALLF(inst->glfuncs21.VertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(sre_RenderPoint), (const void*)offsetof(sre_RenderPoint, color)));

    if (!sregl21_vtassemblersetup(inst, &inst->vtassembler, 4*255))
        return false;

    // Creating the shaders

    GLuint fs = inst->glfuncs21.CreateShader(GL_FRAGMENT_SHADER);
    GLuint vs = inst->glfuncs21.CreateShader(GL_VERTEX_SHADER);

    assert(fs != 0);
    assert(vs != 0);

    SRE_GLCALLF(inst->glfuncs21.ShaderSource(fs, 1, &COMMON_FS, NULL));
    SRE_GLCALLF(inst->glfuncs21.ShaderSource(vs, 1, &COMMON_VS, NULL));

    SRE_GLCALL(inst->glfuncs21.CompileShader(fs));
    SRE_GLCALL(inst->glfuncs21.CompileShader(vs));

    {
        bool succ = true;
        succ &= SREGL21_CHECKSHADER(&inst->glfuncs21, fs, "FS");
        succ &= SREGL21_CHECKSHADER(&inst->glfuncs21, vs, "VS");

        if (!succ)
            return false;
    }

    inst->mainprogram = inst->glfuncs21.CreateProgram();

    assert(inst->mainprogram != 0);

    SRE_GLCALLF(inst->glfuncs21.AttachShader(inst->mainprogram, fs));
    SRE_GLCALLF(inst->glfuncs21.AttachShader(inst->mainprogram, vs));

    SRE_GLCALLF(inst->glfuncs21.BindAttribLocation(inst->mainprogram, 0, "i_pos"));
    SRE_GLCALLF(inst->glfuncs21.BindAttribLocation(inst->mainprogram, 1, "i_uv"));
    SRE_GLCALLF(inst->glfuncs21.BindAttribLocation(inst->mainprogram, 2, "i_col"));

    SRE_GLCALL(inst->glfuncs21.LinkProgram(inst->mainprogram));

    {
        bool succ = true;
        succ &= SREGL21_CHECKPROGRAM(&inst->glfuncs21, inst->mainprogram, "mainprogram");

        if (!succ)
            return false;
    }

    SRE_GLCALL(inst->glfuncs21.DetachShader(inst->mainprogram, fs));
    SRE_GLCALL(inst->glfuncs21.DetachShader(inst->mainprogram, vs));

    SRE_GLCALL(inst->glfuncs21.DeleteShader(fs));
    SRE_GLCALL(inst->glfuncs21.DeleteShader(vs));

    SRE_GLCALLF(inst->glfuncs21.UseProgram(inst->mainprogram));
    
    //

    inst->uniforms.viewport = inst->glfuncs21.GetUniformLocation(inst->mainprogram, "u_viewport");
    inst->uniforms.camera = inst->glfuncs21.GetUniformLocation(inst->mainprogram, "u_camera");
    return true;   
}