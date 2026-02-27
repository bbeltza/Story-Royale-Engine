#include <Base/File.hpp>
#include <Entry.h>

#include <Base/Log.h>

void sre::initialize()
{
    File f("res://hello.txt");
    const char* data = (const char*)f.begin();
    assert(data != NULL);

    size_t size = f.size();
    sre::log("%s", data);
    sre::log("%zd", size);
}