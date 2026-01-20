include(StoryRoyaleEngine)
project(AudioTest)

set(SOURCES AudioTest.cpp)

srEngine_build(AudioTest audio SOURCES NO_BIND)