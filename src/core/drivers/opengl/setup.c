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
        "void main() {"
            "gl_Position = vec4(i_pos);"
        "}"
;

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

    return true;
}