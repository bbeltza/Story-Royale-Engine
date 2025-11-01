#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _argument
{
    const char* arg;
    const char* shortcut;
    const char* desc;
    //void (*handler)(const char*, const char*);
} arg_t;

#ifdef __cplusplus
}    
#endif