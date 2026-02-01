#include <utils/mem.h>

extern "C" void* sre_new(size_t size) { return operator new(size); }
extern "C" void sre_delete(void* block) { return operator delete(block); }