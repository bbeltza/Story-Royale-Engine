#pragma once
#include <stdint.h>

#define NUM_ARGS 8

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _syserr
{
	UNKNOWN_ERROR,
    FILE_NOT_FOUND,
    SDL_ERROR,
    WORLD_CREATION_ERROR
} syserr;

typedef void* (*sysfunc_va)(void*, ...);
typedef struct
{
	sysfunc_va function;
	void* args[NUM_ARGS];
} sysfunc_base;


int syscode();
int sysexiting();
void sysexit(int code);
void syserror(syserr code, ...);
void sysdefer(sysfunc_va function, ...);
void syspawn(sysfunc_va function, ...);

void sysleeps(float s);
void sysleepms(uint32_t s);

#include <syslog.h>


#ifdef __cplusplus
}

// Template C++ wrappers can be implemented here!

//
#endif
