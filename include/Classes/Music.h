#pragma once
#include "File.h"

class Music
{
    public:
    struct Info
    {
        int loop_start = 0, loop_end = -1;
        bool looped;

        float fade_in = 0, fade_out = 0;
        
    };
    public:
    Music();
    ~Music();

    void Load(const char* filepath, Info* info);

    virtual void Play();
    virtual void Stop();
    virtual void FadeOut();

    protected:
    Info* m_info = 0;
    File m_file;
    bool m_loaded = false;
    bool m_sound_freed = false;

    virtual void LoadUserData(void* temp_rw);
};