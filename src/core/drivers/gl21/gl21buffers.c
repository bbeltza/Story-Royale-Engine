#include "gl21.h"

#ifndef NDEBUG
    #define NL "\n"
#else
    #define NL
#endif

//static const GLchar SHADER_HEADER[] = "#version 120\n";

static const GLchar* COMMON_FS =
                        "#version 120\n"
                        ""
                        "uniform sampler2D u_sampler;"
                        "varying vec2 o_uv;"
                        "varying vec4 o_col;"
                        "void main() {"
                            "gl_FragColor = texture2D(u_sampler, o_uv) * o_col;"
                        "}";
static const GLchar* DRAW1_VS =                         
                        "#version 120\n"                
                        ""                              
                        "uniform mat3x4 u_model;"
                        "uniform ivec4 u_col4;"
                        "uniform mat4 u_viewport;"
                        "uniform vec2 u_camera;"
                        ""
                        "attribute vec4 i_pos;"
                        "varying vec2 o_uv;"
                        "varying vec4 o_col;"           
                        "void main() {"                 
                            "o_col = u_col4/255.0f;"
                            "o_uv = u_model[2].zw + u_model[2].xy*i_pos.xy;"

                            "float c = cos(u_model[1].z);"
                            "float s = sin(u_model[1].z);"
                            "mat4 rot = mat4("
                                "c, s, 0, 0,"
                                "-s, c, 0, 0,"
                                "0, 0, 1, 0,"
                                "u_model[0].xy, 0, 1"
                            ");"
                            "mat4 mat = mat4("
                                "u_model[0].z, 0, 0, 0,"
                                "0, u_model[0].w, 0, 0,"
                                "0, 0, 1, 0,"
                                "0, 0, 0, 1"
                            ");"
                            "gl_Position = rot * mat * vec4(i_pos.xy-u_model[1].xy, 0.0, 1.0);"
                            "gl_Position = floor(gl_Position * u_viewport[2][2]);"
                            "gl_Position.xy += u_camera;"
                            "gl_Position.w = 1;"
                            "gl_Position = u_viewport * gl_Position;"
                        "}";

static const GLchar* DRAW2_VS = 
                        "#version 120\n"
                        ""
                        "uniform ivec4 u_col4;"
                        "uniform mat4 u_viewport;"
                        "uniform vec2 u_camera;"
                        ""
                        "attribute vec4 i_pos;"
                        "attribute vec2 i_uv;"
                        "varying vec4 o_col;"
                        "varying vec2 o_uv;"
                        "void main() {"
                            "o_uv = i_uv;"
                            "o_col = u_col4/255.0f;"
                            "gl_Position = i_pos*u_viewport[2][2];"
                            "gl_Position.xy += u_camera;"
                            "gl_Position.w = 1;"
                            "gl_Position = u_viewport * gl_Position;"
                        "}";

static const GLfloat DRAW1_VERTICES[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f
};
static const GLubyte DRAW1_INDICES[] = {
    0, 1, 2,
    2, 3, 0
};

static bool SREGL_CHECKSHADER(struct sregl_functions21* funcs, GLuint shader, const char* tag)
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

static bool SREGL_CHECKPROGRAM(struct sregl_functions21* funcs, GLuint program, const char* tag)
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

static void sreglsetupcommonuniforms(sregl21_inst* inst, GLuint program, struct sregl21_csu* uniforms)
{
    uniforms->color = inst->glfuncs21.GetUniformLocation(program, "u_col4");
    uniforms->camera = inst->glfuncs21.GetUniformLocation(program, "u_camera");
    uniforms->viewport = inst->glfuncs21.GetUniformLocation(program, "u_viewport");
    uniforms->sampler = inst->glfuncs21.GetUniformLocation(program, "u_sampler");
}

bool sregl21setupbuffers(sregl21_inst* inst)
{
    {
        GLuint buffers[3];
        SRE_GLCALLF(inst->glfuncs21.GenBuffers(3, buffers));

        inst->draw1data.vbo = buffers[0];
        inst->draw1data.ibo = buffers[1];
        inst->draw2data.vbo = buffers[2];
    }

    // Creating shaders

    GLuint cofs = inst->glfuncs21.CreateShader(GL_FRAGMENT_SHADER);
    GLuint d1vs = inst->glfuncs21.CreateShader(GL_VERTEX_SHADER);
    GLuint d2vs = inst->glfuncs21.CreateShader(GL_VERTEX_SHADER);
    assert(cofs != 0);
    assert(d1vs != 0);
    assert(d2vs != 0);

    SRE_GLCALLF(inst->glfuncs21.ShaderSource(cofs, 1, &COMMON_FS, NULL));
    SRE_GLCALLF(inst->glfuncs21.ShaderSource(d1vs, 1, &DRAW1_VS, NULL));
    SRE_GLCALLF(inst->glfuncs21.ShaderSource(d2vs, 1, &DRAW2_VS, NULL));

    SRE_GLCALL(inst->glfuncs21.CompileShader(cofs));
    SRE_GLCALL(inst->glfuncs21.CompileShader(d1vs));
    SRE_GLCALL(inst->glfuncs21.CompileShader(d2vs));

    {
        bool succ = true;
        succ &= SREGL_CHECKSHADER(&inst->glfuncs21, cofs, "cofs");
        succ &= SREGL_CHECKSHADER(&inst->glfuncs21, d1vs, "d1vs");
        succ &= SREGL_CHECKSHADER(&inst->glfuncs21, d2vs, "d2vs");

        if (!succ)
            return false;
    }

    inst->draw1data.program = inst->glfuncs21.CreateProgram();
    inst->draw2data.program = inst->glfuncs21.CreateProgram();

    assert(inst->draw1data.program != 0);
    assert(inst->draw2data.program != 0);

    SRE_GLCALLF(inst->glfuncs21.AttachShader(inst->draw1data.program, cofs));
    SRE_GLCALLF(inst->glfuncs21.AttachShader(inst->draw1data.program, d1vs));
    SRE_GLCALLF(inst->glfuncs21.AttachShader(inst->draw2data.program, cofs));
    SRE_GLCALLF(inst->glfuncs21.AttachShader(inst->draw2data.program, d2vs));

    SRE_GLCALLF(inst->glfuncs21.BindAttribLocation(inst->draw1data.program, 0, "i_pos"));
    SRE_GLCALLF(inst->glfuncs21.BindAttribLocation(inst->draw2data.program, 0, "i_pos"));
    SRE_GLCALLF(inst->glfuncs21.BindAttribLocation(inst->draw2data.program, 1, "i_uv"));

    SRE_GLCALL(inst->glfuncs21.LinkProgram(inst->draw1data.program));
    SRE_GLCALL(inst->glfuncs21.LinkProgram(inst->draw2data.program));

    {
        bool succ = true;
        succ &= SREGL_CHECKPROGRAM(&inst->glfuncs21, inst->draw1data.program, "draw1");
        succ &= SREGL_CHECKPROGRAM(&inst->glfuncs21, inst->draw2data.program, "draw2");

        if (!succ)
            return false;
    }

    SRE_GLCALL(inst->glfuncs21.DetachShader(inst->draw1data.program, cofs));
    SRE_GLCALL(inst->glfuncs21.DetachShader(inst->draw1data.program, d1vs));
    SRE_GLCALL(inst->glfuncs21.DetachShader(inst->draw2data.program, cofs));
    SRE_GLCALL(inst->glfuncs21.DetachShader(inst->draw2data.program, d2vs));

    SRE_GLCALL(inst->glfuncs21.DeleteShader(cofs));
    SRE_GLCALL(inst->glfuncs21.DeleteShader(d1vs));
    SRE_GLCALL(inst->glfuncs21.DeleteShader(d2vs));
    
    //

    SRE_GLCALLF(inst->glfuncs21.BindBuffer(GL_ARRAY_BUFFER, inst->draw1data.vbo));
    SRE_GLCALLF(inst->glfuncs21.BufferData(GL_ARRAY_BUFFER, sizeof(DRAW1_VERTICES), DRAW1_VERTICES, GL_STATIC_DRAW));
    SRE_GLCALLF(inst->glfuncs21.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, inst->draw1data.ibo));
    SRE_GLCALLF(inst->glfuncs21.BufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(DRAW1_INDICES), DRAW1_INDICES, GL_STATIC_DRAW));
    
    SRE_GLCALLF(inst->glfuncs21.BindBuffer(GL_ARRAY_BUFFER, inst->draw2data.vbo));
    SRE_GLCALLF(inst->glfuncs21.BufferData(GL_ARRAY_BUFFER, 256*sizeof(sre_RenderPoint), NULL, GL_DYNAMIC_DRAW));
    inst->draw2data.bufsize = 256;

    sreglsetupcommonuniforms(inst, inst->draw1data.program, &inst->draw1data.common_uniforms);
    sreglsetupcommonuniforms(inst, inst->draw2data.program, &inst->draw2data.common_uniforms);

    inst->draw1data.depend_uniforms.model = inst->glfuncs21.GetUniformLocation(inst->draw1data.program, "u_model");

    return true;   
}

void sregl21bindbuffer1(sregl21_inst* inst, GLint vbo)
{
    SRE_GLCALL(inst->glfuncs21.BindBuffer(GL_ARRAY_BUFFER, vbo));
    SRE_GLCALL(inst->glfuncs21.EnableVertexAttribArray(0));
    SRE_GLCALL(inst->glfuncs21.VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0));
}

void sregl21bindbuffer2(sregl21_inst* inst, GLint vbo)
{
    SRE_GLCALL(inst->glfuncs21.BindBuffer(GL_ARRAY_BUFFER, vbo));
    SRE_GLCALL(inst->glfuncs21.EnableVertexAttribArray(0));
    SRE_GLCALL(inst->glfuncs21.EnableVertexAttribArray(1));
    SRE_GLCALL(inst->glfuncs21.VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(sre_RenderPoint), (const void*)offsetof(sre_RenderPoint, pos)));
    SRE_GLCALL(inst->glfuncs21.VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(sre_RenderPoint), (const void*)offsetof(sre_RenderPoint, uv)));
}