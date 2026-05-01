#include "gl32.h"

static const GLchar* D1_VS = "#version 150 core\n"
                             ""
                             "in vec4 i_transform;"
                             "in vec2 i_anchor;"
                             "in float i_angle;"
                             "in vec4 i_color;"
                             "in vec4 i_texcoord;"
                             ""
                             "out GS {"
                                "vec2 transformed[4];"
                                "vec4 color;"
                                "vec4 texcoord;"
                             "} vs_out;"
                             ""
                             "void main() {"
                                "float ca = cos(i_angle);"
                                "float sa = sin(i_angle);"
                                "mat2 scale = mat2("
                                    "i_transform.z, 0.0f,"
                                    "0.0f, i_transform.w"
                                ");"
                                "mat2 rotation = mat2("
                                    "ca, sa,"
                                    "-sa, ca"
                                ");"
                                "mat2 model = rotation * scale;"
                                "vs_out.transformed[0] = model * (-i_anchor + vec2(0.0f, 0.0f)) + i_transform.xy;"
                                "vs_out.transformed[1] = model * (-i_anchor + vec2(1.0f, 0.0f)) + i_transform.xy;"
                                "vs_out.transformed[2] = model * (-i_anchor + vec2(0.0f, 1.0f)) + i_transform.xy;"
                                "vs_out.transformed[3] = model * (-i_anchor + vec2(1.0f, 1.0f)) + i_transform.xy;"
                                ""
                                "vs_out.texcoord = i_texcoord;"
                                "vs_out.color = i_color;"
                             "}";

static const GLchar* D1_GS = "#version 150 core\n"
                             ""
                             "layout (points) in;"
                             "layout (triangle_strip, max_vertices=4) out;"
                             ""
                             "uniform Global {"
                                "mat4 viewport;"
                                "vec2 camera;"
                             "};"
                             ""
                             "in GS {"
                                "vec2 transformed[4];"
                                "vec4 color;"
                                "vec4 texcoord;"
                             "} gs_in[];"
                             ""
                             "out vec4 f_col;"
                             "out vec2 f_uv;"
                             ""
                             "vec2 transformed(vec2 pos) {"
                                "vec4 transf = vec4(floor(pos*viewport[2][2] + camera), 0.0f, 1.0f);"
                                "return (viewport*transf).xy;"
                             "}"
                             ""
                             "void main() {"
                                "f_col = gs_in[0].color;"
                                ""
                                "gl_Position = vec4(0.0f, 0.0f, 0.0f, 1.0f);"
                                ""
                                "gl_Position.xy = transformed(gs_in[0].transformed[0]);"
                                "f_uv = gs_in[0].texcoord.zw;"
                                "EmitVertex();"
                                ""
                                "gl_Position.xy = transformed(gs_in[0].transformed[1]);"
                                "f_uv.x += gs_in[0].texcoord.x;"
                                "EmitVertex();"
                                ""
                                "gl_Position.xy = transformed(gs_in[0].transformed[2]);"
                                "f_uv = gs_in[0].texcoord.zw;"
                                "f_uv.y += gs_in[0].texcoord.y;"
                                "EmitVertex();"
                                ""
                                "gl_Position.xy = transformed(gs_in[0].transformed[3]);"
                                "f_uv.x += gs_in[0].texcoord.x;"
                                "EmitVertex();"

                                "EndPrimitive();"
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
        GLuint d1gs = SRE_GLCALLF(inst->glfuncs21.CreateShader(GL_GEOMETRY_SHADER));
        GLuint d1vs = SRE_GLCALLF(inst->glfuncs21.CreateShader(GL_VERTEX_SHADER));
        GLuint d2vs = SRE_GLCALLF(inst->glfuncs21.CreateShader(GL_VERTEX_SHADER));
        GLuint cfs = SRE_GLCALLF(inst->glfuncs21.CreateShader(GL_FRAGMENT_SHADER));
        
        SRE_GLCALLF(inst->glfuncs21.ShaderSource(d1gs, 1, &D1_GS, NULL));
        SRE_GLCALLF(inst->glfuncs21.ShaderSource(d1vs, 1, &D1_VS, NULL));
        SRE_GLCALLF(inst->glfuncs21.ShaderSource(d2vs, 1, &D2_VS, NULL));
        SRE_GLCALLF(inst->glfuncs21.ShaderSource(cfs, 1, &C_FS, NULL));

        SRE_GLCALLF(inst->glfuncs21.CompileShader(d1gs));
        if (!SREGL21_CHECKSHADER(&inst->glfuncs21, d1gs, "d1gs")) return false;

        SRE_GLCALLF(inst->glfuncs21.CompileShader(d1vs));
        if (!SREGL21_CHECKSHADER(&inst->glfuncs21, d1vs, "d1vs")) return false;

        SRE_GLCALLF(inst->glfuncs21.CompileShader(d2vs));
        if (!SREGL21_CHECKSHADER(&inst->glfuncs21, d2vs, "d1vs")) return false;
        
        SRE_GLCALLF(inst->glfuncs21.CompileShader(cfs));
        if (!SREGL21_CHECKSHADER(&inst->glfuncs21, cfs, "cfs")) return false;

        GLuint d1program = SRE_GLCALLF(inst->glfuncs21.CreateProgram());
        SRE_GLCALLF(inst->glfuncs21.AttachShader(d1program, d1gs));
        SRE_GLCALLF(inst->glfuncs21.AttachShader(d1program, d1vs));
        SRE_GLCALLF(inst->glfuncs21.AttachShader(d1program, cfs));

        GLuint d2program = SRE_GLCALLF(inst->glfuncs21.CreateProgram());
        SRE_GLCALLF(inst->glfuncs21.AttachShader(d2program, d2vs));
        SRE_GLCALLF(inst->glfuncs21.AttachShader(d2program, cfs));

        SRE_GLCALLF(inst->glfuncs21.BindAttribLocation(d1program, 0, "i_transform"));
        SRE_GLCALLF(inst->glfuncs21.BindAttribLocation(d1program, 1, "i_anchor"));
        SRE_GLCALLF(inst->glfuncs21.BindAttribLocation(d1program, 2, "i_angle"));
        SRE_GLCALLF(inst->glfuncs21.BindAttribLocation(d1program, 3, "i_color"));
        SRE_GLCALLF(inst->glfuncs21.BindAttribLocation(d1program, 4, "i_texcoord"));

        SRE_GLCALLF(inst->glfuncs21.LinkProgram(d1program));
        if (!SREGL21_CHECKPROGRAM(&inst->glfuncs21, d1program, "d1program")) return false;
        
        SRE_GLCALLF(inst->glfuncs21.LinkProgram(d2program));
        if (!SREGL21_CHECKPROGRAM(&inst->glfuncs21, d2program, "d2program")) return false;

        GLuint d1stateubi = SRE_GLCALLF(inst->glfuncs32.GetUniformBlockIndex(d1program, "Global"));
        GLuint d2stateubi = SRE_GLCALLF(inst->glfuncs32.GetUniformBlockIndex(d2program, "Global"));

        assert(d1stateubi != GL_INVALID_INDEX);
        assert(d2stateubi != GL_INVALID_INDEX);

        SRE_GLCALLF(inst->glfuncs32.UniformBlockBinding(d2program, d2stateubi, 0));
        SRE_GLCALLF(inst->glfuncs32.UniformBlockBinding(d1program, d1stateubi, 0));

        inst->d2data.coluniform = SRE_GLCALLF(inst->glfuncs21.GetUniformLocation(d2program, "u_col"));
        inst->d2data.program = d2program;
        inst->d1data.program = d1program;
    }

    {
        SRE_GLCALLF(inst->glfuncs.GetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &inst->UBO_ALIGNMENT));
        inst->UBO_STATEALIGN = (sizeof(struct sregl32_stateubo)+inst->UBO_ALIGNMENT-1)/inst->UBO_ALIGNMENT * inst->UBO_ALIGNMENT;

        GLuint ubos[1];
        SRE_GLCALLF(inst->glfuncs21.GenBuffers(1, ubos));
        SRE_GLCALLF(inst->glfuncs21.BindBuffer(GL_UNIFORM_BUFFER, ubos[0]));
        SRE_GLCALLF(inst->glfuncs21.BufferData(GL_UNIFORM_BUFFER, inst->UBO_STATEALIGN*2, NULL, GL_DYNAMIC_DRAW));
        
        inst->stateubo = ubos[0];
    }
    
    {
        SRE_GLCALLF(inst->glfuncs32.GenVertexArrays(1, &inst->d1data.vao));
        SRE_GLCALLF(inst->glfuncs21.GenBuffers(1, &inst->d1data.vbo));
        
        SRE_GLCALLF(inst->glfuncs21.BindBuffer(GL_ARRAY_BUFFER, inst->d1data.vbo));
        SRE_GLCALLF(inst->glfuncs21.BufferData(GL_ARRAY_BUFFER, sizeof(sre_RenderInstance1)*255, NULL, GL_DYNAMIC_DRAW));
        inst->d1data.vbosize = 255;

        SRE_GLCALLF(inst->glfuncs32.BindVertexArray(inst->d1data.vao));
        SRE_GLCALLF(inst->glfuncs21.EnableVertexAttribArray(0));
        SRE_GLCALLF(inst->glfuncs21.EnableVertexAttribArray(1));
        SRE_GLCALLF(inst->glfuncs21.EnableVertexAttribArray(2));
        SRE_GLCALLF(inst->glfuncs21.EnableVertexAttribArray(3));
        SRE_GLCALLF(inst->glfuncs21.EnableVertexAttribArray(4));
        SRE_GLCALLF(inst->glfuncs21.VertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(sre_RenderInstance1), (const void*)offsetof(sre_RenderInstance1, rectangle)));
        SRE_GLCALLF(inst->glfuncs21.VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(sre_RenderInstance1), (const void*)offsetof(sre_RenderInstance1, anchor)));
        SRE_GLCALLF(inst->glfuncs21.VertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(sre_RenderInstance1), (const void*)offsetof(sre_RenderInstance1, angle)));
        SRE_GLCALLF(inst->glfuncs21.VertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(sre_RenderInstance1), (const void*)offsetof(sre_RenderInstance1, color)));
        SRE_GLCALLF(inst->glfuncs21.VertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(sre_RenderInstance1), (const void*)offsetof(sre_RenderInstance1, uv)));
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