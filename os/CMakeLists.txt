project(sre_os)

if (UNIX)
    add_subdirectory("unix")
elseif(WIN32)
    add_subdirectory("win")
endif()

target_link_libraries(sre_os PUBLIC sre)