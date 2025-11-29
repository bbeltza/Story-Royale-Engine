#pragma once
#include <standard>

#include <SDL_rwops.h>
#include <SDL_filesystem.h>

extern "C" void ERROR(const char*, ...);

extern "C"
{
#if defined(WIN32)
	extern const unsigned char* _game_res;
#else
	extern const unsigned char _game_res[];
#endif
};

class File
{
	union
	{
		// Handles for file streaming
		struct
		{
			FILE* stream;
			const char* currmode;
		};
		// Handles for resource streaming
		struct
		{
			const unsigned char* res_begin;
			size_t res_pos;
			size_t res_size;
		};
	};
	const char* currpath = NULL;
	bool isembedded = false;

public:
	struct Chunk
	{
		friend class File;

		const size_t size = 0;
		const char* const data = NULL;

		Chunk() {}
		Chunk(Chunk&& moving);
		Chunk(const Chunk& other) = delete; // If you want to copy a chunk, use File::allocate again

		Chunk& operator=(Chunk&& moving);

		~Chunk();
	private:
		Chunk(size_t size, const void* data) : size(size), data(static_cast<const char*>(data)) {}
	};

public:
	~File();
	// Create an invalid empty file stream
	File() : stream(NULL) {}
	File(const File& other);
	File(const char* path, const char* mode = NULL);

	void reopen(const char* path, const char* mode = NULL);

	int isValid() const { return stream != NULL; }

	Chunk allocate(size_t max_size = 0) const;
	SDL_RWops* toRWops() const;
	const void* resourceData() const
	{
		if (!isembedded) // Could also check if the file is valid, but remember isembedded is true if the file is valid
			return NULL;

		return this->res_begin; // Even if it wasn't valid and this passes, res_begin will be NULL
	}

	size_t getSize() const
	{
		if (!isValid())
		{
			ERROR("File::size(): could not tell the size of the File, file is not valid.");
			return 0;
		}

		if (isembedded)
			return resource_size();
		else
			return stream_size();
	}

	std::vector<uint8_t> readBytes(size_t max_count = 0);
	size_t readBytes(void* buff, size_t count);

	template <class T> inline T read()
	{
		uint8_t data[sizeof(T)]; // Don't call T's constructor, if it has any, just allocate raw bytes
		readBytes(data, sizeof(T));

		return *reinterpret_cast<T*>(data);
	}

	int readF(const char* fmt, ...);

private:

	size_t resource_size() const
	{
		assert(isembedded && res_begin);

		return this->res_size;
	}
	size_t stream_size() const;

	void load_resource();
	void load_stream();
	void error_notfound();

	static bool has_write(const char* mode);

	static constexpr size_t PREFIX_LENGTH = 6;
	static const char fsres_prefix[];
	static const char res_prefix[PREFIX_LENGTH + 1];
	static const char usr_prefix[PREFIX_LENGTH + 1];

	static bool path_hasprefix(const char* path, const char prefix[PREFIX_LENGTH]) { return !strncmp(prefix, path, PREFIX_LENGTH); }
};