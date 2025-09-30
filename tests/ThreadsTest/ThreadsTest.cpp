#include <Engine.hpp>

void testmyself(char a, char b)
{
    printf("a is %d and b is %d!\n", a, b); // It worked... I did magic! I'm a wizard!!!!.....
    SDL_Delay(1000);
    printf("a is again... %d and b is %d!\n", a, b);
}

void Game::Initialize()
{
    Engine->ThreadPool.CreateImmediateThread(testmyself, 3, 2);
}