#ifndef SRE_RENDER_H
#define SRE_RENDER_H
#include <C_API.h>
#include <ints.h>

#if _WIN32
    #define SRE_RENDERCALL __cdecl
#else
    #define SRE_RENDERCALL
#endif

#ifndef __cplusplus
    #include <Datatypes/CRect.h>
    #include <Datatypes/CColor.h>
    #include <Datatypes/Units.h>

    SRE_RECT2DMAKE(sre_unit, ut);
    SRE_RECT2DMAKE(int, i);
    SRE_VEC2MAKE(float, f);
#else
    #include <Datatypes/Rect.hpp>
    #include <Datatypes/Color.hpp>
    #include <Datatypes/Flags.hpp>
    
    typedef sre::rect2Dut sre_rect2Dut;
    typedef sre::rect2Di sre_rect2Di;
    typedef sre::vec2ut sre_vec2ut;
    typedef sre::vec2f sre_vec2f;
    typedef sre::col4 sre_col4;
#endif

#include <Base/Pixel.h>

#ifndef __cplusplus
    typedef struct sre_Sampler sre_Sampler;
#else
    struct sre_Sampler
    {
        sre::vec2i size() const { return { _w, _h }; }
        sre::pixelFormat format() const { return _format; }

        inline bool update(const void* pixels, int pitch);
        inline int aquire();
        inline int release();

        private:
            int _refcount;
            int _w, _h;
            sre_pixelFormat _format;
    };
#endif

typedef enum sre_blendMode
{
    SRE_BLEND_NONE,
    SRE_BLEND_BLEND,
    SRE_BLEND_ADD,
    SRE_BLEND_MOD,
    SRE_BLEND_MUL,

    SRE_BLEND_DEFAULT = SRE_BLEND_BLEND
} sre_blendMode;

typedef enum sre_draw2mode // Primitive rendering mode for draw2
{
    SRE_DRAW2_JOINED,
    SRE_DRAW2_STRIP,
    SRE_DRAW2_TRIANGLE
} sre_draw2mode;

enum sre_drawFlags
{
    SRE_DRAWFLAG_CAMERA = 1 << 0,
    SRE_DRAWFLAG_LINE = 1 << 1 // Draw lines instead of filling the elements, it is not completely implemented right now
};

enum _sre_drawSwitchFlags
{
    SRE_RENDER_SWITCHTYPE = 1 << 0, // Switch from draw1 to draw2, or vice-versa. The render driver can keep the drawing state if two render flushes are from the same draw function, which increases performance
    SRE_RENDER_SWITCHCAMERA = 1 << 1,
    SRE_RENDER_SWITCHTEXTURE = 1 << 2
};

struct SDL_Window;

struct sre_SamplerNew
{
    int _refcount;
    int w, h;
    sre_pixelFormat format;
    void* renderer;
};

typedef struct sre_RenderPoint
{
    sre_vec2ut pos;
    sre_vec2f uv;
} sre_RenderPoint;

typedef struct sre_RenderInstance1
{
    sre_rect2Dut rectangle;
    sre_vec2ut anchor;
    sre_col4 color;
    float angle;

    sre_vec2f uv;
    sre_vec2f uv_offset;
} sre_RenderInstance1;

typedef struct sre_RenderInstance2
{
    sre_draw2mode mode;
    sre_col4 color;
    sre_RenderPoint points[
        #ifdef __cplusplus
            1
        #endif
    ];
} sre_RenderInstance2;

struct sre_RenderVFT
{
    void (* destructor)(void*);

    void (* flush_queueinstances1)(void*, void* texture, const sre_RenderInstance1* instances, size_t instance_count, sre_u32 flags, sre_u32 switch_flags);
    void (* flush_queueinstances2)(void*, void* texture, const sre_RenderInstance2* instance, size_t point_count, sre_u32 flags, sre_u32 switch_flags);

    void (* present)(void*);
    bool (* clear)(void*, float color[3]);
    
    bool (* set_viewportstate)(void*, int w, int h, sre_unit scale);
    bool (* set_blendstate)(void*, sre_blendMode blending);
    bool (* set_camerastate)(void*, sre_unit x, sre_unit y);
    void (* set_clipstate)(void*, const sre_rect2Di* rectangle);
    void (* set_vsync)(void*, bool enable);

    bool (* texture_setup)(void*, void* texture, sre_pixelFormat formathint, int w, int h, sre_pixelFormat* outformat);
    bool (* texture_update)(void*, void* texture, const void* pixels, int pitch);
    void (* texture_destroy)(void*, void* texture);
};


// Structure used to define driver initialization data
typedef struct sre_RenderDriverData
{
    bool (*initialize)(const struct sre_RenderVFT** interface, void* renderdata, struct SDL_Window* window);
    size_t renderer_size; // The size, in bytes of the renderer structure
    size_t texture_size; // The size, in bytes of a texture
} sre_RenderDriverData;

SRE_CAPI_BEGIN

// Sampler API

sre_Sampler* sre_sampler(sre_pixelFormat format, int x, int y);
bool sre_sampler_update(sre_Sampler* sampler, const void* pixels, int pitch);
bool sre_sampler_query(sre_Sampler* sampler, int size[2], sre_pixelFormat* format);

int sre_sampler_aquire(sre_Sampler* sampler);
int sre_sampler_release(sre_Sampler* sampler);

// C Render function wrappers

void sre_render_clipreset();
void sre_render_clipset(const sre_rect2Dut* zone);

void sre_render_blend(sre_blendMode blend);

void sre_render_draw1(sre_u32 flags, const sre_RenderInstance1 instances[], size_t instcount, sre_Sampler* texture);
void sre_render_draw2(sre_u32 flags, const sre_col4* color, const sre_RenderPoint points[], size_t pcount, sre_draw2mode mode, sre_Sampler* texture);

void sre_render_fill(const sre_col4* color);

//
SRE_CAPI_END

#ifdef __cplusplus
    #include <vector>

    inline bool sre_Sampler::update(const void* pixels, int pitch) { return sre_sampler_update(this, pixels, pitch); }
    inline int sre_Sampler::aquire() { return sre_sampler_aquire(this); }
    inline int sre_Sampler::release() { return sre_sampler_release(this); }

    namespace sre
    {
        using Sampler = sre_Sampler;
        using blendMode = sre_blendMode;
        using draw2mode = sre_draw2mode;

        using RenderInstance1 = sre_RenderInstance1; // Render instance for all rectangle draw calls
        using RenderInstance2 = sre_RenderInstance2; // Render instance for all line & complex geometry draw calls
        using RenderPoint = sre_RenderPoint;

        void render_clipreset();
        void render_clipset(sre::rect2Dut zone);

        void render_blend(sre::blendMode blend);
        inline void render_blend() { render_blend(SRE_BLEND_DEFAULT); }

        void render_draw1(sre::flags32 flags, const RenderInstance1 instances[], size_t instcount, Sampler* texture=NULL);
        void render_draw2(sre::flags32 flags, sre::col4 color, const RenderPoint points[], size_t pcount, sre::draw2mode mode=SRE_DRAW2_JOINED, Sampler* texture=NULL);

        template <size_t n>
        void render_draw1(sre::flags32 flags, const RenderInstance1 (&instances)[n], Sampler* texture=NULL) { render_draw1(flags, instances, n, texture); }
        
        template <size_t n>
        void render_draw2(sre::flags32 flags, sre::col4 color, const RenderPoint (&points)[n], sre::draw2mode mode=SRE_DRAW2_JOINED, Sampler* texture=NULL) { return render_draw2(flags, color, points, n, mode, texture); }

        template <size_t n>
        void render_draw2(sre::flags32 flags, sre::col4 color, const sre::vec2ut (&positions)[n], sre::draw2mode mode=SRE_DRAW2_JOINED)
        {
            RenderPoint points[n];
            for (auto i = 0; i < n; i++)
                points[i].pos = positions[i];

            return render_draw2(flags, color, points, n, mode);
        }

        inline void render_fill(sre::col4 color) { render_draw1(0, {{ {0, 65536}, 0, color }}); }

        inline Sampler* sampler(pixelFormat format, int w, int h) { return sre_sampler(format, w, h); }

        template <typename R, typename T=typename R::texture_type>
        struct RenderDriverHelper: sre_RenderDriverData
        {
            RenderDriverHelper(): sre_RenderDriverData{
                [](const sre_RenderVFT** interface, void* inst, SDL_Window* window)
                {
                    static const sre_RenderVFT this_interface = {
                        [](void* inst) { static_cast<R*>(inst)->~R(); },
                        [](void* inst, void* texture, const sre_RenderInstance1* instances, size_t instance_count, sre_u32 flags, sre_u32 switch_flags) {
                            static_cast<R*>(inst)->flush_queueinstances1(static_cast<T*>(texture), instances, instance_count, flags, switch_flags);
                        },
                        [](void* inst, void* texture, const sre_RenderInstance2* instance, size_t point_count, sre_u32 flags, sre_u32 switch_flags) {
                            static_cast<R*>(inst)->flush_queueinstances2(static_cast<T*>(texture), instance, point_count, flags, switch_flags);
                        },
                        [](void* inst) { static_cast<R*>(inst)->present(); },
                        [](void* inst, float color[3]) { return static_cast<R*>(inst)->clear(color); },

                        [](void* inst, int w, int h, sre_unit scale) { return static_cast<R*>(inst)->set_viewportstate(w, h, scale); },
                        [](void* inst, sre_blendMode blending) { return static_cast<R*>(inst)->set_blendstate(blending); },
                        [](void* inst, sre_unit x, sre_unit y) { return static_cast<R*>(inst)->set_camerastate(x, y); },
                        [](void* inst, const sre_rect2Di* rectangle) { static_cast<R*>(inst)->set_clipstate(rectangle); },
                        [](void* inst, bool enable) { static_cast<R*>(inst)->set_vsync(enable); },

                        [](void* inst, void* texture, sre_pixelFormat format, int w, int h, sre_pixelFormat* outformat) { return static_cast<R*>(inst)->texture_setup(static_cast<T*>(texture), format, w, h, outformat); },
                        [](void* inst, void* texture, const void* pixels, int pitch) { return static_cast<R*>(inst)->texture_update(static_cast<T*>(texture), pixels, pitch); },
                        [](void* inst, void* texture) { static_cast<R*>(inst)->texture_destroy(static_cast<T*>(texture)); }
                    };

                    new(inst) R(window);
                    if (!static_cast<R*>(inst)->succeeded()) return false;

                    *interface = &this_interface;
                    return true;
                },
                sizeof(R),
                sizeof(T)
            } {}
        };
    }
#endif

#endif