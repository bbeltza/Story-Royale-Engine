project(StoryRoyaleEngineOS)

if (UNIX)
    add_subdirectory("unix")
elseif(WIN32)
    add_subdirectory("win")
    target_link_libraries(StoryRoyaleEngineOS "ntdll.lib")
endif()

target_link_libraries(StoryRoyaleEngine PUBLIC StoryRoyaleEngineOS)