cmake_minimum_required(VERSION 3.20)

set(SRE_CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(SRE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_MODULE_PATH ${SRE_CMAKE_MODULE_PATH})
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

include(CheckIncludeFile)
include(sre)

project(sre)
project(sre_core)

project(sre_interface)
project(sre_noconsole)
project(sre_werror)
add_library(sre_interface INTERFACE)
add_library(sre_noconsole INTERFACE)
add_library(sre_werror INTERFACE)

add_subdirectory(src)
add_subdirectory(3rdparty)

list(TRANSFORM SOURCES PREPEND "src/")
list(TRANSFORM CORE_SOURCES PREPEND "src/")

add_library(sre STATIC ${SOURCES})
target_include_directories(sre PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(sre PUBLIC ${3RDPARTYLIBS})

add_library(sre_core STATIC ${CORE_SOURCES})
target_link_libraries(sre_core PRIVATE sre)
target_link_libraries(sre_core PRIVATE sre_interface)
target_compile_definitions(sre_core PRIVATE INTERNAL_DEF=1)

target_link_libraries(sre_core PRIVATE sre_driverinterface)

target_link_libraries(sre PRIVATE sre_core)
target_link_libraries(sre PRIVATE sre_interface)
target_link_libraries(sre_interface INTERFACE sre_werror)

target_sources(sre PUBLIC "src/heap.cpp")

add_subdirectory(os)
target_link_libraries(sre PUBLIC sre_os)

target_compile_definitions(sre_interface INTERFACE SRE=1)

if(MSVC)
    target_link_options(sre_noconsole INTERFACE "/SUBSYSTEM:WINDOWS")
    target_compile_options(sre_werror INTERFACE "/WX")
else()
    if (WIN32)
        target_link_options(sre_noconsole INTERFACE "-mwindows")
    endif()
    target_compile_options(sre_werror INTERFACE "-Werror")
endif()
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(sre_werror INTERFACE "-Wno-vla-extension")
endif()
target_compile_definitions(sre_werror INTERFACE _CRT_SECURE_NO_WARNINGS)


if (ANDROID)
    target_link_libraries(sre PUBLIC android log)
endif()

check_include_file(ucontext.h HAVE_UCONTEXT_H)
if (HAVE_UCONTEXT_H)
    target_compile_definitions(sre PUBLIC "HAVE_UCONTEXT_H")
endif()

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    set(EXAMPLE_OPTIONS ON)
else()
    set(EXAMPLE_OPTIONS OFF)

    set(SRE_DIR ${SRE_DIR} PARENT_SCOPE)
    set(CMAKE_ASM_COMPILE_OBJECT "${CMAKE_ASM_COMPILE_OBJECT}" PARENT_SCOPE)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SRE_CMAKE_MODULE_PATH} PARENT_SCOPE)
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