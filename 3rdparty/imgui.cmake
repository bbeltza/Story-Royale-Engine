project(imgui)

add_library(
    imgui
    "imgui/imgui.cpp"
    "imgui/imgui_tables.cpp"
    "imgui/imgui_demo.cpp"
    "imgui/imgui_draw.cpp"
    "imgui/imgui_widgets.cpp"
)

set(IMGUI_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/imgui" CACHE STRING "The ImGUI directory")
target_include_directories(imgui PUBLIC ${IMGUI_DIRECTORY})

install(TARGETS imgui)
install(FILES ${IMGUI_DIRECTORY}/imgui.h
              ${IMGUI_DIRECTORY}/imconfig.h
              DESTINATION ${SRE_DIR}/common)