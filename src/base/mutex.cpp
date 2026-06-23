#include <Base/Mutex.hpp>
#include <SDL_mutex.h>

using namespace sre;

Mutex::Mutex(): m_sdlhandle(SDL_CreateMutex()) {}

Mutex::~Mutex() {
    SDL_DestroyMutex(m_sdlhandle);
}

void Mutex::lock() {
    SDL_LockMutex(m_sdlhandle);
}

bool Mutex::unlock() {
    return SDL_UnlockMutex(m_sdlhandle) == 0;
}

bool Mutex::try_lock() {
    return SDL_TryLockMutex(m_sdlhandle) == 0;
}