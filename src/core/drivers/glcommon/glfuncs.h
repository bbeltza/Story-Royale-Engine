#ifndef SRE_GLFUNCS_H
#define SRE_GLFUNCS_H

#include <SDL_opengl.h>

typedef void (APIENTRYP PFNGLCLEARPROC)(GLbitfield mask);
typedef void (APIENTRYP PFNGLCLEARCOLORPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (APIENTRYP PFNGLVIEWPORTPROC)(GLint x, GLint y, GLint w, GLint h);
typedef GLenum (APIENTRYP PFNGLGETERRORPROC)(void);
typedef void (APIENTRYP PFNGLSCISSORPROC)(GLint x, GLint y, GLsizei w, GLsizei h);
typedef void (APIENTRYP PFNGLENABLEPROC)(GLenum cap);
typedef void (APIENTRYP PFNGLDISABLEPROC)(GLenum cap);
typedef void (APIENTRYP PFNGLBLENDFUNCPROC)(GLenum sfactor, GLenum dfactor);
typedef void (APIENTRYP PFNGLDRAWARRAYSPROC)(GLenum mode, GLint first, GLsizei count);
typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC)(GLenum mode, GLsizei count, GLenum type, const void* indices);
typedef void (APIENTRYP PFNGLGENTEXTURESPROC)(GLsizei n, GLuint* textures);
typedef void (APIENTRYP PFNGLDELETETEXTURESPROC)(GLsizei n, const GLuint* textures);
typedef void (APIENTRYP PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
typedef void (APIENTRYP PFNGLACTIVETEXTUREPROC)(GLenum texture);
typedef void (APIENTRYP PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLGETTEXLEVELPARAMETERIVPROC)(GLenum target, GLint level, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void * data);
typedef void (APIENTRYP PFNGLTEXSUBIMAGE2DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void * pixels);
typedef void (APIENTRYP PFNGLPIXELSTOREIPROC)(GLenum pname, GLint param);

typedef void (APIENTRYP PFNGLVERTEX2FPROC)(GLfloat x, GLfloat y);
typedef void (APIENTRYP PFNGLVERTEX2FVPROC)(const GLfloat *v);
typedef void (APIENTRYP PFNGLTEXCOORD2FPROC)(GLfloat s, GLfloat t);
typedef void (APIENTRYP PFNGLCOLORUBVPROC)(const GLubyte *v);
typedef void (APIENTRYP PFNGLBEGINPROC)(GLenum mode);
typedef void (APIENTRYP PFNGLENDPROC)(void);
typedef void (APIENTRYP PFNGLMATRIXMODEPROC)(GLenum mode);
typedef void (APIENTRYP PFNGLLOADIDENTITYPROC)(void);
typedef void (APIENTRYP PFNGLORTHOPROC)(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble nearVal, GLdouble farVal);
typedef void (APIENTRYP PFNGLSCALEFPROC)(GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRYP PFNGLTRANSLATEFPROC)(GLfloat x, GLfloat y, GLfloat z);

typedef void (APIENTRYP PFNGLPUSHMATRIXPROC)(void);
typedef void (APIENTRYP PFNGLPOPMATRIXPROC)(void);
typedef void (APIENTRYP PFNGLROTATEFPROC)(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);

#define SRE_GLFUNCS                                   \
        SRE_GLPFNDEF(PFNGLCLEARPROC, Clear)           \
        SRE_GLPFNDEF(PFNGLCLEARCOLORPROC, ClearColor) \
        SRE_GLPFNDEF(PFNGLVIEWPORTPROC, Viewport)     \
        SRE_GLPFNDEF(PFNGLGETERRORPROC, GetError)     \
        SRE_GLPFNDEF(PFNGLSCISSORPROC, Scissor)       \
        SRE_GLPFNDEF(PFNGLDISABLEPROC, Enable)        \
        SRE_GLPFNDEF(PFNGLENABLEPROC, Disable)        \
        SRE_GLPFNDEF(PFNGLBLENDFUNCPROC, BlendFunc)   \
        SRE_GLPFNDEF(PFNGLDRAWARRAYSPROC, DrawArrays)                \
        SRE_GLPFNDEF(PFNGLDRAWELEMENTSPROC, DrawElements)            \
        SRE_GLPFNDEF(PFNGLGENTEXTURESPROC, GenTextures)              \
        SRE_GLPFNDEF(PFNGLDELETETEXTURESPROC, DeleteTextures)        \
        SRE_GLPFNDEF(PFNGLBINDTEXTUREPROC, BindTexture)              \
        SRE_GLPFNDEF(PFNGLACTIVETEXTUREPROC, ActiveTexture)          \
        SRE_GLPFNDEF(PFNGLTEXPARAMETERIPROC, TexParameteri)          \
        SRE_GLPFNDEF(PFNGLGETTEXLEVELPARAMETERIVPROC, GetTexLevelParameteriv) \
        SRE_GLPFNDEF(PFNGLTEXIMAGE2DPROC, TexImage2D)                \
        SRE_GLPFNDEF(PFNGLTEXSUBIMAGE2DPROC, TexSubImage2D)          \
        SRE_GLPFNDEF(PFNGLPIXELSTOREIPROC, PixelStorei)          \

#define SRE_GLFUNCS21                                               \
        SRE_GLPFNDEF(PFNGLGENBUFFERSPROC, GenBuffers)               \
        SRE_GLPFNDEF(PFNGLBINDBUFFERPROC, BindBuffer)               \
        SRE_GLPFNDEF(PFNGLBUFFERDATAPROC, BufferData)               \
        SRE_GLPFNDEF(PFNGLBUFFERSUBDATAPROC, BufferSubData)         \
        SRE_GLPFNDEF(PFNGLCREATESHADERPROC, CreateShader)           \
        SRE_GLPFNDEF(PFNGLDELETESHADERPROC, DeleteShader)           \
        SRE_GLPFNDEF(PFNGLCREATEPROGRAMPROC, CreateProgram)         \
        SRE_GLPFNDEF(PFNGLDELETEPROGRAMPROC, DeleteProgram)         \
        SRE_GLPFNDEF(PFNGLSHADERSOURCEPROC, ShaderSource)           \
        SRE_GLPFNDEF(PFNGLCOMPILESHADERPROC, CompileShader)         \
        SRE_GLPFNDEF(PFNGLGETSHADERIVPROC, GetShaderiv)             \
        SRE_GLPFNDEF(PFNGLGETSHADERINFOLOGPROC, GetShaderInfoLog)   \
        SRE_GLPFNDEF(PFNGLATTACHSHADERPROC, AttachShader)           \
        SRE_GLPFNDEF(PFNGLDETACHSHADERPROC, DetachShader)           \
        SRE_GLPFNDEF(PFNGLLINKPROGRAMPROC, LinkProgram)             \
        SRE_GLPFNDEF(PFNGLVALIDATEPROGRAMPROC, ValidateProgram)     \
        SRE_GLPFNDEF(PFNGLGETPROGRAMIVPROC, GetProgramiv)           \
        SRE_GLPFNDEF(PFNGLGETPROGRAMINFOLOGPROC, GetProgramInfoLog) \
        SRE_GLPFNDEF(PFNGLUSEPROGRAMPROC, UseProgram)               \
        SRE_GLPFNDEF(PFNGLBINDATTRIBLOCATIONPROC, BindAttribLocation) \
        SRE_GLPFNDEF(PFNGLENABLEVERTEXATTRIBARRAYPROC, EnableVertexAttribArray)   \
        SRE_GLPFNDEF(PFNGLDISABLEVERTEXATTRIBARRAYPROC, DisableVertexAttribArray) \
        SRE_GLPFNDEF(PFNGLVERTEXATTRIBPOINTERPROC, VertexAttribPointer)           \
        SRE_GLPFNDEF(PFNGLGETUNIFORMLOCATIONPROC, GetUniformLocation)             \
        SRE_GLPFNDEF(PFNGLUNIFORM2FPROC, Uniform2f)                      \
        SRE_GLPFNDEF(PFNGLUNIFORM4IPROC, Uniform4i)                      \
        SRE_GLPFNDEF(PFNGLUNIFORM2FVPROC, Uniform2fv)                      \
        SRE_GLPFNDEF(PFNGLUNIFORMMATRIX4FVPROC, UniformMatrix4fv)                 \
        SRE_GLPFNDEF(PFNGLUNIFORMMATRIX3X4FVPROC, UniformMatrix3x4fv)             \

#define SRE_GLFUNCS32   \
        SRE_GLPFNDEF(PFNGLGENVERTEXARRAYSPROC, GenVertexArrays)                 \
        SRE_GLPFNDEF(PFNGLBINDVERTEXARRAYPROC, BindVertexArray)                 \
        SRE_GLPFNDEF(PFNGLDRAWARRAYSINSTANCEDPROC, DrawArraysInstanced)         \
        SRE_GLPFNDEF(PFNGLGETUNIFORMBLOCKINDEXPROC, GetUniformBlockIndex)       \
        SRE_GLPFNDEF(PFNGLUNIFORMBLOCKBINDINGPROC, UniformBlockBinding)         \
        SRE_GLPFNDEF(PFNGLBINDBUFFERRANGEPROC, BindBufferRange)         \
        SRE_GLPFNDEF(PFNGLBINDBUFFERBASEPROC, BindBufferBase)           \

#define SRE_GLFUNCS11                                                           \
        SRE_GLPFNDEF(PFNGLVERTEX2FPROC, Vertex2f)                               \
        SRE_GLPFNDEF(PFNGLVERTEX2FVPROC, Vertex2fv)                             \
        SRE_GLPFNDEF(PFNGLTEXCOORD2FPROC, TexCoord2f)                           \
        SRE_GLPFNDEF(PFNGLCOLORUBVPROC, Color4ubv)                              \
        SRE_GLPFNDEF(PFNGLBEGINPROC, Begin)                                     \
        SRE_GLPFNDEF(PFNGLENDPROC, End)                                         \
        SRE_GLPFNDEF(PFNGLMATRIXMODEPROC, MatrixMode)                           \
        SRE_GLPFNDEF(PFNGLLOADIDENTITYPROC, LoadIdentity)                       \
        SRE_GLPFNDEF(PFNGLORTHOPROC, Ortho)                                     \
        SRE_GLPFNDEF(PFNGLSCALEFPROC, Scalef)                                   \
        SRE_GLPFNDEF(PFNGLTRANSLATEFPROC, Translatef)                           \
        \
        SRE_GLPFNDEF(PFNGLPUSHMATRIXPROC, PushMatrix)                           \
        SRE_GLPFNDEF(PFNGLPOPMATRIXPROC, PopMatrix)                             \
        SRE_GLPFNDEF(PFNGLROTATEFPROC, Rotatef)                                 \

#ifndef SRE_GLPFNDEF
    #define SRE_GLPFNDEF(t, n) t n;
#endif

#endif