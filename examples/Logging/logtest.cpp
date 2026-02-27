#include <Base/Log.h>

namespace sre
{
    extern "C" void initialize()
    {
        sre::log("", 10);
        sre::log("Hello world2...");
        sre::log<LOGCATEGORY_ERROR>("%d", 10);
    }
}