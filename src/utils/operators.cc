#include <utils/mem.h>

void* sre_new(size_t size) { return operator new(size); }
void sre_delete(void* block) { return operator delete(block); }