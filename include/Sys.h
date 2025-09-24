#pragma once

#define NUM_ARGS 8

#ifdef __cplusplus
extern "C" {
#endif

typedef void* (*func_va)(...);
typedef struct
{
	func_va function;
	void* args[NUM_ARGS];
} func_base;

void error(int code, ...);
void defer(func_va function, ...);


#ifdef __cplusplus
}

// Template wrappers can be implemented here so watch out!

//
#endif
