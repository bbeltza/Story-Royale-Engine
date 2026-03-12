#include "opengl.h"
#include "functions.h"

SRE_VEC2MAKE(float, f);
const sre_vec2f BASIC_RECT_DATA[] = {
    { 0.0f, 0.0f },
    { 1.0f, 0.0f },
    { 1.0f, 1.0f },
    { 0.0f, 1.0f }
};

const uint8_t BASIC_RECT_INDICES[] = {
    0, 1, 2,
    0, 3, 2
};

const char BASIC_VS[] = 
        "#version 120\n"
        ""
        "attribute vec2 i_pos;"
        ""
        "uniform mat4 u_model;"
        "uniform mat4 u_rotation;"
        "uniform vec2 u_anchor;"
        ""
        "uniform mat4 u_projection;"
        "uniform mat4 u_camera;"
        ""
        "uniform vec4 u_regionuv;"
        "uniform vec2 u_flipvec;"
        "varying vec2 f_uv;"
        "void main() {"
            "vec2 uv_add = (u_flipvec/-2.0f + 0.5f);"
            "vec2 uv_begin = u_regionuv.xy;"
            "vec2 uv_end = u_regionuv.zw;"
            "f_uv = ((i_pos.xy*u_flipvec+uv_add)*uv_end + uv_begin);"
            ""
            "float scale = u_model[2][3];"
            "gl_Position = u_camera * u_model * u_rotation * vec4(i_pos - u_anchor, 0.0f, 1.0f);"
            "gl_Position *= vec4(vec2(scale), 1, 1);"
            "gl_Position = u_projection * floor(gl_Position);"
        "}"
;

const char BASICFILL_VS[] = 
        "#version 120\n"
        ""
        "attribute vec4 i_pos;"
        ""
        "varying vec2 f_uv;"
        "void main() {"
            "f_uv = vec2(0.0f);"
            "gl_Position = i_pos + i_pos - 1;"
        "}"
;

const char BASIC_FS[] = 
        "#version 120\n"
        ""
        "varying vec2 f_uv;"
        ""
        "uniform vec4 u_col;"
        "uniform sampler2D u_texture;"
        ""
        "void main() {"
            "gl_FragColor = u_col * texture2D(u_texture, f_uv);"
        "}"
;

static void SRE_GL_SHADERLOG(sre_videoOpenGL* inst, GLuint shader)
{
    GLchar buffer[255];
    SRE_GL_CALL(inst->funcs2.glGetShaderInfoLog(shader, 255, NULL, buffer), abort(););
    sre_log(SRE_LOGCATEGORY_ERROR, "[OPENGL]: Failed compiling shader:\n%s", buffer);
}

static void SRE_GL_PROGRAMLOG(sre_videoOpenGL* inst, GLuint program)
{
    GLchar buffer[255];
    SRE_GL_CALL(inst->funcs2.glGetProgramInfoLog(program, 255, NULL, buffer), abort(););
    sre_log(SRE_LOGCATEGORY_ERROR, "[OPENGL]: Failed linking program:\n%s", buffer);
}

bool sreopengl_bindva2_1(sre_videoOpenGL* inst)
{
    SRE_GL_CALL(inst->funcs2.glEnableVertexAttribArray(0), return false;);
    SRE_GL_CALL(inst->funcs2.glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, sizeof(sre_vec2f), (void*)0), return false;);
    
    return true;
}

bool sreopengl_setupbuffers(sre_videoOpenGL* inst)
{
    if (inst->funcs3.glBindVertexArray)
    {
        SRE_GL_CALL(inst->funcs3.glGenVertexArrays(1, &inst->basic_vao));
        SRE_GL_CALL(inst->funcs3.glBindVertexArray(inst->basic_vao));
    }

    SRE_GL_CALL(inst->funcs2.glGenBuffers(2, &inst->basic_vbo), return false;);
    SRE_GL_CALL(inst->funcs2.glBindBuffer(GL_ARRAY_BUFFER, inst->basic_vbo), return false;);
    SRE_GL_CALL(inst->funcs2.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, inst->basic_ibo), return false;);

    SRE_GL_CALL(inst->funcs2.glBufferData(GL_ARRAY_BUFFER, sizeof(BASIC_RECT_DATA), BASIC_RECT_DATA, GL_STATIC_DRAW), return false;);

    if (!sreopengl_bindva2_1(inst))
        return false;
    
    SRE_GL_CALL(inst->funcs2.glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(BASIC_RECT_INDICES), BASIC_RECT_INDICES, GL_STATIC_DRAW), return false;);

    #define SRE_GL_STATUSCHECK(shader) inst->funcs2.glGetShaderiv(shader, GL_COMPILE_STATUS, &status); if (status == GL_FALSE){ SRE_GL_SHADERLOG(inst, shader); return false; } (void)0
    #define SRE_GL_STATUSCHECKP(program) inst->funcs2.glGetProgramiv(program, GL_LINK_STATUS, &status); if (status == GL_FALSE){ SRE_GL_PROGRAMLOG(inst, program); return false; } (void)0

    const char* shader;
    shader = BASIC_VS;
    GLuint status;
    GLuint vs = SRE_GL_CALL(inst->funcs2.glCreateShader(GL_VERTEX_SHADER));
    SRE_GL_CALL(inst->funcs2.glShaderSource(vs, 1, &shader, NULL));
    SRE_GL_CALL(inst->funcs2.glCompileShader(vs), );
    SRE_GL_STATUSCHECK(vs);

    shader = BASICFILL_VS;
    GLuint vsf = SRE_GL_CALL(inst->funcs2.glCreateShader(GL_VERTEX_SHADER));
    SRE_GL_CALL(inst->funcs2.glShaderSource(vsf, 1, &shader, NULL));
    SRE_GL_CALL(inst->funcs2.glCompileShader(vsf), );
    SRE_GL_STATUSCHECK(vsf);
    
    shader = BASIC_FS;
    GLuint fs = SRE_GL_CALL(inst->funcs2.glCreateShader(GL_FRAGMENT_SHADER));
    SRE_GL_CALL(inst->funcs2.glShaderSource(fs, 1, &shader, NULL));
    SRE_GL_CALL(inst->funcs2.glCompileShader(fs), SRE_GL_SHADERLOG(inst, fs); return false;);
    SRE_GL_STATUSCHECK(fs);

    GLuint program = SRE_GL_CALL(inst->funcs2.glCreateProgram());
    SRE_GL_CALL(inst->funcs2.glAttachShader(program, vs), return false;);
    SRE_GL_CALL(inst->funcs2.glAttachShader(program, fs), return false;);
    SRE_GL_CALL(inst->funcs2.glLinkProgram(program), return false;);

    GLuint program2 = SRE_GL_CALL(inst->funcs2.glCreateProgram());
    SRE_GL_CALL(inst->funcs2.glAttachShader(program2, vsf), return false;);
    SRE_GL_CALL(inst->funcs2.glAttachShader(program2, fs), return false;);
    SRE_GL_CALL(inst->funcs2.glLinkProgram(program2), return false;);

    SRE_GL_STATUSCHECKP(program);
    SRE_GL_STATUSCHECKP(program2);

    SRE_GL_CALL(inst->funcs2.glBindAttribLocation(program, 0, "i_pos"));
    SRE_GL_CALL(inst->funcs2.glBindAttribLocation(program2, 0, "i_pos"));
    SRE_GL_CALL(inst->funcs2.glUseProgram(program));

    SRE_GL_CALL(inst->funcs2.glDeleteShader(vs));
    SRE_GL_CALL(inst->funcs2.glDeleteShader(fs));
    SRE_GL_CALL(inst->funcs2.glDeleteShader(vsf));

    inst->basic_program_uniform_color = SRE_GL_CALL(inst->funcs2.glGetUniformLocation(program, "u_col"));
    inst->basic_program_uniform_model = SRE_GL_CALL(inst->funcs2.glGetUniformLocation(program, "u_model"));
    inst->basic_program_uniform_rotation = SRE_GL_CALL(inst->funcs2.glGetUniformLocation(program, "u_rotation"));
    inst->basic_program_uniform_anchor = SRE_GL_CALL(inst->funcs2.glGetUniformLocation(program, "u_anchor"));
    inst->basic_program_uniform_texture = SRE_GL_CALL(inst->funcs2.glGetUniformLocation(program, "u_texture"));
    inst->basic_program_uniform_region = SRE_GL_CALL(inst->funcs2.glGetUniformLocation(program, "u_regionuv"));
    inst->basic_program_uniform_flip = SRE_GL_CALL(inst->funcs2.glGetUniformLocation(program, "u_flipvec"));
    
    inst->basic_program_state_projection = SRE_GL_CALL(inst->funcs2.glGetUniformLocation(program, "u_projection"));
    inst->basic_program_state_cameraview = SRE_GL_CALL(inst->funcs2.glGetUniformLocation(program, "u_camera"));

    inst->basicfill_program_uniform_color = SRE_GL_CALL(inst->funcs2.glGetUniformLocation(program2, "u_col"));

    SRE_GL_CALL(inst->funcs2.glActiveTexture(GL_TEXTURE0));
    SRE_GL_CALL(inst->funcs2.glUniform1i(inst->basic_program_uniform_texture, 0));

    const unsigned WHITE = -1;
    SRE_GL_CALL(glGenTextures(1, &inst->basic_texture));
    SRE_GL_CALL(glBindTexture(GL_TEXTURE_2D, inst->basic_texture));
    SRE_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA2, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE_3_3_2, &WHITE));

    inst->basic_program = program;
    inst->basicfill_program = program2;

    return true;
}