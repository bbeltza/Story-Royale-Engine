#include "gl32.h"

static const GLchar* D1_VS = "#version 150 core";

bool sregl32setupbuffers(sregl32_inst* inst)
{
    GLuint d1vs = SRE_GLCALLF(inst->glfuncs21.CreateShader(GL_VERTEX_SHADER));
    GLuint d2vs = SRE_GLCALLF(inst->glfuncs21.CreateShader(GL_VERTEX_SHADER));
    GLuint cfs = SRE_GLCALLF(inst->glfuncs21.CreateShader(GL_FRAGMENT_SHADER));
    
    SRE_GLCALLF(inst->glfuncs21.ShaderSource(d1vs, 1, &D1_VS, NULL));

    SRE_GLCALLF(inst->glfuncs21.CompileShader(d1vs));

    return true;
}