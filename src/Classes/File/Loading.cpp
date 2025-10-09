#define fs(x) (FILE *)x

#include "Classes/File.hpp"
#include "Sys.h"

const char res_prefix[] = "_res/";

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
            SDL_AtomicAdd(&info.ref, 1);

            if (!info.resbind)
            {
                info.resbind = true;

                unsigned int l = (int)strlen(REAL_path);

                const unsigned char *resptr = _game_res;
                while (*resptr != '\n')
                {
                    int cmp = strcmp((const char *)resptr, REAL_path);
                    if (!cmp)
                    {
                        //info.handle = (void *)resptr;
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
                    return syserror(FILE_NOT_FOUND, path);
            }
        }
        else
        {
            unsigned int s = (int)(strlen(path + strlen(RES_PREFIX)) + 56);
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
        if (s_loaded->count(path)) return;

        FILE* fstream = fopen(path, "rb");
        if (!fstream)
        {
            return syserror(FILE_NOT_FOUND, path);
        }

        FileInfo &info = m_info();

        SDL_AtomicAdd(&info.ref, 1);
        info.resbind = false;

        fseek(fstream, 0, SEEK_END);
        info.size = ftell(fstream);
        info.data = new unsigned char[info.size];
        fseek(fstream, 0, SEEK_SET);
        fread(info.data, 1, info.size, fstream);

        fclose(fstream);
    }

    m_type = T_TXT;
}