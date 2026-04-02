cmake_minimum_required(VERSION 3.20)

set(SRE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Directory of the Story Royale engine" FORCE)
set(SRE_CMAKE_MODULE_PATH ${SRE_DIR}/cmake)
if (${CMAKE_SOURCE_DIR} STREQUAL ${SRE_DIR})
    set(SRE_ISTOP ON)
else()
    set(SRE_ISTOP OFF)
endif()

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(SRE_ISDEBUG ON)
else()
    set(SRE_ISDEBUG OFF)
endif()

option(SRE_IMGUI "Integrate ImGui. A GUI library that's useful for debugging, along with its integration with the engine's world/entity system" ${SRE_ISDEBUG})
option(SRE_EXAMPLES "Build example applications" ${SRE_ISTOP})
option(SRE_TESTS "Build and run tests (Kinda abandoned)" ON)
option(SRE_INSTALL "Enable engine install targets (very experimental and currently being tested)" ${SRE_ISTOP})

set(CMAKE_MODULE_PATH ${SRE_CMAKE_MODULE_PATH})

if (ANDROID)
    set(BUILD_SHARED_LIBS ON)
else()
    set(BUILD_SHARED_LIBS OFF)
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(3rdparty)

include(sre/targets)
include(sre/game_build)
include(sre/checkheaders)
include(sre/installhelper)

add_subdirectory(src)

project(sre_private)
add_library(sre_private INTERFACE)
target_link_libraries(sre_private INTERFACE sre_werror)
target_compile_definitions(sre_private INTERFACE SRE=1)

project(sredll) # sre shared library?? :o (unimplemented...)
add_library(sredll SHARED src/dllmain.c)
target_link_libraries(sredll PUBLIC ${SRE_MODULES})

if(NOT SRE_ISTOP)
    set(CMAKE_ASM_COMPILE_OBJECT "${CMAKE_ASM_COMPILE_OBJECT}" PARENT_SCOPE)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SRE_CMAKE_MODULE_PATH} PARENT_SCOPE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} PARENT_SCOPE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} PARENT_SCOPE)
endif()

if (NOT SRE_IMGUI)
    target_compile_definitions(sre INTERFACE "IMGUI_DISABLE=1")
endif()

if (SRE_EXAMPLES)
    add_subdirectory(examples)
endif()

if (SRE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

install_wrapper(TARGETS ${SRE_MODULES})
install_wrapper(DIRECTORY include DESTINATION share/sre)
install_wrapper(DIRECTORY common DESTINATION share/sre)
install_wrapper(DIRECTORY scripts DESTINATION share/sre PATTERN __pycache__ EXCLUDE)
install_wrapper(DIRECTORY gen_src DESTINATION share/sre)
install_wrapper(DIRECTORY cmake DESTINATION .)