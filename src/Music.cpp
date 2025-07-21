#include <SDL.hpp>
#include "Music.h"
#include "System.h"

#define get_music Mix_Music* music_dat = (Mix_Music*)this->m_file.GetUserData()

Music::Music() {static int i = 0; printf("Constructed music instance! %d\n", ++i);}

void Music::Load(const char* filepath, Info* info)
{
    if (info) m_info = info;
    m_file.Load(filepath);
    if (m_file.getType() != File::Type::Sound)
    return;
    
    {
        SDL_RWops* temp_rw = SDL_RWFromConstMem(m_file.getRawData(), m_file.getSize());
        LoadUserData(temp_rw);
    }
    if (m_file.GetUserData())
    {
        m_loaded = true;
    }
    else
        System::Error(System::SDL_ERROR);
}

void Music::LoadUserData(void* temp_rw)
{
    m_file.SetUserData(Mix_LoadMUS_RW((SDL_RWops*)temp_rw, 1));
}

Music::~Music()
{
    get_music;
    Mix_FreeMusic(music_dat);
}

void Music::Stop()
{
    get_music;
    Mix_HaltMusic();
}

void Music::FadeOut()
{
    get_music;
    Mix_FadeOutMusic(m_info ? m_info->fade_out*1000 : 1000);
}

void Music::Play()
{
    get_music;
    int res;
    if (m_info && m_info->fade_in)
        res = Mix_FadeInMusic(music_dat, -m_info->looped, (int)(m_info->fade_in*1000));
    else
        res = Mix_PlayMusic(music_dat, m_info ? -m_info->looped : 0);
    printf("PLAYING MUSIC %d\n", res);
    if (res < 0)
        System::Error(System::SDL_ERROR);
}
