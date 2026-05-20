#include <Core/Window.h>
#include <Base/Filesystem.h>
#include <Base/Error.h>
#include <Base/Log.h>
#include <assert.h>

#include <stdio.h>
#include <utils/mem.h>

static const sre_FDpath s_fdpath_nofile = {
    .length = 0,
    .path = ""  
};

const sre_FDpath* SRE_FDPATH_NOFILE = &s_fdpath_nofile;

static inline sre_FDpath* fdpath_new(int length)
{
    sre_FDpath* fdpath = sre_new(sizeof(sre_FDpath) + length*sizeof(fdpath->path[0]));
    fdpath->length = length;

    return fdpath;
}

void sre_fdpathfree(sre_FDpath* fdpath)
{
    if (fdpath == SRE_FDPATH_NOFILE)
        return;
    
    sre_delete(fdpath);
}

static inline bool _mkdir_internal(const char*);

#if _WIN32
    #include <Windows.h>

    static inline bool _mkdir_internal(const char* path) { return CreateDirectoryA(path, NULL) != FALSE; }
    
    static inline sre_FDpath* _filedialog_native(enum sreFDaction action, HWND hwnd, const sre_FDfilter extension_filters[], size_t extension_count, int flags)
    {
        BOOL result;
        CHAR filebuf[MAX_PATH] = "";
        OPENFILENAMEA openfilename = {
            .lStructSize = sizeof(OPENFILENAMEA),
            .hwndOwner = hwnd,
            .lpstrFilter = "All Files\0*.*\0\0",
            .lpstrFile = filebuf,
            .nMaxFile = MAX_PATH,
            .Flags = OFN_CREATEPROMPT | OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST
        };

        if (flags & SRE_FDBIT_ALLOWMULTISELECT)
            openfilename.Flags |= OFN_ALLOWMULTISELECT;
        if (flags & SRE_FDBIT_MUSTEXIST)
        {
            openfilename.Flags |= OFN_FILEMUSTEXIST;
            openfilename.Flags &= ~OFN_CREATEPROMPT;
        }

        if (extension_filters) // Little parser that converts `sre_FDfilter`s into `OPENFILENAME`s filter strings :)
        {
            size_t buffersize = 1; // 1 for the remaining double null-terminator
            for (size_t i = 0; i < extension_count; i++)
            {
                buffersize += strlen(extension_filters[i].nametag) + 1;
                buffersize += strlen(extension_filters[i].extension) + 1;
            }

            CHAR* buf = LocalAlloc(LMEM_FIXED, buffersize);
            CHAR* bufptr = buf;
            for (size_t i = 0; i < extension_count; i++)
            {
                assert((size_t)(bufptr - buf) < buffersize);

                const CHAR* extptr = extension_filters[i].nametag;
                do
                {
                    *bufptr = *extptr;
                    bufptr++;
                } while (*extptr++);

                extptr = extension_filters[i].extension;
                do
                {
                    *bufptr = *extptr;
                    bufptr++;
                } while (*extptr++);
            }
            *bufptr = '\0';

            openfilename.lpstrFilter = buf;
        }

        BOOL (APIENTRY* pDlgFun)(LPOPENFILENAMEA);
        switch (action)
        {
            case SRE_FD_OPEN:
                pDlgFun = GetOpenFileNameA;
                break;
            case SRE_FD_SAVE:
                pDlgFun = GetSaveFileNameA;
                break;
        }

        result = pDlgFun(&openfilename);

        if (extension_filters)
            LocalFree((HLOCAL)openfilename.lpstrFilter);

        if (result == TRUE)
        {
            WORD offs = max(openfilename.nFileExtension, openfilename.nFileOffset);
            size_t len = strlen(filebuf + offs) + offs + 1;
            sre_FDpath* fdpath = fdpath_new((int)len);
            strncpy(fdpath->path, filebuf, len);
            return fdpath;
        }
        else
        {
            DWORD errorex = CommDlgExtendedError();
            if (errorex)
            {
                // Print the error if `errorex` is non-zero?
                return NULL;
            }

            return (sre_FDpath*)SRE_FDPATH_NOFILE;
        }
    }
#else
    static inline sre_FDpath* _filedialog_native(enum sreFDaction action, void* hwnd, const sre_FDfilter extension_filters[], size_t extension_count, int flags)
    {
        sre_error(SRE_ERR_FAIL, "sre_filedialog is unsupported (currently only supported on Windows)");
        return NULL;
    }
#endif
#if defined(SRE_HAVE_SYSSTAT_H)
    #include <sys/stat.h>
    static inline bool _mkdir_internal(const char* path) { return mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0; }
#endif

#include <SDL_syswm.h>

bool sre_mkdir(const char* path) { return _mkdir_internal(path); }
sre_FDpath* sre_filedialog(enum sreFDaction action, struct sre_Window* window, const sre_FDfilter extension_filters[], size_t extension_count, int flags)
{
    if (window)
    {
        sre_error(SRE_ERR_INVALID_PARAMETER, "Parameter `window` has to be NULL for now, for safety reasons, since it is reserved for future use when a new window system refactor comes... Specifying `NULL` as the window will use the main game's window.");
        return NULL;
    }

    if (extension_count)
    {
        if (!extension_filters)
        {
            sre_error(SRE_ERR_INVALID_VALUE, "`extension_filters` is NULL while `extension_count` is non-zero. ");
            return NULL;
        }
    }
    else
    {
        if (extension_filters)
        {
            sre_log(SRE_LOG_WARN "`extension_count` is 0 while `extension_filters` is not NULL, so it'll be ignored. It is preferable to set it as NULL, set it to silence this warning.");
            extension_filters = NULL;
        }
    }

    SDL_Window* sdlwnd = (SDL_Window*)sre_mainwindow(); // Returns the SDL window handle for now, there will be a function that returns the actual OS specific window handle
    SDL_SysWMinfo wminfo;
    SDL_GetVersion(&wminfo.version);
    if (SDL_GetWindowWMInfo(sdlwnd, &wminfo) != SDL_TRUE)
    {
        sre_error(SRE_ERR_FAIL, "SDL_GetWindowWMInfo failed...");
        return NULL;
    }
    
    #if _WIN32
        #define _NATIVE_WND wminfo.info.win.window
    #else
        #define _NATIVE_WND NULL
    #endif

    if (flags & SRE_FDBIT_ALLOWMULTISELECT)
        sre_log(SRE_LOG_WARN "`flags` has the SRE_FDBIT_ALLOWMULTISELECT bit set. Multi selection is not fully supported so a few bugs might be experienced...");
    
    return _filedialog_native(action, _NATIVE_WND, extension_filters, extension_count, flags);
}
