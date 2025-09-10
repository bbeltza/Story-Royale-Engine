#include <standard.h>
#include "Classes/File.h"
#include "SDL.hpp"

File::File() : m_type(T_UNKNOWN)
{
}

File::File(File &&moved)
    : m_type(moved.m_type), m_userdata(moved.m_userdata), m_filepath(moved.m_filepath)
{
}

File::~File()
{
    if (m_type < T_TXT) return;
}

File::Map& File::get_loaded()
{
    static Map lf;
    return lf;
}