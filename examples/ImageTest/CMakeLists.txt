include(sre)
project(ImageTest)

set(SOURCES ImageTest.cpp)

srEngine_property(ImageTest PROPERTIES RESOURCES "res")
srEngine_build(ImageTest images SOURCES)

target_compile_definitions(sre PUBLIC IMGUI_DISABLE=1)