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
typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC)(GLenum mode, GLsizei count, GLenum type, const void* indices);
typedef void (APIENTRYP PFNGLGENTEXTURESPROC)(GLsizei n, GLuint* textures);
typedef void (APIENTRYP PFNGLDELETETEXTURESPROC)(GLsizei n, const GLuint* textures);
typedef void (APIENTRYP PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
typedef void (APIENTRYP PFNGLACTIVETEXTUREPROC)(GLenum texture);
typedef void (APIENTRYP PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void * data);

#define SRE_GLFUNCS                                   \
        SRE_GLPFNDEF(PFNGLCLEARPROC, Clear)           \
        SRE_GLPFNDEF(PFNGLCLEARCOLORPROC, ClearColor) \
        SRE_GLPFNDEF(PFNGLVIEWPORTPROC, Viewport)     \
        SRE_GLPFNDEF(PFNGLGETERRORPROC, GetError)     \
        SRE_GLPFNDEF(PFNGLSCISSORPROC, Scissor)       \
        SRE_GLPFNDEF(PFNGLDISABLEPROC, Enable)        \
        SRE_GLPFNDEF(PFNGLENABLEPROC, Disable)        \
        SRE_GLPFNDEF(PFNGLBLENDFUNCPROC, BlendFunc)   \
        SRE_GLPFNDEF(PFNGLDRAWELEMENTSPROC, DrawElements)         \
        SRE_GLPFNDEF(PFNGLGENTEXTURESPROC, GenTextures)           \
        SRE_GLPFNDEF(PFNGLDELETETEXTURESPROC, DeleteTextures)     \
        SRE_GLPFNDEF(PFNGLBINDTEXTUREPROC, BindTexture)           \
        SRE_GLPFNDEF(PFNGLACTIVETEXTUREPROC, ActiveTexture)       \
        SRE_GLPFNDEF(PFNGLTEXPARAMETERIPROC, TexParameteri)       \
        SRE_GLPFNDEF(PFNGLTEXIMAGE2DPROC, TexImage2D)             \

#define SRE_GLFUNCS21                                               \
        SRE_GLPFNDEF(PFNGLGENBUFFERSPROC, GenBuffers)               \
        SRE_GLPFNDEF(PFNGLBINDBUFFERPROC, BindBuffer)               \
        SRE_GLPFNDEF(PFNGLBUFFERDATAPROC, BufferData)               \
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

#ifndef SRE_GLPFNDEF
    #define SRE_GLPFNDEF(t, n) t n;
#endif

#endif