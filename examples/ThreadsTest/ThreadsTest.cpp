#include <Base/Thread.hpp>
#include <GameEntry.h>

#include <utils/logging.h>

#include <OS.h>

void testmyself(char a, float b) // Well it kind of doesn't work on floats. Looking for a fix (Like for Signals)
{
    LOG("a is %d and b is %f!", a, b); // It worked... I did magic! I'm a wizard!!!!.....
    delay_s(1);
    LOG("a is again... %d and b is %f!", a, b);
}

void sre::initialize()
{
    Thread thrd = Threads::Create(testmyself, 3, 2.0f);
    LOG("Thread has been finished");
}