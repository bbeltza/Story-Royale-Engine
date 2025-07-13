include(StoryRoyaleEngine)
include(srEngineResources)
project(RectsTest)

add_executable(RectsTest Rects.cpp)

srEngine_include_directories(RectsTest)
srEngine_set_resource_file(RectsTest res)
srEngine_link_target(RectsTest)