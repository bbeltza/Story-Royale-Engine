#include "gl32.h"

static const GLchar* D1_VS = "#version 150 core\n"
                             ""
                             "uniform Global {"
                                "mat4 viewport;"
                                "vec2 camera;"
                             "};"
                             ""
                             "struct Instance {"
                                "vec2 position;"
                                "vec2 size;"
                                "vec2 anchor;"
                                "uint _color_padding;"
                                "float angle;"

                                "vec2 uv;"
                                "vec2 uv_offset;"
                             "};"
                             "uniform UBInstance {"
                                "Instance instances[255];"
                             "};"
                             "uniform UBInstColor {"
                                "ivec4 colors[255];"
                             "};"
                             ""
                             "in vec2 v_pos;"
                             "out vec4 f_col;"
                             "out vec2 f_uv;"
                             ""
                             "void main() {"
                                "Instance inst = instances[gl_InstanceID];"
                                "float c = cos(inst.angle);"
                                "float s = sin(inst.angle);"
                                "mat4 transform1 = mat4("
                                    "c, s, 0, 0,"
                                    "-s, c, 0, 0,"
                                    "0, 0, 1, 0,"
                                    "inst.position, 0, 1"
                                ");"
                                "mat4 transform2 = mat4("
                                    "inst.size.x, 0, 0, 0,"
                                    "0, inst.size.y, 0, 0,"
                                    "0, 0, 1, 0,"
                                    "0, 0, 0, 1"
                                ");"
                                "gl_Position = transform1 * transform2 * vec4(v_pos-inst.anchor, 0.0, 1.0);"
                                "gl_Position = floor(gl_Position * viewport[2][2]);"
                                "gl_Position.xy += camera;"
                                "gl_Position.w = 1;"
                                "gl_Position = viewport * gl_Position;"

                                "f_col = vec4(colors[gl_InstanceID])/255;"
                                //"f_col = colors[gl_InstanceID];"
                                "f_uv = inst.uv_offset + inst.uv*v_pos;"
                             "}";

static const GLchar* D2_VS = "#version 150 core\n"
                             ""
                             "uniform Global {"
                                "mat4 viewport;"
                                "vec2 camera;"
                             "};"
                             ""
                             "uniform ivec4 u_col;"
                             "in vec2 v_pos;"
                             "in vec2 v_uv;"
                             ""
                             "out vec4 f_col;"
                             "out vec2 f_uv;"
                             "void main() {"
                                "gl_Position = vec4(v_pos*viewport[2][2] + camera, 0, 1);"
                                "gl_Position = floor(gl_Position);"
                                "gl_Position = viewport * gl_Position;"
                                "f_col = vec4(u_col)/255;"
                                "f_uv = v_uv;"
                             "}";

static const GLchar* C_FS = "#version 150 core\n"
                            ""
                            "uniform sampler2D u_tex;"
                            "in vec4 f_col;"
                            "in vec2 f_uv;"
                            "void main() {"
                                "gl_FragColor = texture(u_tex, f_uv) * f_col;"
                            "}";

bool sregl32setupbuffers(sregl32_inst* inst)
{
    {
        GLuint d1vs = SRE_GLCALLF(inst->glfuncs21.CreateShader(GL_VERTEX_SHADER));
        GLuint d2vs = SRE_GLCALLF(inst->glfuncs21.CreateShader(GL_VERTEX_SHADER));
        GLuint cfs = SRE_GLCALLF(inst->glfuncs21.CreateShader(GL_FRAGMENT_SHADER));
        
        SRE_GLCALLF(inst->glfuncs21.ShaderSource(d1vs, 1, &D1_VS, NULL));
        SRE_GLCALLF(inst->glfuncs21.ShaderSource(d2vs, 1, &D2_VS, NULL));
        SRE_GLCALLF(inst->glfuncs21.ShaderSource(cfs, 1, &C_FS, NULL));

        SRE_GLCALLF(inst->glfuncs21.CompileShader(d1vs));
        if (!SREGL21_CHECKSHADER(&inst->glfuncs21, d1vs, "d1vs")) return false;

        SRE_GLCALLF(inst->glfuncs21.CompileShader(d2vs));
        if (!SREGL21_CHECKSHADER(&inst->glfuncs21, d2vs, "d1vs")) return false;
        
        SRE_GLCALLF(inst->glfuncs21.CompileShader(cfs));
        if (!SREGL21_CHECKSHADER(&inst->glfuncs21, cfs, "cfs")) return false;

        GLuint d1program = SRE_GLCALLF(inst->glfuncs21.CreateProgram());
        SRE_GLCALLF(inst->glfuncs21.AttachShader(d1program, d1vs));
        SRE_GLCALLF(inst->glfuncs21.AttachShader(d1program, cfs));

        GLuint d2program = SRE_GLCALLF(inst->glfuncs21.CreateProgram());
        SRE_GLCALLF(inst->glfuncs21.AttachShader(d2program, d2vs));
        SRE_GLCALLF(inst->glfuncs21.AttachShader(d2program, cfs));

        SRE_GLCALLF(inst->glfuncs21.BindAttribLocation(d1program, 0, "v_pos"));

        SRE_GLCALLF(inst->glfuncs21.LinkProgram(d1program));
        if (!SREGL21_CHECKPROGRAM(&inst->glfuncs21, d1program, "d1program")) return false;
        
        SRE_GLCALLF(inst->glfuncs21.LinkProgram(d2program));
        if (!SREGL21_CHECKPROGRAM(&inst->glfuncs21, d2program, "d2program")) return false;

        GLuint d2stateubi = SRE_GLCALLF(inst->glfuncs32.GetUniformBlockIndex(d2program, "Global"));
        GLuint d1stateubi = SRE_GLCALLF(inst->glfuncs32.GetUniformBlockIndex(d1program, "Global"));
        GLuint d1instubi = SRE_GLCALLF(inst->glfuncs32.GetUniformBlockIndex(d1program, "UBInstance"));
        GLuint d1instcolubi = SRE_GLCALLF(inst->glfuncs32.GetUniformBlockIndex(d1program, "UBInstColor"));

        SRE_GLCALLF(inst->glfuncs32.UniformBlockBinding(d2program, d2stateubi, 0));
        SRE_GLCALLF(inst->glfuncs32.UniformBlockBinding(d1program, d1stateubi, 0));
        SRE_GLCALLF(inst->glfuncs32.UniformBlockBinding(d1program, d1instubi, 1));
        SRE_GLCALLF(inst->glfuncs32.UniformBlockBinding(d1program, d1instcolubi, 2));
        
        inst->d2data.coluniform = SRE_GLCALLF(inst->glfuncs21.GetUniformLocation(d2program, "u_col"));
        inst->d2data.program = d2program;
        inst->d1data.program = d1program;
    }

    {
        SRE_GLCALLF(inst->glfuncs.GetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &inst->UBO_ALIGNMENT));
        inst->UBO_STATEALIGN = (sizeof(struct sregl32_stateubo)+inst->UBO_ALIGNMENT-1)/inst->UBO_ALIGNMENT * inst->UBO_ALIGNMENT;

        GLuint ubos[3];
        SRE_GLCALLF(inst->glfuncs21.GenBuffers(3, ubos));
        SRE_GLCALLF(inst->glfuncs21.BindBuffer(GL_UNIFORM_BUFFER, ubos[0]));
        SRE_GLCALLF(inst->glfuncs21.BufferData(GL_UNIFORM_BUFFER, inst->UBO_STATEALIGN*2, NULL, GL_DYNAMIC_DRAW));
        SRE_GLCALLF(inst->glfuncs21.BindBuffer(GL_UNIFORM_BUFFER, ubos[1]));
        SRE_GLCALLF(inst->glfuncs21.BufferData(GL_UNIFORM_BUFFER, sizeof(struct sre_RenderInstance1)*255, NULL, GL_DYNAMIC_DRAW));
        SRE_GLCALLF(inst->glfuncs21.BindBuffer(GL_UNIFORM_BUFFER, ubos[2]));
        SRE_GLCALLF(inst->glfuncs21.BufferData(GL_UNIFORM_BUFFER, sizeof(int[4])*255, NULL, GL_DYNAMIC_DRAW));
        
        inst->stateubo = ubos[0];
        inst->d1data.instubo = ubos[1];
        inst->d1data.instcolubo = ubos[2];

        SRE_GLCALLF(inst->glfuncs32.BindBufferBase(GL_UNIFORM_BUFFER, 1, inst->d1data.instubo));
        SRE_GLCALLF(inst->glfuncs32.BindBufferBase(GL_UNIFORM_BUFFER, 2, inst->d1data.instcolubo));
    }
    
    {
        SRE_GLCALLF(inst->glfuncs32.GenVertexArrays(1, &inst->d1data.vao));
        SRE_GLCALLF(inst->glfuncs21.GenBuffers(1, &inst->d1data.vbo));

        static const GLfloat D1_VERTICES[] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f
        };

        SRE_GLCALLF(inst->glfuncs21.BindBuffer(GL_ARRAY_BUFFER, inst->d1data.vbo));
        SRE_GLCALLF(inst->glfuncs21.BufferData(GL_ARRAY_BUFFER, sizeof(D1_VERTICES), D1_VERTICES, GL_STATIC_DRAW));

        SRE_GLCALLF(inst->glfuncs32.BindVertexArray(inst->d1data.vao));
        SRE_GLCALLF(inst->glfuncs21.EnableVertexAttribArray(0));
        SRE_GLCALLF(inst->glfuncs21.VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0));
    }

    {
        SRE_GLCALLF(inst->glfuncs32.GenVertexArrays(1, &inst->d2data.vao));
        SRE_GLCALLF(inst->glfuncs21.GenBuffers(1, &inst->d2data.vbo));

        SRE_GLCALLF(inst->glfuncs21.BindBuffer(GL_ARRAY_BUFFER, inst->d2data.vbo));
        SRE_GLCALLF(inst->glfuncs21.BufferData(GL_ARRAY_BUFFER, sizeof(sre_RenderPoint)*255, NULL, GL_DYNAMIC_DRAW));
        inst->d2data.vbosize = 255;

        SRE_GLCALLF(inst->glfuncs32.BindVertexArray(inst->d2data.vao));
        SRE_GLCALLF(inst->glfuncs21.EnableVertexAttribArray(0));
        SRE_GLCALLF(inst->glfuncs21.EnableVertexAttribArray(1));
        SRE_GLCALL(inst->glfuncs21.VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(sre_RenderPoint), (const void*)offsetof(sre_RenderPoint, pos)));
        SRE_GLCALL(inst->glfuncs21.VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(sre_RenderPoint), (const void*)offsetof(sre_RenderPoint, uv)));
    }

    return true;
}