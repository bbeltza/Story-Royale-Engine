#include <standard.h>
#include "Classes/File.h"
#include "SDL.hpp"

#include "System.h"

#define f (FILE *)

const char *IMAGE_TYPES[] = {
    "png",
    "jpg",
    "jpeg",
    "bmp",
    "svg",
    "webp",
    NULL};

const char *FONT_TYPES[] = {
    "ttf",
    NULL};

const char *SOUND_TYPES[] = {
    "mp3",
    "wav",
    "ogg",
    "flac",
    NULL
};

const bool File::s_resbind = _game_res != nullptr;

const char res_prefix[] = "_res/";

File::File() : m_type(Type::Uninitialized)
{
}

File::File(File &&moved)
    : m_type(moved.m_type), m_userdata(moved.m_userdata), m_filepath(moved.m_filepath)
{
}

File::~File()
{
    if (m_type == Type::Unknown || m_type == Type::Uninitialized) return;
}

File::Map& File::get_loaded()
{
    static Map lf;
    return lf;
}

static bool checkType(const char *ext, const char *_array[], File::Type _target, File::Type *_type)
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

void File::Load(const char *path)
{
    int rescmp;
    if (strlen(path) < 6)
        rescmp = -1;
    else
        rescmp = strncmp(RES_PREFIX, path, 6);

    if (!rescmp)
    {
        // for (int i = 0; i < 1092; i++)
        // printf("%d ", _game_res[i]);
        char *REAL_path;
        if (areResourcesBound())
        {
            REAL_path = (char *)(path + strlen(RES_PREFIX));
            m_filepath = REAL_path;

            FileInfo &info = m_info();

            if (!info.resbind)
            {
                info.resbind = true;

                unsigned int l = strlen(REAL_path);

                const unsigned char *resptr = _game_res;
                while (*resptr != '\n')
                {
                    int cmp = strcmp((const char *)resptr, REAL_path);
                    if (!cmp)
                    {
                        info.handle = (void *)resptr;
                        resptr += l + 1;
                        int32_t datapos = 0;
                        for (int i = 0; i <= 3; i++)
                            datapos |= (resptr[3 - i] << 8 * i);

                        info.data = (unsigned char *)(_game_res + datapos);
                        info.size = info.data[3];
                        for (int i = 0; i <= 3; i++)
                            info.size |= (info.data[3 - i] << 8 * i);
                        info.data += 4;

                        break;
                    }
                    resptr += strlen((const char*)resptr) + 5;
                }
                if (*resptr == '\n')
                {
                    m_type = Type::Unknown;
                    return (void)System::Error(System::FILE_NOT_FOUND, path);
                }
            }
        }
        else
        {
            unsigned int s = strlen(path + strlen(RES_PREFIX)) + 56;
            REAL_path = new char[s];
            strcpy_s(REAL_path, sizeof(res_prefix), res_prefix);
            strcat_s(REAL_path, s, path + sizeof(res_prefix));

            Load(REAL_path);
            delete[] REAL_path;
        }
    }
    else
    {
        m_filepath = path;
        FILE* fstream = fopen(path, "rb");
        if (!fstream)
        {
            m_type = Type::Unknown;
            return System::Error(System::FILE_NOT_FOUND, path);
        }

        FileInfo &info = m_info();

        info.resbind = false;
        info.handle = fstream;

        fseek(f info.handle, 0, SEEK_END);
        info.size = ftell(f info.handle);
        info.data = new unsigned char[info.size];
        fseek(f info.handle, 0, SEEK_SET);
        fread(info.data, 1, info.size, f info.handle);
        printf("%zd\n", info.size);
        for (size_t i = 0; i < info.size; i++)
        {
            //putchar(m_info().data[i]);
        }
    }

    const char *rchr = strrchr(path, '.');
    if (!rchr)
        m_type = Type::Text;
    else
    {
        if (!(*++rchr))
            m_type = Type::Text; // If it finds a null terminator right after the point, assume a text type
        else
        {
            if (checkType(rchr, IMAGE_TYPES, Type::Image, &m_type))
                return;
            if (checkType(rchr, FONT_TYPES, Type::Font, &m_type))
                return;
            if (checkType(rchr, SOUND_TYPES, Type::Sound, &m_type))
                return;
            m_type = Type::Text;
        }
    }
}