#include <stdlib.h>
#include <SDL.h>

#include <utils/logging.h>

#if !defined(NDEBUG) && _WIN32 // This operator is broken on Linux so for safety only override for Windows
SDL_atomic_t SR_NUM_ALLOCATIONS;

void* operator new(size_t size)
{
	size_t newsize = size + sizeof(size_t);
	size_t* block = reinterpret_cast<size_t*>(malloc(newsize));
	if (!block)
		abort();

	block[0] = size;
	SDL_AtomicAdd(&SR_NUM_ALLOCATIONS, static_cast<int>(size));

	//LOG("GOT OPERATOR NEW, NOW CURRENT SIZE: %zd, %zd", SDL_AtomicGet(&SR_NUM_ALLOCATIONS), size);

	block++;
	return block;
}
void operator delete(void* block)
{
	if (!block) return;
	
	size_t* tblock = reinterpret_cast<size_t*>(block);
	tblock -= 1;

	SDL_AtomicAdd(&SR_NUM_ALLOCATIONS, -static_cast<int>(tblock[0]));

	//LOG("GOT OPERATOR DELETE, NOW CURRENT SIZE: %zd %zd", SDL_AtomicGet(&SR_NUM_ALLOCATIONS), tblock[0]);

	free(tblock);
}
#endif