project(sreVec2Test)

set(SOURCES "init.cpp")

srEngine_build(sreVec2Test)

add_test(NAME sre_vec2 COMMAND sreVec2Test)