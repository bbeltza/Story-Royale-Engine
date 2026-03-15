#include <Base/Filesystem.h>

inline bool _mkdir_internal(const char*);

#if _WIN32
    #include <Windows.h>
    #error "I'm on linux right now so I cannot properly implement the internal but I'll do it"

    inline bool _mkdir_internal(const char* path) { return CreateDirectoryA(path, NULL) != FALSE; }
#elif __unix__
    #include <sys/stat.h>
    inline bool _mkdir_internal(const char* path) { return mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0; }
#else
    #error
#endif

bool sre_mkdir(const char* path) { return _mkdir_internal(path); }