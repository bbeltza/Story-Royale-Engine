#include <Classes/Thread.hpp>
#include <GameEntry.h>

#include <Sys.h>

void testmyself(char a, float b) // Well it kind of doesn't work on floats. Looking for a fix (Like for Signals)
{
    syslogln("a is %d and b is %f!", a, b); // It worked... I did magic! I'm a wizard!!!!.....
    SDL_Delay(1000);
    syslogln("a is again... %d and b is %f!", a, b);
}

void Game::Initialize()
{
    Thread& thrd = Threads::Create(testmyself, 3, 2.0f);
    syslogln("Thread has been finished");
}