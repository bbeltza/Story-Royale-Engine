project(sre_drivers)
add_library(sre_drivers INTERFACE)

function(SRE_DRIVERBUILD TARGET)
    project(${TARGET})
    add_library(${TARGET} OBJECT ${ARGN})

    target_include_directories(${TARGET} PRIVATE "../")
    target_link_libraries(${TARGET} PRIVATE sre)
    target_link_libraries(${TARGET} PRIVATE sre_private)
    target_link_libraries(sre_drivers INTERFACE ${TARGET} $<TARGET_OBJECTS:${TARGET}>) # Be careful with includes!

    install_wrapper(TARGETS ${TARGET})
endfunction()