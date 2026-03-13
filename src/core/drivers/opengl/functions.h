#define SRE_GL_FUNC(type, name) type name
#define SRE_GL_FUNCTIONS_2_1                                            \
        SRE_GL_FUNC(PFNGLBINDBUFFERPROC, glBindBuffer);                 \
        SRE_GL_FUNC(PFNGLBUFFERDATAPROC, glBufferData);                 \
        \
        SRE_GL_FUNC(PFNGLBUFFERSUBDATAPROC, glBufferSubData);                 \
        \
        SRE_GL_FUNC(PFNGLGENBUFFERSPROC, glGenBuffers);                 \
                                                                        \
        SRE_GL_FUNC(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);      \
        SRE_GL_FUNC(PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray);    \
        SRE_GL_FUNC(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);              \
                                                                                       \
        SRE_GL_FUNC(PFNGLCREATESHADERPROC, glCreateShader);              \
        SRE_GL_FUNC(PFNGLCREATEPROGRAMPROC, glCreateProgram);            \
        SRE_GL_FUNC(PFNGLDELETESHADERPROC, glDeleteShader);              \
        SRE_GL_FUNC(PFNGLDELETEPROGRAMPROC, glDeleteProgram);            \
        SRE_GL_FUNC(PFNGLUSEPROGRAMPROC, glUseProgram);                  \
        SRE_GL_FUNC(PFNGLSHADERSOURCEPROC, glShaderSource);              \
        SRE_GL_FUNC(PFNGLCOMPILESHADERPROC, glCompileShader);            \
        SRE_GL_FUNC(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);      \
        SRE_GL_FUNC(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);      \
        SRE_GL_FUNC(PFNGLGETSHADERIVPROC, glGetShaderiv);                \
        SRE_GL_FUNC(PFNGLGETPROGRAMIVPROC, glGetProgramiv);                \
        SRE_GL_FUNC(PFNGLATTACHSHADERPROC, glAttachShader);              \
        SRE_GL_FUNC(PFNGLDETACHSHADERPROC, glDetachShader);              \
        SRE_GL_FUNC(PFNGLLINKPROGRAMPROC, glLinkProgram);                \
                                                                         \
        SRE_GL_FUNC(PFNGLBINDATTRIBLOCATIONPROC, glBindAttribLocation);  \
        SRE_GL_FUNC(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);  \
                                                                         \
        SRE_GL_FUNC(PFNGLUNIFORM1IPROC, glUniform1i);                    \
        SRE_GL_FUNC(PFNGLUNIFORM1IVPROC, glUniform1iv);                  \
        SRE_GL_FUNC(PFNGLUNIFORM2FPROC, glUniform2f);                    \
        SRE_GL_FUNC(PFNGLUNIFORM2FVPROC, glUniform2fv);                  \
        SRE_GL_FUNC(PFNGLUNIFORM4FPROC, glUniform4f);                    \
        SRE_GL_FUNC(PFNGLUNIFORM4FVPROC, glUniform4fv);                  \
        SRE_GL_FUNC(PFNGLUNIFORMMATRIX2FVPROC, glUniformMatrix2fv);      \
        SRE_GL_FUNC(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv);      \
                                                                         \
        SRE_GL_FUNC(PFNGLACTIVETEXTUREPROC, glActiveTexture);            \


#define SRE_GL_FUNCTIONS_3_2                                             \
        SRE_GL_FUNC(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);        \
        SRE_GL_FUNC(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays);  \
        SRE_GL_FUNC(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);        \
        SRE_GL_FUNC(PFNGLISVERTEXARRAYPROC, glIsVertexArray);            \

        