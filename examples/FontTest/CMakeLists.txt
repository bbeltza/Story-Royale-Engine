project(sretestfont)

set(SOURCES FontTest.cpp)

SRE_PROPERTY(sretestfont PROPERTIES RESOURCES "res")
SRE_BUILD(sretestfont SOURCES)