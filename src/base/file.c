#include <Base/File.h>

#include <assert.h>
#include <errno.h>

#include <utils/logging.h>
#include <utils/mem.h>

#include <SDL_filesystem.h>

struct sre_FVFT
{
	bool (*open)(sre_FileImpl* file, const char* path, int mode);
	void (*close)(sre_FileImpl file);

	size_t (*read)(sre_FileImpl file, void* data, size_t size);
	size_t (*write)(sre_FileImpl file, const void* data, size_t size);

	bool (*seek)(sre_FileImpl file, long offset, sre_seek origin);
	long (*tell)(sre_FileImpl file);

	size_t (*size)(sre_FileImpl file);
	const sre_byte* (*begin)(sre_FileImpl file); // Get beginning of file, returns NULL on stdio's implementation
};

#if _WIN32
	extern const sre_byte* _game_res;
#else
	extern const sre_byte _game_res[];
#endif
extern const char __game_title[];
extern const char __game_pwdres[];
extern const unsigned __game_lenres;

const char SRE_RES_PREFIX[] = "res://";
const char SRE_USR_PREFIX[] = "usr://";

#define SRE_FILEPREFIX_LENGTH (sizeof(SRE_RES_PREFIX) - 1)

extern const sre_FVFT SRE_STDIO_VFT;
extern const sre_FVFT SRE_RESOURCE_VFT;

#include "file/resource.c"
#include "file/stdio.c"

bool sre_fileopen(sre_File* file, const char* path, int mode)
{
	if (!file) return false;
	memset(file, 0, sizeof(*file));

	if (!path) return false;

#if _WIN32
	if (!_game_res)
		sre_win_resourceinit();
#endif

	bool ret = false;
	if (!strncmp(path, SRE_RES_PREFIX, SRE_FILEPREFIX_LENGTH))
	{
		assert(_game_res != NULL);

		const char* const relpath = path + SRE_FILEPREFIX_LENGTH;
		if (_game_res[0])
		{
			file->vfptr = &SRE_RESOURCE_VFT;
			ret = file->vfptr->open(&file->impl, relpath, mode);
		}
		else
		{
			file->vfptr = &SRE_STDIO_VFT;

			size_t pathlen = strlen(relpath) + __game_lenres;
			ut_dynsalloc(char, fullpath, pathlen);
			strcpy(fullpath, __game_pwdres);
			strcat(fullpath, relpath);

			ret = file->vfptr->open(&file->impl, fullpath, mode);
		}
	}
	else
	{
		file->vfptr = &SRE_STDIO_VFT;
		if (!strncmp(path, SRE_USR_PREFIX, SRE_FILEPREFIX_LENGTH))
		{
			const char* relpath = path + SRE_FILEPREFIX_LENGTH;
			char* fullpath = SDL_GetPrefPath(NULL, __game_title);
			fullpath = SDL_realloc(fullpath, strlen(fullpath) + strlen(relpath) + 1);
			strcat(fullpath, relpath);
			
			ret = file->vfptr->open(&file->impl, fullpath, mode);

			SDL_free(fullpath);
		}
		else
			ret = file->vfptr->open(&file->impl, path, mode);
	}

	if (!ret)
		ERROR("Could not open file '%s': %s", path, strerror(errno));
	return ret;
}

void sre_fileclose(sre_File* file)
{
	if (!file || !file->impl) return;
	assert(file->vfptr != NULL);

	file->vfptr->close(file->impl);
}

size_t sre_fileread(const sre_File* file, void* data, size_t size)
{
	if (!file || !data || !size) return 0;
	if (!file->impl) return 0;
	assert(file->vfptr != NULL);

	return file->vfptr->read(file->impl, data, size);
}

size_t sre_filewrite(const sre_File* file, const void* data, size_t size)
{
	if (!file || !data || !size) return 0;
	if (!file->impl) return 0;
	assert(file->vfptr != NULL);

	return file->vfptr->write(file->impl, data, size);
}

bool sre_fileseek(const sre_File* file, long offset, sre_seek origin)
{
	if (!file) return false;
	if (!file->impl) return false;
	assert(file->vfptr != NULL);

	return file->vfptr->seek(file->impl, offset, origin);
}

long sre_filetell(const sre_File* file)
{
	if (!file) return -1;
	if (!file->impl) return -1;
	assert(file->vfptr != NULL);

	return file->vfptr->tell(file->impl);
}

size_t sre_filesize(const sre_File* file)
{
	if (!file) return 0;
	if (!file->impl) return 0;
	assert(file->vfptr != NULL);

	return file->vfptr->size(file->impl);
}

const sre_byte* sre_filebegin(const sre_File* file)
{
	if (!file) return NULL;
	if (!file->impl) return NULL;
	assert(file->vfptr != NULL);

	return file->vfptr->begin(file->impl);
}