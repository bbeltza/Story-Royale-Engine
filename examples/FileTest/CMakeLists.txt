project(sretestfile)

set(SOURCES FileTest.cpp)

SRE_PROPERTY(sretestfile PROPERTIES RESOURCES "assets")
SRE_BUILD(sretestfile SOURCES)