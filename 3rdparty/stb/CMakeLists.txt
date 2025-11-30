project(stb)

add_library(stb vorbis.c)

target_include_directories(stb PUBLIC ./)