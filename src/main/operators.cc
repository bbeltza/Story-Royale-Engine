#include <utils/mem.h>
#include <cstddef>

extern "C" void* sre_new(size_t size) { return operator new(size); }
extern "C" void sre_delete(void* block) { return operator delete(block); }