#include <Base/File.hpp>
#include <Entry.h>

#include <utils/logging.h>

void sre::initialize()
{
    File f("res://hello.txt");
    LOG("Game res: %s", _game_res);
    const char* data = (const char*)f.resource_data();
    size_t size = f.size();
    LOG("%s", data);
    LOG("%zd", size);
}