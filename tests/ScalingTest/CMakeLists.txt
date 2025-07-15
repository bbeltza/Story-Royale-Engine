include(StoryRoyaleEngine)

project(ScalingTest)

add_executable(ScalingTest ScalingTest.cpp)

srEngine_include_directories(ScalingTest)
srEngine_link_target(ScalingTest)