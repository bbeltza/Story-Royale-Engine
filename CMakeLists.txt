cmake_minimum_required(VERSION 3.20)

set(SRENGINE_CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(SRENGINE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_MODULE_PATH ${SRENGINE_CMAKE_MODULE_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib)

if (UNIX)
    enable_language(ASM)
endif()

if (ANDROID)
    set(BUILD_SHARED_LIBS ON)
else()
    set(BUILD_SHARED_LIBS OFF)
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(StoryRoyaleEngine)

project(sre)
project(sre_core)

project(sre_interface)
project(sre_noconsole)
add_library(sre_interface INTERFACE)
add_library(sre_noconsole INTERFACE)

add_subdirectory(src)
add_subdirectory(3rdparty)

list(TRANSFORM SOURCES PREPEND "src/")
list(TRANSFORM CORE_SOURCES PREPEND "src/")

add_library(sre STATIC ${SOURCES})
target_include_directories(sre PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(sre PUBLIC ${3RDPARTYLIBS})

add_library(sre_core STATIC ${CORE_SOURCES})
target_link_libraries(sre_core PRIVATE sre)
target_compile_definitions(sre_core PRIVATE INTERNAL_DEF=1)

target_link_libraries(sre PRIVATE sre_core)
target_link_libraries(sre PRIVATE sre_interface)

add_subdirectory(os)
target_link_libraries(sre PUBLIC sre_os)

if(MSVC)
    target_link_options(sre_noconsole INTERFACE "/SUBSYSTEM:WINDOWS")
    target_compile_options(sre_interface INTERFACE "/WX")
    target_compile_definitions(sre_interface INTERFACE _CRT_SECURE_NO_WARNINGS)
else()
    if (WIN32)
        target_link_options(sre_noconsole INTERFACE "-mwindows")
    endif()
    target_compile_options(sre_interface INTERFACE "-Werror")
endif()

if (ANDROID)
    target_link_libraries(StoryRoyaleEngine PUBLIC android log)
endif()

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    set(EXAMPLE_OPTIONS ON)
else()
    set(EXAMPLE_OPTIONS OFF)

    set(SRENGINE_DIR ${SRENGINE_DIR} PARENT_SCOPE)
    set(CMAKE_ASM_COMPILE_OBJECT "${CMAKE_ASM_COMPILE_OBJECT}" PARENT_SCOPE)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SRENGINE_CMAKE_MODULE_PATH} PARENT_SCOPE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} PARENT_SCOPE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} PARENT_SCOPE)
endif()

option(WANT_EXAMPLES "Build example applications" ${EXAMPLE_OPTIONS})
option(BUILD_TESTING "Build and run tests" ON)

if (WANT_EXAMPLES)
    add_subdirectory(examples)
endif()

if (BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()