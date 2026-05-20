#ifndef SRE_FILESYSTEM_H
#define SRE_FILESYSTEM_H
#include <C_API.h>
#include <ints.h>

SRE_CAPI_BEGIN

enum sreFDaction
{
    SRE_FD_OPEN,
    SRE_FD_SAVE
};
enum sreFDflags
{
    SRE_FDBIT_ALLOWMULTISELECT = 0x1,
    SRE_FDBIT_MUSTEXIST = 0x2,
};
typedef struct sre_FDfilter
{
    const char* nametag;
    const char* extension;
} sre_FDfilter;

typedef struct sre_FDpath
{
    size_t length;
    char path[
        #ifdef __cplusplus
            1
        #endif
    ];
} sre_FDpath;

struct sre_Window;

bool sre_mkdir(const char* path);

// Prompt the user to choose a file to save or open (depending on `action`), and return its path
// Note that this function is unfinished and not totally supported (it is only supported on Windows for now, however it won't be supported on every single platform)
sre_FDpath* sre_filedialog(enum sreFDaction action, struct sre_Window* window, const sre_FDfilter extension_filters[], size_t extension_count, int flags);
void sre_fdpathfree(sre_FDpath* fdpath);

extern const sre_FDpath* SRE_FDPATH_NOFILE;

SRE_CAPI_END

#ifdef __cplusplus

#include <string>
namespace sre
{
    using FDfilter = sre_FDfilter;
    class Window;

    inline bool mkdir(const char* path) { return sre_mkdir(path); }
    inline bool mkdir(const std::string& path) { return sre_mkdir(path.c_str()); }

    inline bool file_dialog(sreFDaction action, Window* window, const FDfilter filters[], size_t n, int flags, std::string& str)
    {
        sre_FDpath* fpath = sre_filedialog(action, reinterpret_cast<sre_Window*>(window), filters, n, flags);
        if (!fpath)
            return false;
        
        str.assign(fpath->path, fpath->length);
        sre_fdpathfree(fpath);
        return true;
    }

    template <size_t n>
    inline bool file_dialog(sreFDaction action, Window* window, const FDfilter (&filters)[], int flags, std::string& str)
    {
        return file_dialog(action, window, filters, n, flags, str);
    }

    inline bool file_dialog(sreFDaction action, Window* window, const FDfilter& filter, int flags, std::string& str)
    {
        return file_dialog(action, window, &filter, 1, flags, str);
    }
}

#endif

#endif
