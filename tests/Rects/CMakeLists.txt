include(StoryRoyaleEngine)
project(RectsTest)

add_executable(RectsTest Rects.cpp)

srEngine_include_directories(RectsTest)
srEngine_link_target(RectsTest)