#include <Engine.hpp>

void testmyself(char a, float b) // Well it kind of doesn't work on floats. Looking for a fix (Like for Signals)
{
    printf("a is %d and b is %f!\n", a, b); // It worked... I did magic! I'm a wizard!!!!.....
    SDL_Delay(1000);
    printf("a is again... %d and b is %f!\n", a, b);
}

void Game::Initialize()
{
    Engine->ThreadPool.CreateImmediateThread(testmyself, 3, 2.0f);
}