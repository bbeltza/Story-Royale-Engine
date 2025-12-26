#include <Base/File.hpp>
#include <Engine.hpp>

void sre::initialize()
{
    File f("res://hello.txt");
    LOG("Game res: %s\n", _game_res);
    const char* data = (const char*)f.resourceData();
    size_t size = f.getSize();
    LOG("%s\n", data);
    LOG("%zd\n", size);
}