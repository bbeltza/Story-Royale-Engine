#include <Classes/File.h>
#include <ECS.h>

void Game::Initialize(void)
{
    printf("Game res: %s\n", _game_res);
    File f;
    f.Load("res://hello.txt");
    printf("%s\n", f.getRawData());
    printf("%d\n", f.getType());
}