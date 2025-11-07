project(StoryRoyaleEngineOS)

add_library(StoryRoyaleEngineOS "win/output.c" "win/os.c")

target_link_libraries(StoryRoyaleEngine PUBLIC StoryRoyaleEngineOS)