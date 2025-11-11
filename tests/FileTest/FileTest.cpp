#include <Base/File.hpp>
#include <Engine.hpp>

void Game::Initialize(void)
{
    printf("Game res: %s\n", _game_res);
    File f("res://hello.txt");
    File::Chunk ch = f.allocate();
    LOG("%s\n", ch.data);
    LOG("%zd\n", ch.size);
}