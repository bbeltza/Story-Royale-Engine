# Get the target property's holder name
macro(SRE_PROPERTY_TARGET TARGET VAR)
    set(${VAR} ${TARGET}_PROPERTY_INTERFACE)
endmacro()

function(SRE_PROPERTY TARGET)
    SRE_PROPERTY_TARGET(${TARGET} INT)
    if (NOT TARGET ${INT})
        project(${INT})
        add_library(${INT} INTERFACE)
    endif()

    set_target_properties(${INT} ${ARGN})
endfunction()