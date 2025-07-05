cmake_minimum_required(VERSION 3.12)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(BUILD_SHARED_LIBS OFF)
set(CXX_STANDARD C++14)

cmake_policy(SET CMP0079 NEW)
cmake_policy(SET CMP0091 NEW)
if(MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

add_compile_definitions(_CMAKE)

add_subdirectory(3rdparty)

project(StoryRoyaleEngine)

add_subdirectory(src)

target_include_directories(StoryRoyaleEngine PRIVATE "include/")

target_include_directories(StoryRoyaleEngine PUBLIC 3rdparty/fmt/include)

target_link_libraries(StoryRoyaleEngine SDL2::SDL2-static)
target_link_libraries(StoryRoyaleEngine SDL2_ttf::SDL2_ttf-static)
target_link_libraries(StoryRoyaleEngine SDL2_image::SDL2_image-static)
target_link_libraries(StoryRoyaleEngine fmt::fmt)

add_subdirectory(tests/CircleTest)
