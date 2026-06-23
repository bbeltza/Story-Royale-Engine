#include <Base/Error.h>
#include <Base/Log.h>

namespace sre
{
    extern "C" void initialize()
    {
        sre::log("Debug log %d '%s' (implicit)", 10, "format test");
        sre::log(SRE_LOG_DEBUG "Debug log %d '%s' (explicit)", 10, "format test");

        sre::log(SRE_LOG_INFO "Info log %d %f", 55, 1.1);
        sre::log(SRE_LOG_WARN "Warn log %d %f", 56, 1.2);
        
        sre::error(SRE_ERR_FAIL, "Error log");
        sre::logmsg("Critical log", "\x04");
    }
}