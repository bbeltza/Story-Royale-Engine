#pragma once
#include <standard>

#include <SDL_rwops.h>

extern "C"
{
    extern const unsigned char _game_res[];
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
    const char* currpath=NULL;
    bool isembedded=false;

    public:
    struct Chunk
    {
        friend class File;

        const size_t size=0;
        const char* const data=NULL;

        Chunk(Chunk&& moving);
        Chunk(const Chunk& other) = delete; // If you want to copy a chunk, use File::allocate again

        ~Chunk();
        private:
        Chunk() {}
        Chunk(size_t size, const void* data): size(size), data(static_cast<const char*>(data)) {}
    };

    public:
    ~File();
    // Create an invalid empty file stream
    File(): stream(NULL) {}
    File(const File& other);
    File(const char* path, const char* mode=NULL);
    
    void reopen(const char* path, const char* mode=NULL);

    Chunk allocate(size_t max_size=0);

    int isValid() const { return stream != NULL; }

    SDL_RWops* toRWops() const;

    private:

    void load_resource();
    void load_stream();
    void error_notfound();

    static bool has_write(const char* mode);

    static bool path_has_resprefix(const char* path) { return strlen(path) < 6 ? false : !strncmp(res_prefix, path, 6); };

    static const char fsres_prefix[];
    static const char res_prefix[];
};