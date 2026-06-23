SRE_DRIVERBUILD(sre_gl11 "gl11main.c" "gl11common.c" "gl11drawstate.c")
target_link_libraries(sre_gl11 PRIVATE sre_opengl)