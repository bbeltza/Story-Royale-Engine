include(StoryRoyaleEngine)
include(srEngineResources)
project(ImageTest)

add_executable(ImageTest ImageTest.cpp)

srEngine_include_directories(ImageTest)
SRENGINE_SET_RESOURCE_FILE(ImageTest res)
srEngine_link_target(ImageTest)