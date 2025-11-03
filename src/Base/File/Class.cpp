#include <standard>
#include "Base/File.hpp"
#include "SDL.hpp"

#include "Sys.h"

File::Map* File::s_loaded = nullptr;

File::File() : m_type(T_UNKNOWN) {}

File::File(File &&moved)
    : m_type(moved.m_type), m_userdata(moved.m_userdata), m_filepath(moved.m_filepath)
{
    moved.m_filepath.clear();
}

File::~File()
{
    if (m_filepath.empty()) return;
    if (m_type < T_TXT) return;

    SDL_AtomicAdd(&m_info().ref, -1);
    int val = SDL_AtomicGet(&m_info().ref);
    
    if (val != 0) return;

    s_loaded->erase(m_filepath);
    if (s_loaded->empty())
    {
        delete s_loaded;
        s_loaded = nullptr;
    }
}