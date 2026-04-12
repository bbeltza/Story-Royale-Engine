#ifndef SRE_GLFUNCS_H
#define SRE_GLFUNCS_H

#include <SDL_opengl.h>

typedef (*PFNGLCLEARPROC)(GLbitfield mask);
typedef (*PFNGLCLEARCOLORPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef (*PFNGLVIEWPORTPROC)(GLint x, GLint y, GLint w, GLint h);
typedef (*PFNGLGETERRORPROC)(void);

#define SRE_GLFUNCS                                   \
        SRE_GLPFNDEF(PFNGLCLEARPROC, Clear)           \
        SRE_GLPFNDEF(PFNGLCLEARCOLORPROC, ClearColor) \
        SRE_GLPFNDEF(PFNGLVIEWPORTPROC, Viewport)    \
        SRE_GLPFNDEF(PFNGLGETERRORPROC, GetError)    

#ifndef SRE_GLPFNDEF
    #define SRE_GLPFNDEF(t, n) t n;
#endif

#endif