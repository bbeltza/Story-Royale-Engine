project(FileTest)
include(StoryRoyaleEngine)
include(srEngineResources)

add_executable(FileTest FileTest.cpp)

srEngine_include_directories(FileTest)
SRENGINE_SET_RESOURCE_FILE(FileTest assets)
srEngine_link_target(FileTest)