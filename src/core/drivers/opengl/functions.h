#define SRE_GL_FUNC(type, name) type name
#define SRE_GL_FUNCTIONS_2_1                                            \
        SRE_GL_FUNC(PFNGLBINDBUFFERPROC, glBindBuffer);                 \
        SRE_GL_FUNC(PFNGLBUFFERDATAPROC, glBufferData);                 \
                                                                        \
        SRE_GL_FUNC(PFNGLGENBUFFERSPROC, glGenBuffers);                 \
                                                                        \
        SRE_GL_FUNC(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);      \
        SRE_GL_FUNC(PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray);    \
        SRE_GL_FUNC(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);               \


#define SRE_GL_FUNCTIONS_3_2                                             \
        SRE_GL_FUNC(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);        \
        SRE_GL_FUNC(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays);  \
        SRE_GL_FUNC(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);        \
        SRE_GL_FUNC(PFNGLISVERTEXARRAYPROC, glIsVertexArray);             \
