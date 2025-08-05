# TODO:
#[[

!!: Done!
!: Working on!


- Window running; !!
- Window updating/filling the background with a color; !!
- Instancing/Scenes; !!
- Collision detection; !!
- Rendering instances/entities; !!
- Input handling; !!
- Components; !!

- User interface layers; !!

- Rendering text; !!
- Rendering textures; !!
- Resource binder; !!

- Additional structs:
    - Timers; !!
    - Tweens !!

- Audio; !!

- Linking with ImGui (maybe before uhhh); *maybe after

- Work on the game!; !

]]

cmake_minimum_required(VERSION 3.12)

set(SRENGINE_CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(SRENGINE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${SRENGINE_CMAKE_MODULE_PATH})
include(StoryRoyaleEngine)

set(BUILD_SHARED_LIBS OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

srEngine_policy()
if(MSVC)
    set(NO_CONSOLE_OPTIONS "/SUBSYSTEM:WINDOWS")

    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
else()
    set(NO_CONSOLE_OPTIONS "-mwindows")
endif()


add_subdirectory(3rdparty)

project(StoryRoyaleEngine)
project(no_bind)
add_library(no_bind ${CMAKE_MODULE_PATH}/empty_res.c)

add_subdirectory(src)

target_compile_definitions(StoryRoyaleEngine PRIVATE _CRT_SECURE_NO_WARNINGS)

set(3RDPARTYLIBS
SDL2::SDL2-static
SDL2_ttf::SDL2_ttf-static
SDL2_image::SDL2_image-static
SDL2_mixer::SDL2_mixer-static

fmt::fmt)
set(SRENGINE_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include

    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/SDL/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/SDL_ttf
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/SDL_image
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/SDL_mixer
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/fmt/include
)

target_include_directories(StoryRoyaleEngine PUBLIC ${SRENGINE_INCLUDE_DIRS})
#srEngine_include_directories(StoryRoyaleEngine)
target_link_libraries(StoryRoyaleEngine ${3RDPARTYLIBS})

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    set(BUILD_TESTS)
    add_subdirectory(tests/Rects)
    add_subdirectory(tests/ImageTest)
    add_subdirectory(tests/FileTest)
    add_subdirectory(tests/ScalingTest)
else()
    set(NO_CONSOLE_OPTIONS ${NO_CONSOLE_OPTIONS} PARENT_SCOPE)
    set(SRENGINE_DIR ${SRENGINE_DIR} PARENT_SCOPE)
    set(SRENGINE_INCLUDE_DIRS ${SRENGINE_INCLUDE_DIRS} PARENT_SCOPE)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SRENGINE_CMAKE_MODULE_PATH} PARENT_SCOPE)
endif()
