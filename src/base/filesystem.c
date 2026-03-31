#include <Base/Filesystem.h>

static inline bool _mkdir_internal(const char*);

#if _WIN32
    #include <Windows.h>

    static inline bool _mkdir_internal(const char* path) { return CreateDirectoryA(path, NULL) != FALSE; }
#elif defined(SRE_HAVE_SYSSTAT_H)
    #include <sys/stat.h>
    static inline bool _mkdir_internal(const char* path) { return mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0; }
#else
    #error
#endif

bool sre_mkdir(const char* path) { return _mkdir_internal(path); }