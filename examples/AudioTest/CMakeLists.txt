include(sre)
project(AudioTest)

set(SOURCES AudioTest.cpp)

srEngine_property(AudioTest PROPERTIES RESOURCES "res")
srEngine_build(AudioTest audio SOURCES NO_BIND)