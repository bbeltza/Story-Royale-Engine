# Install wrapper that checks for SRE_INSTALL and does nothing if it's not set

if (SRE_INSTALL)
    macro(install_wrapper)
        install(${ARGN})
    endmacro()
else()
    macro(install_wrapper)

    endmacro()
endif()