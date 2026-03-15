#ifndef SRE_TEMPORARY_H
#define SRE_TEMPORARY_H
#include <C_API.h>
#include <ints.h>

SRE_CAPI_BEGIN

/*
 * Get the system's temporary path, useful for things like caching resources...
 * @param buffer The buffer where to store the temporary path, or `NULL` to allocate a buffer with `sre_new()`
 * @param size If `buffer` is not `NULL`, then the maximum amount of bytes to allocate. If `buffer` is `NULL`, then an amount of extra bytes to allocate in the extra buffer
 * @return `buffer` if it's not `NULL`, otherwise the newly allocated buffer, or `NULL` if the function fails
 * @remark If `buffer` is `NULL`, then don't forget to free the allocated buffer using `sre_delete()`!
 */
char* sre_temporarypath(char* buffer, size_t size);

SRE_CAPI_END

#ifdef __cplusplus

#include <string>
#include <memory>
namespace sre
{
    // Refer to `sre_temporarypath()`
    inline char* temporary_path(char* buffer, size_t size) { return sre_temporarypath(buffer, size); }
    // std::string `temporary_path()` helper wrapper
    inline std::string temporary_path()
    {
        std::unique_ptr<char> buf{temporary_path(NULL, 0)};
        std::string str(buf.get());

        return str;
    }
}

#endif

#endif