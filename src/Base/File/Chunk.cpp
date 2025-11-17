#include "Base/File.hpp"
#include "utils/logging.h"


File::Chunk& File::Chunk::operator=(Chunk&& moving)
{
    *const_cast<size_t*>(&this->size) = moving.size;
    *const_cast<const char**>(&this->data) = moving.data;

    *const_cast<const char**>(&moving.data) = NULL;

    return *this;
}

File::Chunk::Chunk(Chunk &&moving) : size(moving.size), data(moving.data)
{
    *const_cast<const char**>(&moving.data) = NULL;
}

File::Chunk::~Chunk()
{
    if (data)
        operator delete(const_cast<char *const>(data));
}

File::Chunk File::allocate(size_t max_size)
{
    if (!isValid())
    {
        ERROR("Failed to allocate file data. File is not valid");
        return Chunk(0, NULL);
    }

    if (isembedded)
    {
        void* data = operator new(this->res_size);
        memcpy(data, this->res_begin, this->res_size);

        WARN("File::allocate(): Allocating embedded resource from %s. It is better to use the raw data instead (when it will be available)", this->currpath);

        return Chunk(this->res_size, data);
    }
    else
    {
        long old_pos = ftell(stream);

        {
            fseek(stream, 0L, SEEK_END);
            size_t file_size = ftell(stream);
            if (!max_size)
                max_size = file_size;
            else if (file_size < max_size)
            {
                WARN("Speficied a size higher than the file size (size: %zd & file size: %d) for %s. Using file size");
                max_size = file_size;
            }
        }

        rewind(stream);

        char *data = static_cast<char *>(operator new(max_size));
        fread(const_cast<char *>(data), max_size, 1, stream);

        fseek(stream, old_pos, SEEK_SET);

        return Chunk(max_size, data);
    }
}