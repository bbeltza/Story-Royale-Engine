#include <SDL.hpp>
#include "Sound.h"

#define get_chunk Mix_Chunk* chunk_dat = (Mix_Chunk*)m_file.GetUserData()

Sound::Sound() {}

Sound::~Sound()
{
    get_chunk;
    Mix_FreeChunk(chunk_dat);
}

void Sound::LoadUserData(void* temp_rw)
{
    m_file.SetUserData(Mix_LoadWAV_RW((SDL_RWops*)temp_rw, 1));
}

void Sound::Play()
{
    get_chunk;
    channel_playing = Mix_PlayChannel(channel_playing, chunk_dat, 0);
}

void Sound::Stop()
{
    if (channel_playing > -1)
        ;
}