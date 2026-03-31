project(stb)

add_library(stb vorbis.c)

target_include_directories(stb PUBLIC ./)

install(TARGETS stb)
# Might want to install the header too, just that it is not used by anything other than the engine
# install(FILES vorbis.h DESTINATION ${SRE_DIR}/common)