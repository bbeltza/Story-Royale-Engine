if (NOT EXISTS ${SRE_DIR}/VERSION.txt)
    message(WARNING "VERSION.txt NOT FOUND. Engine directory corrupt?")
else()
    file(READ ${SRE_DIR}/VERSION.txt SRE_VER LIMIT 8)
    set(PROJ_SRE_VER VERSION ${SRE_VER})

    set(SRE_VERSION ${SRE_VER} CACHE STRING "The Engine version: ${SRE_VER}")
endif()

project(sre ${PROJ_SRE_VER})
add_library(sre INTERFACE)

project(sre_noconsole)
project(sre_werror)
add_library(sre_noconsole INTERFACE)
add_library(sre_werror INTERFACE)

set(SRE_MODULES
    sre # SRE global interface target
    sre_main # Contains the entry point, as well as some argument helpers
    sre_core # Engine core library
    sre_base # Engine base library
    sre_drivers # All of the engine's (render) drivers
    sreECS # Scene and entity manager library
    sreECS_presets # Scene and entity presets (only has components and camera controllers...) (optional)
    sreGUI # GUI manager library
    sreGUI_presets # UI presets (optional)

    sre_imgui # ImGui
)

if(NOT BUILD_SHARED_LIBS)
    set(_SDL_LIB_SUFFIX "-static")
    set(_ZLIB_SUFFIX "static")
endif()

set(SRE_3RDPARTYLIBS
    SDL2::SDL2${_SDL_LIB_SUFFIX}
    SDL2_image::SDL2_image${_SDL_LIB_SUFFIX}
    
    freetype
    #zlib${_ZLIB_SUFFIX}
)

project(srelib)
add_library(srelib INTERFACE)
target_link_libraries(srelib INTERFACE ${SRE_MODULES})

# Setup interface targets

target_include_directories(sre INTERFACE ${SRE_DIR}/common)
target_include_directories(sre INTERFACE ${SRE_DIR}/include)
target_link_libraries(sre INTERFACE ${SRE_3RDPARTYLIBS})

# Compiler specific argument options

if(MSVC)
    target_link_options(sre_noconsole INTERFACE "/SUBSYSTEM:WINDOWS")
    target_compile_options(sre_werror INTERFACE "/WX")
    target_compile_definitions(sre_werror INTERFACE _CRT_SECURE_NO_WARNINGS)
else()
    if (WIN32)
        target_link_options(sre_noconsole INTERFACE "-mwindows")
    endif()
    target_compile_options(sre_werror INTERFACE "-Werror")
    target_link_options(sre INTERFACE "-Wl,--start-group") # Incremental linking, this is needed for non-MSVC compilers
    target_link_options(sre INTERFACE "-Wl,--dynamic-list=${CMAKE_MODULE_PATH}/hints.list")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(sre_werror INTERFACE "-Wno-vla-extension" "-Wno-deprecated-declarations")
endif()

if (ANDROID)
    include(sre/game_apk)
    # Support for 16kb aligned memory pages (Support for disabling the option might come)
    target_link_options(sre INTERFACE "-Wl,-z,max-page-size=16384")
    target_link_libraries(sre INTERFACE android log)
endif()