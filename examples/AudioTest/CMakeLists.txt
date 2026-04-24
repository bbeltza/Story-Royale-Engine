project(sretestaudio)

set(SOURCES AudioTest.cpp)

SRE_PROPERTY(sretestaudio PROPERTIES RESOURCES "res")
SRE_BUILD(sretestaudio SOURCES NO_BIND)