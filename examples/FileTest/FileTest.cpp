#include <Base/File.hpp>
#include <Entry.h>

#include <utils/logging.h>

void sre::initialize()
{
    File f("res://hello.txt");
    const char* data = (const char*)f.begin();
    assert(data != NULL);

    size_t size = f.size();
    LOG("%s", data);
    LOG("%zd", size);
}