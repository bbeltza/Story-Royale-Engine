include(sre)
project(FileTest)

set(SOURCES FileTest.cpp)

srEngine_property(FileTest PROPERTIES RESOURCES "assets")
srEngine_build(FileTest file SOURCES)