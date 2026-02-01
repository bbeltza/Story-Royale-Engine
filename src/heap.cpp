#include <stdlib.h>
#include <SDL.h>

#include <utils/logging.h>

#if !defined(NDEBUG)
SDL_atomic_t SR_NUM_ALLOCATIONS;

void* operator new(size_t size)
{
	size_t newsize = size + sizeof(size_t);
	size_t* block = reinterpret_cast<size_t*>(malloc(newsize));
	if (!block)
		std::terminate();

	block[0] = size;
	SDL_AtomicAdd(&SR_NUM_ALLOCATIONS, static_cast<int>(size));

	LOG("GOT OPERATOR NEW, NOW CURRENT SIZE: %zd, %zd", SDL_AtomicGet(&SR_NUM_ALLOCATIONS), size);

	#ifdef __unix__ // Uhmmm I think new/delete blocks are aligned to 2 pointers on unix-like systems
					// I'll try whatever number to align it correctly
		block += 2;
	#else
		block++;
	#endif
	return block;
}

void operator delete(void* block)
{
	if (!block) return;
	
	size_t* tblock = reinterpret_cast<size_t*>(block);
	#ifdef __unix__
		tblock += 2;
	#else
		tblock--;
	#endif

	SDL_AtomicAdd(&SR_NUM_ALLOCATIONS, -static_cast<int>(tblock[0]));

	LOG("GOT OPERATOR DELETE, NOW CURRENT SIZE: %zd %zd", SDL_AtomicGet(&SR_NUM_ALLOCATIONS), tblock[0]);

	free(tblock);
}

#endif