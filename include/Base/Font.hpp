#pragma once
#include <standard>
#include <atomic>

#include <Base/Sampler.hpp>
#include <Datatypes/Rect.hpp>
#include <Datatypes/Color.hpp>

#include <Base/Alignment.hpp>

struct FT_FaceRec_;

namespace sre
{
    class File;

    class UnicodeSet;

    using fontRenderCallback = void (*)(void* pointer, sre::RenderInstance1* renderdata, int character, unsigned index);
    struct FontRenderData
    {
        constexpr FontRenderData(const char* text, int textlen=-1, sre::flags32 renderflags=0, sre::col4 color=sre::WHITE, fontRenderCallback callback=NULL):
            text(text), textlen(textlen), renderflags(renderflags), color(color), modifier_callback(callback) {}

        const char* text;
        int textlen;
        sre::flags32 renderflags;
        sre::col4 color;

        fontRenderCallback modifier_callback;
    };

    struct FontRenderTextData
    {
        constexpr FontRenderTextData(sre::rect2Dut area, sre::alignment h=sre::ALIGN_LEFT, sre::alignment v=sre::ALIGN_TOP, int count=-1):
            area(area), h_alignment(h), v_alignment(v), char_count(count) {}

        sre::rect2Dut area;
        sre::alignment h_alignment;
        sre::alignment v_alignment;
        int char_count;
    };

    struct Glyph
    {
        sre::rect2Df uvregion;
        sre::vec2ut bearing;
        sre::unit advance;
    };

    class FontAtlas
    {
    public:
        static constexpr int CHARACTER_COUNT = 95;

        // Load font atlas from a single dimension, where it would assume that every character in the atlas has the exact same size
        FontAtlas(const sre::RSampler& texture, sre::vec2i contant_size);
        // Load font from a set of rectangle regions, where `region[chr-33]` corresponds to the region in the texture where character `chr` is
        FontAtlas(const sre::RSampler& texture, const sre::Glyph glyphs[CHARACTER_COUNT], sre::unit lineskip, bool normalize=true);
        
        sre::RSampler texture() const { return m_atlas; }

        void render_text(const FontRenderData& renderdata, const FontRenderTextData& textdata, const UnicodeSet* unicodeset=NULL, void* userdata=NULL);
        void render_line(const FontRenderData& renderdata, sre::vec2ut topleft_begin, const UnicodeSet* unicodeset=NULL, void* userdata=NULL);
    private:
        sre::RSampler m_atlas;
        sre::Glyph m_glyphset[CHARACTER_COUNT];
        sre::unit m_lineskip;
    };

    class Font
    {
        struct FontData
        {
            FontData(FT_FaceRec_* face, const sre::RSampler& texture, const sre::Glyph regions[FontAtlas::CHARACTER_COUNT], sre::unit lineskip): ftface(face), atlas(texture, regions, lineskip) {}
            ~FontData();

            void dereference() { 
                ref--;
                if (ref.load() == 0)
                    delete this;
                else
                    assert(ref.load() > 0);
            }

            std::atomic<int> ref{1};

            FT_FaceRec_* ftface{};
            FontAtlas atlas;
        }* m_data = NULL;
    public:
        constexpr Font() = default;
        Font(const Font& right);
        Font(Font&& right);
        ~Font();

        void operator =(const Font& right);
        void operator =(Font&& right);

        bool load(const sre::byte* data, sre::size_t size);
        bool valid() const { return m_data != NULL; } 

        FontAtlas& get_atlas() const
        {
            if (!m_data)
                _throwgetatlas();
            
            return m_data->atlas;
        } 
    private:
        void _throwgetatlas() const;
        void _close();
    };

    #if 0
    class _Font
    {
    public:
        _Font(const sre::File& from_file, int pt = 12);
        _Font(const char *path, int pt = 12);
        ~Font();

        void render(const sre::rect2Dut &bounds, const sre::col4 &color, const char *text, int count, Alignment halignment = A_LEFT, Alignment valignment = A_TOP);
        void render_line(const sre::vec2ut &start, const sre::col4 &color, const char *text, int count, int acc = 0);

        bool valid() const { return m_font != NULL; }

        // Converts `src` utf-8 string into null terminated utf-8 codepoint into `dst`
        // And returns the size of the codepoint
        static int str_to_utf8chr(const char* src, char* dst);
    private:
        TTF_Font *m_font = NULL;
        SDL_RWops *m_rwops = NULL;

        std::vector<std::string> linebuf;

        RSampler ascii_atlas;
        std::array<sre::rect2Df, 127> ascii_uvs; // Fast access ascii uv table
        std::unordered_map<int, RSampler> unicode;
    };
    #endif
}