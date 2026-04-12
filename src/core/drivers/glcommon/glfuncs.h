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

#define SRE_GLFUNCS                                   \
        SRE_GLPFNDEF(PFNGLCLEARPROC, Clear)           \
        SRE_GLPFNDEF(PFNGLCLEARCOLORPROC, ClearColor) \
        SRE_GLPFNDEF(PFNGLVIEWPORTPROC, Viewport)     \
        SRE_GLPFNDEF(PFNGLGETERRORPROC, GetError)     \
        SRE_GLPFNDEF(PFNGLSCISSORPROC, Scissor)       \
        SRE_GLPFNDEF(PFNGLDISABLEPROC, Enable)        \
        SRE_GLPFNDEF(PFNGLENABLEPROC, Disable)        \
        SRE_GLPFNDEF(PFNGLBLENDFUNCPROC, BlendFunc)        

#define SRE_GLFUNCS21                                     \
        SRE_GLPFNDEF(PFNGLGENBUFFERSPROC, GenBuffers)     \
        SRE_GLPFNDEF(PFNGLBINDBUFFERPROC, BindBuffer)     \
        SRE_GLPFNDEF(PFNGLBUFFERDATAPROC, BufferData)     

#ifndef SRE_GLPFNDEF
    #define SRE_GLPFNDEF(t, n) t n;
#endif

#endif