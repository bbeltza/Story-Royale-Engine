#include "gl11.h"
#include <math.h>
#include <assert.h>

// common drawing

void sregl11_present(void* _inst)
{
    sregl11_inst* inst = _inst;
    SREGL_PRESENT(inst->common);
}

bool sregl11_clear(void* _inst, float color[3])
{
    sregl11_inst* inst = _inst;
    SREGL_CLEAR(inst->glfuncs, color);
    return true;
}

// common set_ functions

bool sregl11_set_blendstate(void* _inst, sre_blendMode blendmode)
{
    sregl11_inst* inst = _inst;
    return sregl_set_blendstate(&inst->glfuncs, blendmode);
}

void sregl11_set_clipstate(void* _inst, const sre_rect2Di* rectangle)
{
    sregl11_inst* inst = _inst;
    sregl_set_clipstate(&inst->glfuncs, inst->common.window, rectangle);
}

void sregl11_set_vsync(void* _inst, bool enable)
{
    (void)_inst;
    sregl_set_vsync(enable);
}

// all of the texture functions

// From SDL_utils.c (SDL_powerof2)
// This trick comes down by trying to fill said number's bits lower than the most significant one, to later increment it, getting a number with a single bit in it aka a power of two:
    // You first subtract the number by 1, this alters many of the lower bits, and in case it is a power of two, it disables the most significant bit off, and gives already the target number two increment back (which is already the input number)
    // Then you can see that you do multiple bitmask ORs to the number, by the number bitshifted by a power of two incremented each time.
    // ORing it by the number bitshifted by every number is unecessary to my point of view, it's already hard to explain, but that is because the number's result is accumulated so you're ORing the number that has been ORed before.
    // So we have now a number that ressembles like this: 0000001111111111 (this obviously depends on the numbers, and is a shorter representation)
    // So when incrementing it by 1, you'd get something like this: 0000010000000000 ; This number is 1024, and it's a power of two! Now you'd just return that number
static int padbypowerof2(int x)
{
    assert(x > 0);
    
    int value = x;
    value -= 1;
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    value += 1;

    return value;
}

bool sregl11_texture_setup(void* _inst, void* _texture, sre_pixelFormat format, int w, int h, sre_pixelFormat* outformat)
{
    sregl11_inst* inst = _inst;
    sregl11_texture* texture = _texture;

    int rw = padbypowerof2(w);
    int rh = padbypowerof2(h);
    texture->xrange = (float)w / rw;
    texture->yrange = (float)h / rh;
    assert(texture->xrange <= 1.0f);
    assert(texture->yrange <= 1.0f);

    return sregl_texture_setup(&inst->glfuncs, &texture->texture, format, rw, rh, outformat);
}

bool sregl11_texture_update(void* _inst, void* texture, const sre_rect2Di* region, const void* pixels, int pitch)
{
    sregl11_inst* inst = _inst;
    return sregl_texture_update(&inst->glfuncs, texture, region, pixels, pitch);
}

void sregl11_texture_destroy(void* _inst, void* texture)
{
    sregl11_inst* inst = _inst;
    sregl_texture_destroy(&inst->glfuncs, texture);
}