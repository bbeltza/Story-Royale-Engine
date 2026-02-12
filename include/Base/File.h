#ifndef SRE_FILE_H
#define SRE_FILE_H
#include <C_API.h>
#include <ints.h>
#include <string.h>

SRE_CAPI_BEGIN

typedef struct sre_File sre_File;
typedef void* sre_FileImpl; // File implementation type (it's just a void* to not be confused with the other vftable parameters)

// File virtual function table structure
typedef struct sre_FVFT sre_FVFT;

struct sre_File
{
    const struct sre_FVFT* vfptr;
    sre_FileImpl impl;
    // Maybe add some data like the path of the file?
};

enum sre_fileMode
{
    SRE_FILE_DEFAULT = 0,
    SRE_FILE_READ,
    SRE_FILE_WRITE,
    SRE_FILE_READWRITE,
    SRE_FILE_READWRITE_CREATE
};

typedef enum sre_seek
{
    SRE_SEEK_SET,
    SRE_SEEK_CUR,
    SRE_SEEK_END
} sre_seek;

bool sre_fileopen(sre_File* file, const char* path, int mode);
void sre_fileclose(sre_File* file);

size_t sre_fileread(const sre_File* file, void* data, size_t size);
size_t sre_filewrite(const sre_File* file, const void* data, size_t size);

bool sre_fileseek(const sre_File* file, long offset, sre_seek origin);
long sre_filetell(const sre_File* file);

size_t sre_filesize(const sre_File* file);
const sre_byte* sre_filebegin(const sre_File* file);

/*

enum sre_fileConstants
{
    SRE_FILEPREFIX_LENGTH = 6
};

extern const char SRE_FSRES_PREFIX[];
extern const char SRE_RES_PREFIX[SRE_FILEPREFIX_LENGTH + 1];
extern const char SRE_USR_PREFIX[SRE_FILEPREFIX_LENGTH + 1];

extern int sre_fileopen(sre_File* file, const char* path, const char* mode);
extern void sre_fileclose(sre_File* file);
extern size_t sre_filesize(const sre_File* file);

extern const sre_byte* sre_getresource(const char* path, size_t* size_output);

extern long sre_fileseek(const sre_File* file, long offset, int origin);
#define sre_filerewind(file) (sre_fileseek(file, 0, SEEK_SET) == 0);

extern bool sre_filewrite(const sre_File* file, const void* rawdata, size_t size);

*/

SRE_CAPI_END

#endif