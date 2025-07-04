cmake_minimum_required(VERSION 3.10)

set(BUILD_SHARED_LIBS OFF)

cmake_policy(SET CMP0079 NEW)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_subdirectory(3rdparty)

project(StoryRoyaleEngine)

add_subdirectory(src)

target_include_directories(StoryRoyaleEngine PRIVATE "include/")

target_include_directories(StoryRoyaleEngine PUBLIC 3rdparty/fmt/include)

target_link_libraries(StoryRoyaleEngine SDL2::SDL2-static)
target_link_libraries(StoryRoyaleEngine SDL2_ttf::SDL2_ttf-static)
target_link_libraries(StoryRoyaleEngine SDL2_image::SDL2_image-static)
target_link_libraries(StoryRoyaleEngine fmt)

add_subdirectory(tests/CircleTest)
