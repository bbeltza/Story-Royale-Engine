project(sre)
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
    sreECS # Scene and entity manager library
    sreECS_presets # Scene and entity presets (only has components and camera controllers...) (optional)
    sreGUI # GUI manager library
    sreGUI_presets # UI presets (optional)
)

if(NOT BUILD_SHARED_LIBS)
    set(_SDL_LIB_SUFFIX "-static")
    set(_ZLIB_SUFFIX "static")
endif()

set(SRE_3RDPARTYLIBS
    SDL2::SDL2${_SDL_LIB_SUFFIX}
    SDL2_ttf::SDL2_ttf${_SDL_LIB_SUFFIX}
    SDL2_image::SDL2_image${_SDL_LIB_SUFFIX}
    
    stb
    imgui
    #zlib${_ZLIB_SUFFIX}
)

project(srelib)
add_library(srelib INTERFACE)
target_link_libraries(srelib INTERFACE ${SRE_MODULES})

# Setup interface targets

target_include_directories(sre INTERFACE ${SRE_DIR}/common)
target_include_directories(sre INTERFACE ${SRE_DIR}/include)
target_link_libraries(sre INTERFACE ${SRE_3RDPARTYLIBS})