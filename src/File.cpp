#include <standard.h>
#include "File.h"
#include "SDL.hpp"

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

const bool File::s_resbind = _game_res != nullptr;
const char res_prefix[] = "_res/";

File::File() : m_type(Type::Uninitialized)
{
}

File::~File()
{
    if (!areResourcesBound() && m_info.handle)
        fclose(f m_info.handle);
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
        //for (int i = 0; i < 1092; i++)
            //printf("%d ", _game_res[i]);
        char *REAL_path;
        if (areResourcesBound())
        {
            REAL_path = (char *)(path + strlen(RES_PREFIX));
            m_filepath = REAL_path;
            unsigned int l = strlen(REAL_path);

            const unsigned char *resptr = _game_res;
            while (*resptr != '\n')
            {
                int cmp = strcmp((const char*)resptr, REAL_path);
                if (!cmp)
                {
                    printf("COMPARED\n");
                    m_info.handle = (void *)resptr;
                    printf("l: %d\n", l);
                    resptr += l + 1;
                    int32_t datapos = 0;
                    for (int i = 0; i <= 3; i++)
                    datapos |= (resptr[3 - i] << 8 * i);
                    printf("%d\n", datapos);
                    printf("datapos: %d\n", datapos);
                    
                    m_info.data = (unsigned char *)(_game_res + datapos);
                    printf("%d %d %d %d\n", m_info.data[0], m_info.data[1], m_info.data[2], m_info.data[3]);
                    m_info.size = m_info.data[3];
                    for (int i = 0; i <= 3; i++)
                        m_info.size |= (m_info.data[3 - i] << 8 * i);
                    printf("%zd\n", m_info.size);
                    m_info.data += 4;

                    break;
                }
                resptr += l + 4;
            }
            if (*resptr == '\n')
            {
                m_type = Type::Unknown;
                return (void)printf("Couldn't find file '%s', no such directory\n", path);
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

        m_info.handle = fopen(path, "rb");
        if (!m_info.handle)
        {
            m_type = Type::Unknown;
            return (void)printf("Couldn't find file '%s', no such directory\n", path);
        }

        fseek(f m_info.handle, 0, SEEK_END);
        m_info.size = ftell(f m_info.handle);
        m_info.data = new unsigned char[m_info.size];
        fseek(f m_info.handle, 0, SEEK_SET);
        fread(m_info.data, 1, m_info.size, f m_info.handle);
        printf("%zd\n", m_info.size);
        for (size_t i = 0; i < m_info.size; i++)
        {
            //putchar(m_info.data[i]);
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
                m_type = Type::Text;
            }
        }
}