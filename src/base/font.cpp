#include <Base/Font.hpp>
#include <Base/File.hpp>
#include <Base/Image.hpp>
#include <Base/Error.h>
#include <Base/Log.h>

#include <freetype/freetype.h>

static std::atomic<unsigned> s_ftreferences{0};
static FT_Library s_ftlibrary{};

static void ft_donelib()
{
    FT_Error fterr;

    s_ftreferences--;
    if (s_ftreferences.load() == 0)
    {
        FT_Library tmplib = s_ftlibrary;
        s_ftlibrary = NULL;
        fterr = FT_Done_FreeType(tmplib);
        assert(fterr == FT_Err_Ok);
    }
}

static void ft_streamclose(FT_Stream stream)
{
    ft_sfree(stream->base);
}

using namespace sre;

Font::FontData::~FontData()
{
    FT_Done_Face(ftface);
}

Font::Font(const Font& right): m_data(right.m_data)
{
    if (m_data)
    {
        m_data->ref++;
        s_ftreferences++;
    }
}

Font::Font(Font&& right): m_data(right.m_data)
{
    right.m_data = NULL;
}

Font::~Font()
{
    _close();
}

void Font::operator=(const Font& right)
{
    if (m_data == right.m_data)
        return;

    if (m_data)
    {
        m_data->dereference();
        
        if (!right.m_data)
        {
            ft_donelib();
            m_data = NULL;
            return;
        }
    }
    else if (right.m_data)
        s_ftreferences++;

    m_data = right.m_data;
    if (m_data)
        m_data->ref++;
}

void Font::operator=(Font&& right)
{
    FontData* tmp = right.m_data;
    right.m_data = NULL;

    if (m_data == tmp)
    {
        if (tmp)
        {
            s_ftreferences--;
            tmp->ref--;
            assert(tmp->ref.load() > 0);
        }
        return;
    }

    if (m_data)
    {
        m_data->dereference();

        if (!right.m_data)
        {
            ft_donelib();
            m_data = NULL;
            return;
        }
    }

    m_data = tmp;
}

void Font::_close()
{
    if (!m_data)
        return;

    FontData* tmp = m_data;
    m_data = NULL;

    tmp->dereference();
    ft_donelib();
}

void Font::_throwgetatlas() const { sre::critical(SRE_ERR_INVALID_STATE, "Font is invalid, attempt to call get_atlas on an invalid font!"); }

bool Font::load(const sre::byte* data, sre::size_t size)
{
    if (!size || !data)
        return sre::error(SRE_ERR_INVALID_PARAMETER, "Parameter size/data are invalid!") && false;

    FT_Error fterr;


    //
    s_ftreferences++;
    if (!s_ftlibrary)
    {
        assert(s_ftreferences.load() == 1);
        fterr = FT_Init_FreeType(&s_ftlibrary);
        if (fterr)
            sre::critical(SRE_ERR_FAIL, "FreeType library initialization failed!");
        
        assert(s_ftlibrary);
    }

    //

    
    //
    FT_Byte* facebytes = static_cast<FT_Byte*>(ft_smalloc(size));
    ft_memcpy(facebytes, data, size);
    
    FT_Open_Args createinfo{};
    createinfo.flags = FT_OPEN_MEMORY;
    createinfo.memory_base = facebytes;
    createinfo.memory_size = static_cast<FT_Long>(size);
    
    FT_Face ftface;
    fterr = FT_Open_Face(s_ftlibrary, &createinfo, 0, &ftface);
    if (fterr != FT_Err_Ok)
    {
        ft_donelib();
        ft_sfree(facebytes);
        sre::error(SRE_ERR_FAIL, "FT_Open_Face failed");
        return false;
    }

    ftface->stream->close = ft_streamclose;

    fterr = FT_Set_Pixel_Sizes(ftface, 0, 12);
    assert(fterr == FT_Err_Ok);
    
    const SDL_Color colors[3] = { { 0, 0, 0, 0 }, { 255, 255, 255, 255 } };

    int wsum = 0;
    int hmax = 0;
    sre::Image characters[FontAtlas::CHARACTER_COUNT];
    sre::Glyph glyphs[FontAtlas::CHARACTER_COUNT];
    for (int i = 0; i < FontAtlas::CHARACTER_COUNT; i++)
    {
        FT_UInt glyphindex = FT_Get_Char_Index(ftface, i + 32);
        fterr = FT_Load_Glyph(ftface, glyphindex, FT_LOAD_DEFAULT);
        fterr = FT_Render_Glyph(ftface->glyph, FT_RENDER_MODE_MONO);
        if (fterr != FT_Err_Ok)
        {
            ft_donelib();
            FT_Done_Face(ftface);
            sre::error(SRE_ERR_FAIL, "FT_Load_Char failed");
            return false;
        }

        SDL_Surface* sdlsurface = SDL_CreateRGBSurfaceWithFormat(0, (int)ftface->glyph->bitmap.width, (int)ftface->glyph->bitmap.rows, 0, SDL_PIXELFORMAT_INDEX1MSB);
        SDL_SetPaletteColors(sdlsurface->format->palette, colors, 0, 2);
        for (unsigned i = 0; i < ftface->glyph->bitmap.rows; i++)
            memcpy(static_cast<char*>(sdlsurface->pixels) + sdlsurface->pitch * i,
                   ftface->glyph->bitmap.buffer + ftface->glyph->bitmap.pitch * i,
                   ftface->glyph->bitmap.pitch);
        
        wsum += sdlsurface->w;
        hmax = ut_max(hmax, sdlsurface->h);

        characters[i] = sdlsurface;
        glyphs[i].advance = static_cast<sre::unit>(ftface->glyph->advance.x)/64;
        glyphs[i].bearing = sre::vec2ut{ ftface->glyph->bitmap_left, ftface->glyph->bitmap_top };
    }

    constexpr int BASE_MULTIPLE = 64;
    int dim = BASE_MULTIPLE;
    while ((wsum/dim) > (dim/hmax))
        dim += BASE_MULTIPLE;
    assert(hmax < dim);

    sre::Image atlas{dim};
    sre::vec2i pos{0};
    for (int i = 0; i < FontAtlas::CHARACTER_COUNT; i++)
    {
        auto &img = characters[i];
        sre::vec2i size = img.size();
        if (pos.x + size.x > dim)
        {
            pos.x = 0;
            pos.y += hmax;
        }

        atlas.blit(img, pos, 0);

        sre::rect2Di rect{ pos, { size.x, hmax } };
        glyphs[i].uvregion = sre::rect2Df{rect};
        glyphs[i].bearing.y = hmax - glyphs[i].bearing.y;

        pos.x += size.x;
    }

    sre::unit lineskip = hmax - ftface->descender/64.0_ut;
    auto fontdata = new FontData{ftface, atlas.to_texture(), glyphs, lineskip};
    m_data = fontdata;
    return true;
}