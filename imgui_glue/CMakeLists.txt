project(sre_imguiglue)
project(sre_imgui)

add_library(sre_imgui STATIC "imgui_srcs.cpp")
target_include_directories(sre_imgui PRIVATE ${SRE_DIR}/3rdparty/imgui)
target_include_directories(sre_imgui PRIVATE ${SRE_DIR}/3rdparty/SDL/include)

add_library(sre_imguiglue STATIC "imgui_glue.cpp")
target_link_libraries(sre_imguiglue PRIVATE sre)
target_link_libraries(sre_imguiglue PRIVATE sre_private)
target_include_directories(sre_imguiglue PRIVATE ${SRE_DIR}/3rdparty/imgui)

target_link_libraries(sre INTERFACE sre_imgui)

if (MSVC)
    target_link_options(sre_imguiglue PUBLIC "/export:SRE_HINT_IMGUI_GLUE")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(sre_imguiglue PUBLIC "-Wl,-u,SRE_HINT_IMGUI_GLUE")    
endif()