#ifndef SRE_VECTOR_H
#define SRE_VECTOR_H

// C compatible vector 2 class template
#define SRE_VEC2MAKE(type, suffix) typedef struct sre_vec2##suffix { type x; type y; } sre_vec2##suffix
#define SRE_VEC2SCALAR(x) { x, x }

#define sre_vec2add(v1, v2) { (v1).x += (v2).x; (v1).y += (v2).y } (void)0
#define sre_vec2sub(v1, v2) { (v1).x -= (v2).x; (v1).y -= (v2).y } (void)0
#define sre_vec2mul(v1, v2) { (v1).x *= (v2).x; (v1).y *= (v2).y } (void)0
#define sre_vec2div(v1, v2) { (v1).x /= (v2).x; (v1).y /= (v2).y } (void)0

#endif