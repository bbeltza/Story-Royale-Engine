#define SRE 1

#include <cstdlib>
#include <memory>
#include <SDL.h>

#if 01
	#define SRE_DISABLE_LOGS
#endif
#include <Base/Log.h>

#if !defined(NDEBUG)
SDL_atomic_t SR_ALLOCATED_SIZE;
SDL_atomic_t SR_ALLOCATED_BLOCKS;

void* operator new(size_t size)
{
	size_t newsize = size + sizeof(size_t)
	#ifdef __unix__
	* 2
	#endif
	;
	size_t* block = reinterpret_cast<size_t*>(malloc(newsize));
	if (!block)
		throw std::bad_alloc();

	block[0] = size;
	SDL_AtomicAdd(&SR_ALLOCATED_SIZE, static_cast<int>(size));
	SDL_AtomicAdd(&SR_ALLOCATED_BLOCKS, 1);

	sre::log("operator new(%zd): %d, %d", size, SDL_AtomicGet(&SR_ALLOCATED_SIZE), SDL_AtomicGet(&SR_ALLOCATED_BLOCKS));

	#ifdef __unix__ // Uhmmm I think new/delete blocks are aligned to 2 pointers on unix-like systems
		block += 2;
	#else
		block++;
	#endif
	return block;
}

void operator delete(void* block) noexcept
{
	if (!block) return;
	
	size_t* tblock = reinterpret_cast<size_t*>(block);
	#ifdef __unix__
		tblock -= 2;
	#else
		tblock--;
	#endif

	SDL_AtomicAdd(&SR_ALLOCATED_SIZE, -static_cast<int>(tblock[0]));
	SDL_AtomicAdd(&SR_ALLOCATED_BLOCKS, -1);

	sre::log("operator delete(%zd): %d, %d", *tblock, SDL_AtomicGet(&SR_ALLOCATED_SIZE), SDL_AtomicGet(&SR_ALLOCATED_BLOCKS));

	free(tblock);
}

#endif