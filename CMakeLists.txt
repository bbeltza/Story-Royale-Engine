cmake_minimum_required(VERSION 3.12)

set(SRENGINE_CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_MODULE_PATH ${SRENGINE_CMAKE_MODULE_PATH})
include(StoryRoyaleEngine)

set(BUILD_SHARED_LIBS OFF)
set(CXX_STANDARD C++14)

srEngine_policy()
if(MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

add_subdirectory(3rdparty)

project(StoryRoyaleEngine)

add_subdirectory(src)

set(3RDPARTYLIBS
SDL2::SDL2-static
SDL2_ttf::SDL2_ttf-static
SDL2_image::SDL2_image-static

fmt::fmt)
set(SRENGINE_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include

    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/SDL/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/SDL_ttf
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/SDL_image
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/fmt/include
)

srEngine_include_directories(StoryRoyaleEngine)
target_link_libraries(StoryRoyaleEngine ${3RDPARTYLIBS})

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    add_subdirectory(tests/Rects)
else()
    set(SRENGINE_INCLUDE_DIRS ${SRENGINE_INCLUDE_DIRS} PARENT_SCOPE)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SRENGINE_CMAKE_MODULE_PATH} PARENT_SCOPE)
endif()
