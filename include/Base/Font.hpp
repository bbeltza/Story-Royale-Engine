#pragma once
#include <SDL_ttf.h>
#include <standard>

#include <Base/Sampler.hpp>
#include <Datatypes/Rect.hpp>
#include <Datatypes/Color.hpp>

#include <Base/Alignment.hpp>

namespace sre
{
    class File;

    class Font
    {
    public:
        Font(const sre::File& from_file, int pt = 12);
        Font(const char *path, int pt = 12);
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
}