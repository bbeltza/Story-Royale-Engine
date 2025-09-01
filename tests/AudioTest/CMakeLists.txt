include(StoryRoyaleEngine)
include(srEngineResources)
project(AudioTest)

add_executable(AudioTest AudioTest.cpp)

srEngine_include_directories(AudioTest)
srEngine_set_resource_file(AudioTest res)
srEngine_link_target(AudioTest)