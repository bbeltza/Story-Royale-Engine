#ifndef SRE_DRAW_H
#define SRE_DRAW_H
#include <C_API.h>
#include <ints.h>

#include <datatypes/units.h>
#include <utils/math.h>

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
    SRE_DRAWFLAGS_STROKE = ut_bit(1) // Used by some types (Like DDRect), draw the contour of the object to draw instead of filling it
} sre_DrawFlags;

/*
 * Structure representing the data to fill the screen, used with `sre_draw(SRE_DRAW_FILL)`
 * It's just a color struct, so you can still pass a color struct into `sre_draw`
 */
typedef struct
{
    sre_u8 color[4];
} sre_DDFill;

/*
 * Structure representing the data to draw a line, used with `sre_draw(SRE_DRAW_LINE)`
 */
typedef struct
{
    sre_u32 flags;
    sre_u8 color[4];
    //sre_unit thickness; // The thickness of the line, it is currently unavailable for future use

    sre_unit pt1_x;
    sre_unit pt1_y;

    sre_unit pt2_x;
    sre_unit pt2_y;
} sre_DDLine;

/*
 * Structure representing the data to draw multiple lines, used with `sre_draw(SRE_DRAW_LINES, &x)` 
 */
typedef struct
{
    sre_u32 flags;
    sre_u8 color[4];

    int count;
    int _unused;
    sre_unit (*pts)[2];
} sre_DDLines;

typedef struct 
{
    sre_u32 flags;
    sre_u8 color[4];

    sre_unit pos_x;
    sre_unit pos_y;
    sre_unit size_x;
    sre_unit size_y;

    sre_unit anchor_x;
    sre_unit anchor_y;
} sre_DDRect;

typedef struct
{
    sre_DDRect rect;
    float angle;
} sre_DDRRect;

typedef struct
{
    sre_u32 flags;
    sre_s32 texture;
    
    sre_unit pos_x;
    sre_unit pos_y;
    sre_unit size_x;
    sre_unit size_y;
    
    sre_unit anchor_x;
    sre_unit anchor_y;

    sre_u8 modulate[4];
} sre_DDTexture;

typedef struct
{
    sre_DDTexture texture;
    float angle;
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
 */
int sre_draw(sre_DrawType type, const void* data);

SRE_CAPI_END

#endif