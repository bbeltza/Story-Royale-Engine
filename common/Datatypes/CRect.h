#ifndef SRE_RECT_H
#define SRE_RECT_H
#include <Datatypes/CVector.h>

// C compatible rect 2D class template + union
#define SRE_RECT2DMAKE(type, suffix) SRE_VEC2MAKE(type, suffix); \
typedef union sre_rect2D##suffix { \
    struct {type x; type y; type w; type h;}; \
    struct {sre_vec2##suffix position; sre_vec2##suffix size;}; \
} sre_rect2D##suffix

#endif