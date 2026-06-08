project(sre_imguiglue)
add_library(sre_imguiglue INTERFACE)

find_package(OpenGL)
if (NOT OpenGL_FOUND)
    # TODO: One thing, write your own ImGui render implementation. Using the engine's render functions, this will give you way less overhead when trying to wrap
        # ImGui impl sources inside a render driver. (I may actually need add more functions to the render API, this is good so it has more functionality, enough to write stuff in it)
    message(WARNING "Package OpenGL hasn't been found, ImGui's OpenGL 2 implementation links OpenGL statically so it's required to use it, so sre_imguiglue won't have any effect")
    return()
endif()

if (MSVC)
    target_link_options(sre_imguiglue INTERFACE "/export:SRE_HINT_IMGUI_GLUE")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(sre_imguiglue INTERFACE "-Wl,-u,SRE_HINT_IMGUI_GLUE")    
endif()

target_link_libraries(sre_imguiglue INTERFACE sre_imguigluelib OpenGL::GL)