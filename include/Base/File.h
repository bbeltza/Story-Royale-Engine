#ifndef SRE_FILE_H
#define SRE_FILE_H
#include <C_API.h>
#include <ints.h>
#include <stdio.h>
#include <string.h>

SRE_CAPI_BEGIN

#if defined(_WIN32)
	extern const sre_byte *_game_res;
#else
	extern const sre_byte _game_res[];
#endif

enum sre_FileConstants
{
    SRE_FILEPREFIX_LENGTH = 6
};

extern const char SRE_FSRES_PREFIX[];
extern const char SRE_RES_PREFIX[SRE_FILEPREFIX_LENGTH + 1];
extern const char SRE_USR_PREFIX[SRE_FILEPREFIX_LENGTH + 1];

static inline int sre_filehasprefix(const char* path, const char prefix[SRE_FILEPREFIX_LENGTH]) { return !strncmp(prefix, path, SRE_FILEPREFIX_LENGTH); }
static inline int sre_modehaswrite(const char* mode) { return strchr(mode, '+') || strchr(mode, 'a') || strchr(mode, 'w'); }

typedef struct sre_FP
{
    FILE* fp;
    char mode[sizeof(void*)]; // The mode of the stream, picks up the size of a pointer, but it typically won't use more than 4 bytes
    char* path;
} sre_FP;

typedef struct sre_FRES
{
    const sre_byte* begin;
    size_t pos;
    size_t size;
} sre_FRES;

typedef struct sre_File
{
    union
    {
        sre_FP fp;
        sre_FRES res;
    };
    int embedded;
} sre_File;

extern int sre_fileopen(sre_File* file, const char* path, const char* mode);
extern void sre_fileclose(sre_File* file);
extern size_t sre_filesize(const sre_File* file);

extern const sre_byte* sre_getresource(const char* path, size_t* size_output);

SRE_CAPI_END

#endif