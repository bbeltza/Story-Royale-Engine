include(sre)
project(ImageTest)

set(SOURCES ImageTest.cpp)

srEngine_property(ImageTest PROPERTIES RESOURCES "res")
srEngine_build(ImageTest images SOURCES)