#include "opengl.h"
#include "functions.h"

SRE_VEC2MAKE(float, f);
const sre_vec2f BASIC_RECT_DATA[] = {
    { -0.5f, -0.5f },
    {  0.5f, -0.5f },
    {  0.5f, 0.5f },
    {  -0.5f, 0.5f }
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
        "uniform vec2 u_anchor;"
        ""
        "uniform mat4 u_projection;"
        "uniform mat4 u_camera;"
        ""
        "varying vec2 f_uv;"
        "void main() {"
            "f_uv = i_pos.xy + 0.5f;"
            "gl_Position = u_projection * u_camera * u_model * vec4(i_pos - (u_anchor-0.5f), 0.0f, 1.0f);"
        "}"
;

const char BASICFILL_VS[] = 
        "#version 120\n"
        ""
        "attribute vec4 i_pos;"
        ""
        "varying vec2 f_uv;"
        "void main() {"
            "f_uv = i_pos.xy + 0.5f;"
            "gl_Position = sign(i_pos);"
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
            "gl_FragColor = u_col;"
        "}"
;

static void SRE_GL_SHADERLOG(sre_videoOpenGL* inst, GLuint shader)
{
    GLchar buffer[255];
    SRE_GL_CALL(inst->funcs2.glGetShaderInfoLog(shader, 255, NULL, buffer), abort(););
    sre_log(SRE_LOGCATEGORY_ERROR, "[OPENGL]: Failed compiling shader:\n%s", buffer);
}

bool sreopengl_bindva2_1(sre_videoOpenGL* inst)
{
    SRE_GL_CALL(inst->funcs2.glEnableVertexAttribArray(0), return false;);
    SRE_GL_CALL(inst->funcs2.glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, sizeof(sre_vec2f), (void*)0), return false;);
    
    return true;
}

bool sreopengl_setupbuffers(sre_videoOpenGL* inst)
{
    SRE_GL_CALL(inst->funcs2.glGenBuffers(2, &inst->basic_vbo), return false;);
    SRE_GL_CALL(inst->funcs2.glBindBuffer(GL_ARRAY_BUFFER, inst->basic_vbo), return false;);
    SRE_GL_CALL(inst->funcs2.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, inst->basic_ibo), return false;);

    SRE_GL_CALL(inst->funcs2.glBufferData(GL_ARRAY_BUFFER, sizeof(BASIC_RECT_DATA), BASIC_RECT_DATA, GL_STATIC_DRAW), return false;);

    if (!sreopengl_bindva2_1(inst))
        return false;
    
    SRE_GL_CALL(inst->funcs2.glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(BASIC_RECT_INDICES), BASIC_RECT_INDICES, GL_STATIC_DRAW), return false;);

    #define SRE_GL_STATUSCHECK(shader) inst->funcs2.glGetShaderiv(shader, GL_COMPILE_STATUS, &status); if (status == GL_FALSE){ SRE_GL_SHADERLOG(inst, vs); return false; } (void)0

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

    SRE_GL_CALL(inst->funcs2.glBindAttribLocation(program, 0, "i_pos"));
    SRE_GL_CALL(inst->funcs2.glBindAttribLocation(program2, 0, "i_pos"));
    SRE_GL_CALL(inst->funcs2.glUseProgram(program));

    SRE_GL_CALL(inst->funcs2.glDeleteShader(vs));
    SRE_GL_CALL(inst->funcs2.glDeleteShader(fs));
    SRE_GL_CALL(inst->funcs2.glDeleteShader(vsf));

    inst->basic_program_uniform_color = SRE_GL_CALL(inst->funcs2.glGetUniformLocation(program, "u_col"));
    inst->basic_program_uniform_model = SRE_GL_CALL(inst->funcs2.glGetUniformLocation(program, "u_model"));
    inst->basic_program_uniform_anchor = SRE_GL_CALL(inst->funcs2.glGetUniformLocation(program, "u_anchor"));
    inst->basic_program_uniform_texture = SRE_GL_CALL(inst->funcs2.glGetUniformLocation(program, "u_texture"));
    
    inst->basic_program_state_projection = SRE_GL_CALL(inst->funcs2.glGetUniformLocation(program, "u_projection"));
    inst->basic_program_state_cameraview = SRE_GL_CALL(inst->funcs2.glGetUniformLocation(program, "u_camera"));

    inst->basicfill_program_uniform_color = SRE_GL_CALL(inst->funcs2.glGetUniformLocation(program2, "u_col"));

    SRE_GL_CALL(inst->funcs2.glActiveTexture(GL_TEXTURE0));
    SRE_GL_CALL(inst->funcs2.glUniform1i(inst->basic_program_uniform_texture, 0));

    inst->basic_program = program;
    inst->basicfill_program = program2;

    return true;
}