#ifndef SRE_FILESYSTEM_H
#define SRE_FILESYSTEM_H
#include <C_API.h>
#include <ints.h>

SRE_CAPI_BEGIN

bool sre_mkdir(const char* path);

SRE_CAPI_END

#ifdef __cplusplus

#include <string>
namespace sre
{
    bool mkdir(const char* path) { return sre_mkdir(path); }
    bool mkdir(const std::string& path) { return sre::mkdir(path.c_str()); }
}

#endif

#endif