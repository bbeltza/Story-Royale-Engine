project(sretestimage)

set(SOURCES ImageTest.cpp)

SRE_PROPERTY(sretestimage PROPERTIES RESOURCES "res")
SRE_BUILD(sretestimage SOURCES)