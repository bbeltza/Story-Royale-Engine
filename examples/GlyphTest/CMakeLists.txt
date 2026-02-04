project("GlyphTest")

set(SOURCES GlyphTest.cpp)

srEngine_property(GlyphTest PROPERTIES RESOURCES "res")
srEngine_build(GlyphTest glyph SOURCES)