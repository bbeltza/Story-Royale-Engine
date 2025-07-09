#include <standard.h>
#include "File.h"

const char* IMAGE_TYPES[] = {
    "png",
    "jpg",
    "jpeg",
    "bmp",
    "svg",
    "webp",
    NULL
};

const char* FONT_TYPES[] = {
    "ttf",
    NULL
};



File::File(): m_type(UNINITIALIZED)
{

}

File::~File()
{

}

static bool checkType(const char* ext, const char* _array[], ENUM_FileType _target, ENUM_FileType* _type)
{
    uint8_t n = 0;
    while (_array[n])
    {
        if (!strcmp(ext, _array[n]))
        {
            *_type = _target;
            return true;
        }

        n++;
    }
    return false;
}

void File::Load(const char* path)
{
    int rescmp;
    if (strlen(path) < 6)
        rescmp = -1;
    else
        rescmp = strncmp(RES_PREFIX, path, 6);

    if (!rescmp)
    {
        unsigned int s = strlen(path + 6) + 5;
        char* REAL_path = new char[s];
        strcpy_s(REAL_path, 5, "res/");
        strcat_s(REAL_path, s, path + 6);

        return Load(REAL_path);

        delete[] REAL_path;
    }
    else
    {
        m_filepath = path;

        const char* rchr = strrchr(path, '.');
        if (!rchr) m_type = TEXT;
        else
        {
            if (!(*rchr++)) m_type = UNKNOWN; // If it finds a null terminator right after the point, return an unknown type
            else
            {
                if (checkType(rchr, IMAGE_TYPES, IMAGE, &m_type)) return;
                if (checkType(rchr, FONT_TYPES, FONT, &m_type)) return;
                m_type = TEXT;
            }
        }
    }
}