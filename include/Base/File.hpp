#pragma once
#include <standard>

#include <Base/File.h>
#include <Base/Chunk.h>
#include <Base/RWops.h>

#include <SDL_endian.h>
#include <SDL_filesystem.h>

extern "C" void ERROR(const char*, ...);

extern "C"
{
#if defined(_WIN32)
	extern const unsigned char* _game_res;
#else
	extern const unsigned char _game_res[];
#endif
};

namespace sre
{
	struct ChunkDeleter { void operator ()(const sre_Chunk* chunk) { return sre_chunkfree(chunk); } };
	using Chunk = std::unique_ptr<const sre_Chunk, ChunkDeleter>;
	class File: protected sre_File
	{
	public:
		File(): sre_File{} {}
		File(const char* path, const char* mode = NULL) { sre_fileopen(this, path, mode); }
		~File() { sre_fileclose(this); }

		inline bool valid() const { return this->fp.fp != NULL; }

		inline Chunk allocate(size_t max_size=0) const { return Chunk(sre_fileallocate(this, max_size)); }

		inline SDL_RWops* to_RWops() const { return sre_filetorwops(this); }

		inline const byte* resource_data() const
		{
			if (!embedded) // Could also check if the file is valid, but remember isembedded is true if the file is valid
				return NULL;

			return res.begin; // Even if it wasn't valid and this passes, res_begin will be NULL
		}

		size_t size() const { return sre_filesize(this); }

		template <typename Fn, typename... Args>
		auto call_cfunc(Fn func, Args&&... args) { return func(static_cast<sre_File*>(this), std::forward<Args>(args)...); }

		template <typename Fn, typename... Args>
		auto call_cfunc(Fn func, Args&&... args) const { return func(static_cast<const sre_File*>(this), std::forward<Args>(args)...); }
	};
}

#define File _File
#define Chunk _Chunk

// Still may need content for the file class to implement the rest of the stuff
class File
{
	union
	{
		// Handles for file streaming
		struct
		{
			FILE* ptr;
			const char* currmode;
			void* _unused1;
		} stream = { NULL };
		// Handles for resource streaming
		struct
		{
			const unsigned char* begin;
			size_t pos;
			size_t size;
		} res;
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
	File() {}
	File(const File& other);
	File(const char* path, const char* mode = NULL);

	void reopen(const char* path, const char* mode = NULL);

	int isValid() const { return stream.ptr != NULL; }

	Chunk allocate(size_t max_size = 0) const;
	SDL_RWops* toRWops() const;
	const void* resourceData() const
	{
		if (!isembedded) // Could also check if the file is valid, but remember isembedded is true if the file is valid
			return NULL;

		return res.begin; // Even if it wasn't valid and this passes, res_begin will be NULL
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

	// Read API

	std::vector<uint8_t> readBytes(size_t max_count = 0);
	size_t readBytes(void* buff, size_t count);

	template <class T> inline T read()
	{
		uint8_t data[sizeof(T)]; // Don't call T's constructor, if it has any, just allocate raw bytes
		readBytes(data, sizeof(T));

		return std::move(*reinterpret_cast<T*>(data));
	}

	template <class T> inline T readSwapped()
	{
		uint8_t tmp[sizeof(T)];
		uint8_t data[sizeof(T)];
		readBytes(tmp, sizeof(T));

		for (size_t i = 0; i < sizeof(T); i++)
			data[i] = tmp[(sizeof(T) - 1) - i];
		
		return std::move(*reinterpret_cast<T*>(data));
	}

	template <class T> inline T readLE() { return IS_LITTLE_ENDIAN ? read<T>() : readSwapped<T>(); }
	template <class T> inline T readBE() { return IS_BIG_ENDIAN ? read<T>() : readSwapped<T>(); }

	int readF(const char* fmt, ...);

	// Write API

	size_t writeBytes(const void* data, size_t size);

	template <class T> size_t write(T data) { return writeBytes(&data, sizeof(T)); }
	template <class T> size_t writeSwapped(T data)
	{
		uint8_t swap[sizeof(T)];
		for (size_t i = 0; i < sizeof(T); i++)
			swap[i] = reinterpret_cast<uint8_t*>(&data)[(sizeof(T) - 1) - i];

		return writeBytes(swap, sizeof(T));
	}

	template <class T> size_t writeLE(T data) { return IS_LITTLE_ENDIAN ? write(data) : writeSwapped(data); }
	template <class T> size_t writeBE(T data) { return IS_BIG_ENDIAN ? write(data) : writeSwapped(data); }

	static constexpr bool IS_LITTLE_ENDIAN = (SDL_BYTEORDER == SDL_LIL_ENDIAN);
	static constexpr bool IS_BIG_ENDIAN = (SDL_BYTEORDER == SDL_BIG_ENDIAN);
private:

	size_t resource_size() const
	{
		assert(isembedded && res.begin);

		return res.size;
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

#undef Chunk
#undef File