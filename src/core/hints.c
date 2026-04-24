#include <Hints.h>
#include <SDL_loadso.h>

const void* sre_gethint(const char* name)
{    
    if (!name)
        return NULL; // Might abort but at least have erroring

    char buff[64] = "SRE_HINT_";
    {
        char* ptr = buff + 9; // strlen(buff -> "SRE_HINT_")
        size_t namelen = 0;
        do {
            *ptr = *name;
            name++;
            ptr++;
            namelen++;

            if (namelen > 64-9)
            {
                //  `name` length limit exceeded!
                return NULL;
            }
        } while (*name);
    }

    return SDL_LoadFunction(NULL, buff);
}