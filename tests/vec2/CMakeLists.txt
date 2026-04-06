set(SOURCES "init.cpp")

SRE_BUILD(srevec2test SOURCES)

add_test(NAME sre_vec2 COMMAND vec2test)