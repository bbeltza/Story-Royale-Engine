project(sretestrects)

set(SOURCES Rects.cpp)

SRE_PROPERTY(sretestrects PROPERTIES TITLE     "Rectangles Test"
                                       RESOURCES "res")
SRE_BUILD(sretestrects SOURCES)