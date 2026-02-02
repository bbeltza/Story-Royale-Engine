include(sre)
project(ThreadsTest)

set(SOURCES ThreadsTest.cpp)

srEngine_build(ThreadsTest threads SOURCES)