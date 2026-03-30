cmake_minimum_required(VERSION 3.20)

option(SRE_MINIMAL "Build binaries, without building the rest (Resource folderd, icons, etc...)" OFF)

set(SRE_CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(SRE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_MODULE_PATH ${SRE_CMAKE_MODULE_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib)

if (ANDROID)
    set(BUILD_SHARED_LIBS ON)
else()
    set(BUILD_SHARED_LIBS OFF)
endif()

if (UNIX)
    enable_language(ASM)
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CheckIncludeFile)
include(sre)

project(sre)
add_library(sre INTERFACE)

add_subdirectory(3rdparty)
add_subdirectory(src)

target_include_directories(sre INTERFACE ${SRE_DIR}/common)
target_include_directories(sre INTERFACE ${SRE_DIR}/include)
target_link_libraries(sre INTERFACE ${3RDPARTYLIBS})

project(sre_noconsole)
project(sre_werror)
add_library(sre_noconsole INTERFACE)
add_library(sre_werror INTERFACE)

project(sre_private)
add_library(sre_private INTERFACE)
target_link_libraries(sre_private INTERFACE sre_werror)
target_compile_definitions(sre_private INTERFACE SRE=1)

set(SRE_MODULES
    sre # SRE global interface target
    sre_main # Contains the entry point, as well as some argument helpers
    sre_core # Engine core library
    sre_base # Engine base library
    sreECS # Scene and entity manager library
    sreECS_presets # Scene and entity presets (only has components and camera controllers...) (optional)
    sreGUI # GUI manager library
    sreGUI_presets # UI presets (optional)
)
project(srelib)
add_library(srelib INTERFACE)
target_link_libraries(srelib INTERFACE ${SRE_MODULES})

project(sredll) # sre shared library?? :o (unimplemented...)
add_library(sredll SHARED src/internal.h)
target_link_libraries(sredll PUBLIC ${SRE_MODULES})

if(MSVC)
    target_link_options(sre_noconsole INTERFACE "/SUBSYSTEM:WINDOWS")
    target_compile_options(sre_werror INTERFACE "/WX")
    target_compile_definitions(sre_werror INTERFACE _CRT_SECURE_NO_WARNINGS)
else()
    if (WIN32)
        target_link_options(sre_noconsole INTERFACE "-mwindows")
    endif()
    target_compile_options(sre_werror INTERFACE "-Werror")
endif()
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(sre_werror INTERFACE "-Wno-vla-extension" "-Wno-deprecated-declarations")
endif()

if (ANDROID)
    target_link_libraries(sre INTERFACE android log)
else()
    check_include_file(ucontext.h HAVE_UCONTEXT_H)
endif()

check_include_file(sys/stat.h HAVE_SYSSTAT_H)
if (HAVE_UCONTEXT_H)
    target_compile_definitions(sre INTERFACE "HAVE_UCONTEXT_H")
endif()
if (HAVE_SYSSTAT_H)
    target_compile_definitions(sre INTERFACE "HAVE_SYSSTAT_H")
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