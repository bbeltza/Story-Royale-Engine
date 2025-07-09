#pragma once
#include "Enums.h"

class File
{
public:
    File();
    ~File();

    void Load(const char* path);
    void* GetUserData() const {return m_userdata;}

private:
    friend class DrawingDevice;

    ENUM_FileType m_type;

    void* m_userdata = nullptr;
    const char* m_filepath;
};