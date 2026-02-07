// NOTE: This example will be removed in favor of coroutines, and of course a coroutine example will be present

#include <Core/Thread.hpp>
#include <Entry.h>

#include <utils/logging.h>

#include <OS.h>

int testmyself(char a, float b)
{
    LOG("a is %d and b is %f!", a, b);
    delay_s(1);
    LOG("a is again... %d and b is %f!", a, b);

    return 0;
}

void sre::initialize()
{
    sre::Thread thrd(testmyself, 3, 2.0f);
    LOG("Thread has been finished");
}