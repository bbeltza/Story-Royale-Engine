include(sre)
project(RectsTest)

set(SOURCES Rects.cpp)

srEngine_property(RectsTest PROPERTIES TITLE     "Rectangles Test"
                                       RESOURCES "res")
srEngine_build(RectsTest rects SOURCES)