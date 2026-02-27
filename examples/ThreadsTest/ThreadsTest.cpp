// NOTE: This example will be removed in favor of coroutines, and of course a coroutine example will be present

#include <Core/Thread.hpp>
#include <Entry.h>

#include <Base/Log.h>

#include <OS.h>

int testmyself(char a, float b)
{
    sre::log("a is %d and b is %f!", a, b);
    delay_s(1);
    sre::log("a is again... %d and b is %f!", a, b);

    return 0;
}

void sre::initialize()
{
    sre::Thread thrd(testmyself, 3, 2.0f);
    sre::log("Thread has been finished");
}