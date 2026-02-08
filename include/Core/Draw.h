#ifndef SRE_DRAW_H
#define SRE_DRAW_H
#include <C_API.h>
#include <ints.h>
#include <utils/math.h>

#include <Core/Texture.h>
#include <Datatypes/Units.h>
#ifdef __cplusplus
    #include <Datatypes/Vector.hpp>
    #include <Datatypes/Rect.hpp>
    #include <Datatypes/Color.hpp>

    using sre_col4 = sre::col4;
    using sre_vec2ut = sre::vec2ut;
    using sre_rect2Dut = sre::rect2Dut;
    using sre_rect2Di = sre::rect2Di;
#else
    typedef struct
    {
        sre_u8 r;
        sre_u8 g;
        sre_u8 b;
        sre_u8 a;
    } sre_col4;

    typedef struct
    {
        sre_unit x;
        sre_unit y;
    } sre_vec2ut; // TODO: Add C compatible vec2 templates (with macros)
    typedef union
    {
        struct
        {
            sre_unit x;
            sre_unit y;
            sre_unit w;
            sre_unit h;
        };
        struct
        {
            sre_vec2ut position;
            sre_vec2ut size;
        };
    } sre_rect2Dut;
    typedef SDL_Rect sre_rect2Di;
#endif

SRE_CAPI_BEGIN

extern const void *const SRE_DRAW_DONT_CENTER;

typedef enum
{
    SRE_DRAW_FILL,
    SRE_DRAW_LINE,
    SRE_DRAW_LINES,
    SRE_DRAW_RECTANGLE,
    SRE_DRAW_RRECTANGLE,
    SRE_DRAW_CIRCLE, // Unavailable
    SRE_DRAW_TEXTURE,
    SRE_DRAW_RTEXTURE
} sre_DrawType;

typedef enum
{
    SRE_DRAWFLAGS_USECAM = ut_bit(0), // Use the current camera coordinates to determine the final position of the object to draw
    SRE_DRAWFLAGS_STROKE = ut_bit(1), // Used by some types (Like DDRect), draw the contour of the object to draw instead of filling it
    SRE_DRAWFLAGS_FLIPX = ut_bit(2), // Used only by textures for now, to determine how to flip the content to render
    SRE_DRAWFLAGS_FLIPY = ut_bit(3)
} sre_DrawFlags;

/*
 * Structure representing the data to fill the screen, used with `sre_draw(SRE_DRAW_FILL)`
 * It's just a color struct, so you can still pass a color struct into `sre_draw`
 */
typedef struct sre_DDFill
{
    sre_col4 color;
} sre_DDFill;

/*
 * Structure representing the data to draw a line, used with `sre_draw(SRE_DRAW_LINE)`
 */
typedef struct sre_DDLine
{
    sre_s32 flags;
    sre_col4 color;
    //sre_unit thickness; // The thickness of the line, it is currently unavailable for future use

    sre_vec2ut pt1;
    sre_vec2ut pt2;
} sre_DDLine;

/*
 * Structure representing the data to draw multiple lines, used with `sre_draw(SRE_DRAW_LINES, &x)` 
 */
typedef struct sre_DDLines
{
    sre_s32 flags;
    sre_col4 color;

    int count;
    int _unused;
    const sre_vec2ut *pts;
} sre_DDLines;

typedef struct sre_DDRect
{
    sre_s32 flags;
    sre_col4 color;

    sre_rect2Dut rect;
    sre_vec2ut anchor;
} sre_DDRect;

typedef struct sre_DDRRect
{
    sre_DDRect rect;
    double angle;
} sre_DDRRect;

typedef struct sre_DDTexture
{
    sre_s32 flags;
    sre_col4 modulate;
    
    sre_rect2Dut rect;
    sre_vec2ut anchor;

    sre_Texture texture; // Texture identifier to draw
    sre_rect2Di region; // Region of the texture to render. Set the size to 0 to render the entire texture
} sre_DDTexture;

typedef struct sre_DDRTexture
{
    sre_DDTexture texture;
    double angle;
} sre_DDRTexture;

/*
 * Sends a drawing command to the current graphics driver
 *
 * Or in Layman's terms, it draws something to the screen
 *
 * The parameter `type` controls what to draw and `data` how to draw it
 * @param type The type of the command, one of the `SRE_DRAW_XX` enums
 * @param data A pointer to the data to draw, one of the `sre_DDXx` structs, depending on `type`
 * 
 * Passing something else or the wrong struct to the wrong `type` is undefined behavior 
 * 
 * @return 0 on success, -1 on error
 */
int sre_draw(sre_DrawType type, const void* data);

/* (Experimental and unimplemented)
 * Sets up and begins the drawing clip rect
 * @param rect The rectangle to clip in
 * 
 * @return 0 on success, -1 on error
 */
int sre_draw_clipbegin(sre_rect2Dut rect);

/*
 * Ends the drawing clip rect
 * Can be called if there's no drawing clip rect
 */
void sre_draw_clipend();

SRE_CAPI_END

#endif