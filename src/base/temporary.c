#include <Base/Temporary.h>
#include <utils/math.h>
#include <utils/mem.h>

#if _WIN32
    #include <Windows.h>

    char* sre_temporarypath(char* buffer, size_t size)
    {
        CHAR buf[MAX_PATH+1];
        DWORD len = GetTempPath2A(sizeof(buf), buf)+1;
        if (len == 0)
        {
            return NULL;
        }

        if (buffer)
            return strncpy(buffer, buf, ut_min(len, size));

        buffer = sre_new(len + size);
        return strncpy(buffer, buf, len);
    }
#else
    #include <string.h>
    #include <stdlib.h>

    char* sre_temporarypath(char* buffer, size_t size)
    {
        const char* tmpdir = getenv("TMPDIR");
        tmpdir = tmpdir ? tmpdir : "/tmp/";
        size_t len = strlen(tmpdir)+1;

        if (buffer)
            return strncpy(buffer, tmpdir, ut_min(len, size));
        
        buffer = sre_new(len + size);
        return strncpy(buffer, tmpdir, len);
    }
#endif