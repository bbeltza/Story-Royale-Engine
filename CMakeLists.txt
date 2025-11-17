cmake_minimum_required(VERSION 3.19)

set(SRENGINE_CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(SRENGINE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_MODULE_PATH ${SRENGINE_CMAKE_MODULE_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib)

if (ANDROID)
    set(BUILD_SHARED_LIBS ON)
else()
    set(BUILD_SHARED_LIBS OFF)
endif()
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(StoryRoyaleEngine)

project(StoryRoyaleEngine)
project(StoryRoyaleEngineCore)

srEngine_policy()

if(MSVC)
    set(SR_ENGINE_NO_CONSOLE_OPTIONS "/SUBSYSTEM:WINDOWS")
    set(SR_ENGINE_COMPILER_OPTIONS "/WX")
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
else()
    if (WIN32)
        set(SR_ENGINE_NO_CONSOLE_OPTIONS "-mwindows")
    endif()
    set(SR_ENGINE_COMPILER_OPTIONS "-Werror")
endif()

add_subdirectory(gen_src)
add_subdirectory(src)

add_subdirectory(3rdparty)

target_compile_definitions(StoryRoyaleEngine PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_features(StoryRoyaleEngine PRIVATE cxx_std_11 c_std_99)
target_compile_options(StoryRoyaleEngine PRIVATE "${SR_ENGINE_COMPILER_OPTIONS}")

if(NOT BUILD_SHARED_LIBS)
    set(SDL_LIB_SUFFIX "-static")
    set(ZLIB_SUFFIX "static")
endif()

set(3RDPARTYLIBS
SDL2::SDL2${SDL_LIB_SUFFIX}
SDL2_ttf::SDL2_ttf${SDL_LIB_SUFFIX}
SDL2_image::SDL2_image${SDL_LIB_SUFFIX}

stb
zlib${ZLIB_SUFFIX}
)
set(SRENGINE_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include

    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/SDL/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/SDL_ttf
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/SDL_image
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/stb
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/zlib
)

target_include_directories(StoryRoyaleEngine PUBLIC ${SRENGINE_INCLUDE_DIRS})
target_include_directories(StoryRoyaleEngineOS PUBLIC ${SRENGINE_INCLUDE_DIRS})
target_include_directories(StoryRoyaleEngineCore PUBLIC ${SRENGINE_INCLUDE_DIRS})

target_link_libraries(StoryRoyaleEngine PUBLIC ${3RDPARTYLIBS})
target_link_libraries(StoryRoyaleEngine PRIVATE StoryRoyaleEngineCore)
if (ANDROID)
    target_link_libraries(StoryRoyaleEngine PUBLIC
    android
    log
    )
endif()

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    set(BUILD_TESTS ON)
    add_subdirectory(tests/Rects)
    add_subdirectory(tests/FileTest)
    add_subdirectory(tests/AudioTest)
    add_subdirectory(tests/ImageTest)
    add_subdirectory(tests/ScalingTest)
    add_subdirectory(tests/ThreadsTest)
    add_subdirectory(tests/TargetTextures)
    add_subdirectory(tests/GlyphTest)
else()
    set(SR_ENGINE_NO_CONSOLE_OPTIONS ${SR_ENGINE_NO_CONSOLE_OPTIONS} PARENT_SCOPE)
    set(SRENGINE_DIR ${SRENGINE_DIR} PARENT_SCOPE)
    set(SRENGINE_INCLUDE_DIRS ${SRENGINE_INCLUDE_DIRS} PARENT_SCOPE)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SRENGINE_CMAKE_MODULE_PATH} PARENT_SCOPE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} PARENT_SCOPE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} PARENT_SCOPE)
endif()