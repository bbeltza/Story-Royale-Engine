project(sre_driverinterface)
add_library(sre_driverinterface INTERFACE)
target_link_libraries(sre_driverinterface INTERFACE sre)
target_link_libraries(sre_driverinterface INTERFACE sre_interface)
target_include_directories(sre_driverinterface INTERFACE ".")

function(SRE_DRIVERBUILD TARGET)
    project(${TARGET})
    add_library(${TARGET} STATIC ${ARGN})
    target_link_libraries(${TARGET} PRIVATE sre_driverinterface)
    target_link_libraries(sre_driverinterface INTERFACE ${TARGET})
endfunction()


add_subdirectory(sdl_renderer)
add_subdirectory(opengl)