#include <Base/File.hpp>
#include <Engine.hpp>

void sre::initialize()
{
    LOG("Game res: %s\n", _game_res);
    File f("res://hello.txt");
    const char* data = (const char*)f.resourceData();
    size_t size = f.getSize();
    LOG("%s\n", data);
    LOG("%zd\n", size);
}