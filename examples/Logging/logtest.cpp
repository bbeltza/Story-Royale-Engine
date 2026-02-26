#include <Base/Log.h>

namespace sre
{
    extern "C" void initialize()
    {
        sre::log("%d", 10);
        sre::log("Hello world2...");
    }
}