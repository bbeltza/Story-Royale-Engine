#ifndef SRE_RENDER_H
#define SRE_RENDER_H
#include <C_API.h>
#include <ints.h>

#include <Datatypes/Color.h>
#include <Datatypes/Rect.h>

SRE_RECT2DMAKESFFX(sre_unit, ut);
SRE_RECT2DMAKESFFX(int, i);
SRE_VEC2MAKESFFX(float, f);
#ifdef __cplusplus
    #include <Datatypes/Flags.hpp>
    
    typedef sre::vec2ut sre_vec2ut;
    typedef sre::vec2f sre_vec2f;
#endif

#include <Base/Pixel.h>

#ifndef __cplusplus
    typedef struct sre_Texture sre_Texture;
    typedef struct sre_RenderTarget sre_RenderTarget;
#else
    struct sre_Texture
    {
        sre::vec2i size() const { return { _w, _h }; }
        sre::SDLpixelFormat format() const { return _format; }

        inline bool update(const void* pixels, int pitch);
        inline bool update(const sre::rect2Di* region, const void* pixels, int pitch);
        inline int aquire();
        inline int release();

        private:
            sre_Texture() = delete; // Textures are returned using sre::texture_create

            int _refcount;
            int _w, _h;
            sre_SDLpixelFormat _format;
    };
#endif

#define SRE_RENDERSTATUS_SUCCEEDED 0
#define SRE_RENDERSTATUS_UNSUPPORTED 1
#define SRE_RENDERSTATUS_FAILED -1

typedef enum sre_renderDriver
{
    SRE_RENDERDRIVER_SOFTWARE,
    SRE_RENDERDRIVER_SDLRENDERER,
	SRE_RENDERDRIVER_OPENGL_11,
	SRE_RENDERDRIVER_OPENGL_21,
	SRE_RENDERDRIVER_OPENGL_32,

		SRE_RENDERDRIVER_DIRECTX_9,
		SRE_RENDERDRIVER_DIRECTX_11,
		SRE_RENDERDRIVER_DIRECTX_12
} sre_renderDriver;

typedef enum sre_blendMode
{
    SRE_BLEND_BLEND,
    SRE_BLEND_ADD,
    SRE_BLEND_MOD,
    SRE_BLEND_MUL,

    SRE_BLEND_DEFAULT = SRE_BLEND_BLEND
} sre_blendMode;

typedef enum sre_draw2primitive // Primitive topology mode for draw2
{
    SRE_PRIMITIVE_TRIANGLES,
    SRE_PRIMITIVE_TRIANGLESTRIP,
    SRE_PRIMITIVE_LINEPERLINE,
    SRE_PRIMITIVE_LINESTRIP,
    SRE_PRIMITIVE_LINELOOP,
    SRE_PRIMITIVE_POINTS
} sre_draw2primitive;

enum sre_drawFlags
{
    // Individual camera axis flags. Not implemented anywhere yet
    SRE_DRAWFLAG_CAMERAX = 1 << 0,
    SRE_DRAWFLAG_CAMERAY = 1 << 1,

    SRE_DRAWFLAG_CAMERA = SRE_DRAWFLAG_CAMERAX | SRE_DRAWFLAG_CAMERAY
};

enum _sre_drawSwitchFlags
{
    SRE_RENDER_SWITCHTYPE = 1 << 0, // Switch from draw1 to draw2, or vice-versa. The render driver can keep the drawing state if two render flushes are from the same draw function, which increases performance
    SRE_RENDER_SWITCHCAMERA = 1 << 1,
    SRE_RENDER_SWITCHTEXTURE = 1 << 2
};

struct SDL_Window;

typedef struct sre_RenderPoint
{
    sre_vec2ut pos;
    sre_vec2f uv;
    sre_col4 color;
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

struct sre_RenderVFT
{
    void (* destructor)(void* _inst);

    void (* draw1)(void* _inst, const sre_RenderInstance1* instances, size_t instance_count);
    void (* draw2)(void* _inst, const sre_RenderPoint* pts, size_t point_count, sre_draw2primitive mode);

    void (* begin)(void* _inst, const float clear[4]); // Formerly as `clear`
    void (* end)(void* _inst); // Formerly as `present`
    
    void (* set_viewportstate)(void* _inst, int w, int h, sre_unit scale);
    void (* set_vsync)(void* _inst, bool enable);
    void (* set_texturestate)(void* _inst, void* _texture);
    void (* set_blendstate)(void* _inst, sre_blendMode blendmode);
    void (* set_camerastate)(void* _inst, sre_unit x, sre_unit y);
    void (* set_scissorstate)(void* _inst, const sre_rect2Di* rectangle);

    bool (* texture_setup)(void* _inst, void* _texture, sre_SDLpixelFormat formathint, int w, int h, sre_SDLpixelFormat* outformat);
    bool (* texture_update)(void* _inst, void* _texture, const sre_rect2Di* region, const void* pixels, int pitch);
    void (* texture_destroy)(void* _inst, void* _texture);
};

enum sreRenderDriverBits
{
    SRE_RENDERBIT_SUPPORT_LINELOOP = 1 << 0, // Support for handling SRE_PRIMITIVE_LINELOOP for draw2 directly. If this bit is not set, then the engine will handle it directly
                                                // by adding an extra point equal to the first one in `points`, and handing it to the driver with the SRE_PRIMITIVE_LINESTRIP mode.
    // SRE_RENDERBIT_MULTITHREADED = 1 << 1, // Render driver functions can be run in other threads. Otherwise, every render function such as texture actions will be deferred.
    // SRE_RENDERBIT_SUPPORT_RENDERTARGETS = 1 << 2
    // SRE_RENDERBIT_HANDLE_NULLTEXTURE = 1 << 3 // Whether to handle draw commands in which `texture` is NULL, meaning there are no texture bound you want to draw a flat surface.
                                                    // If this flag is not set, then you won't get a NULL as `texture`, and the engine will create a flat texture and pass it instead.
                                                    // This flag is commented for now.
};

// Structure used to define driver initialization data
//
// You shouldn't bother using this structure in your game at all.
// But if you want to make your own render driver, then you can use this structure
// by declaring a global/static variable/constant of this type, and pointing to it with the future SRE_HINT_EXTERN_RENDERDRIVER hint.
// If you're on C++, you may also want to look at the sre::RenderDriverHelper template, it inherits this structure and lets you implement your render driver
//  with a custom class as a more "clean" way
typedef struct sre_RenderDriverData
{
    // Initialization function, should write to `interface` and return either of these values:
    //
        // `SRE_RENDERSTATUS_SUCCEEDED` (0): Initialization has succeeded, the engine can properly use the render driver
        // `SRE_RENDERSTATUS_FAILED` (-1): Initialization has failed, an error has occurred and has to be looked into
        // `SRE_RENDERSTATUS_UNSUPPORTED` (1): The render driver was found unsupported by the system. The engine needs to switch to another driver that can be supported
    int (*initialize)(const struct sre_RenderVFT** interface, void* renderdata, struct SDL_Window* window);
    size_t renderer_size; // The size, in bytes to allocate for the renderer structure
    size_t texture_size; // The size, in bytes to allocate for every texture

    const char* name; // Name tag of the render driver. It is technically optional, but very recommended
    unsigned flags; // sreRenderDriverBits
} sre_RenderDriverData;

SRE_CAPI_BEGIN

// Texture API

sre_Texture* sre_texture(sre_SDLpixelFormat format, int x, int y);
bool sre_texture_update(sre_Texture* texture, const sre_rect2Di* region, const void* pixels, int pitch);
bool sre_texture_query(sre_Texture* texture, int size[2], sre_SDLpixelFormat* format);

int sre_texture_aquire(sre_Texture* texture);
int sre_texture_release(sre_Texture* texture);

// C Render function wrappers

void sre_render_set_vsync(bool enable);
bool sre_render_set_scissors(const sre_rect2Dut* zone);
bool sre_render_set_blendmode(sre_blendMode blendmode);
void sre_render_reset_scissors();
void sre_render_draw1(sre_u32 flags, const sre_RenderInstance1 instances[], size_t instcount, sre_Texture* texture);
void sre_render_draw2(sre_u32 flags, const sre_RenderPoint points[], size_t pcount, sre_draw2primitive mode, sre_Texture* texture);

//
SRE_CAPI_END

#ifdef __cplusplus
    #include <vector>

    inline bool sre_Texture::update(const sre::rect2Di* region, const void* pixels, int pitch) { return sre_texture_update(this, region, pixels, pitch); }
    inline bool sre_Texture::update(const void* pixels, int pitch) { return sre_texture_update(this, NULL, pixels, pitch); }
    inline int sre_Texture::aquire() { return sre_texture_aquire(this); }
    inline int sre_Texture::release() { return sre_texture_release(this); }

    namespace sre
    {
        using Texture = sre_Texture;
        using blendMode = sre_blendMode;
        using draw2primitive = sre_draw2primitive;

        using RenderInstance1 = sre_RenderInstance1; // Render instance for all rectangle draw calls
        using RenderPoint = sre_RenderPoint;

        namespace render
        {
            void set_vsync(bool enable);
            bool set_scissors(sre::rect2Dut zone);
            bool set_blendmode(sre::blendMode mode);

            void reset_scissors();

            bool has_begun();
            bool begin(sre::col4 clear, sre::vec2ut camera);

            // Draw functions
            void draw1(sre::flags32 flags, const RenderInstance1 instances[], size_t instcount, Texture* texture=NULL);
            void draw2(sre::flags32 flags, const RenderPoint points[], size_t pcount, sre::draw2primitive mode=SRE_PRIMITIVE_TRIANGLES, Texture* texture=NULL);

            // Draw template helpers
            template <size_t n>
            void draw1(sre::flags32 flags, const RenderInstance1 (&instances)[n], Texture* texture=NULL) {
                draw1(flags, instances, n, texture);
            }
        
            template <size_t n>
            void draw2(sre::flags32 flags, const RenderPoint (&points)[n], sre::draw2primitive mode=SRE_PRIMITIVE_TRIANGLES, Texture* texture=NULL) {
                return draw2(flags, points, n, mode, texture);
            }

            // High level drawing functions

            inline void fill(sre::col4 color) {
                draw1(0, {{ {0, 65536}, 0, color }});
            }
        };

        inline Texture* texture(SDLpixelFormat formathint, int w, int h) { return sre_texture(formathint, w, h); }

        template <typename R, typename T=typename R::texture_type>
        struct RenderDriverHelper: sre_RenderDriverData
        {
            RenderDriverHelper(const char* name=NULL, unsigned flags=0): sre_RenderDriverData{
                [](const sre_RenderVFT** interface, void* inst, SDL_Window* window) -> int
                {
                    static const sre_RenderVFT this_interface = {
                        [](void* inst) { static_cast<R*>(inst)->~R(); },
                        [](void* inst, const sre_RenderInstance1* instances, size_t instance_count) {
                            static_cast<R*>(inst)->draw1(instances, instance_count);
                        },
                        [](void* inst, const sre_RenderPoint* points, size_t point_count, sre_draw2primitive mode) {
                            static_cast<R*>(inst)->draw2(points, point_count, mode);
                        },
                        [](void* inst, const float clear[4]) { static_cast<R*>(inst)->begin(clear); },
                        [](void* inst) { static_cast<R*>(inst)->end(); },

                        [](void* inst, int w, int h, sre_unit scale) { return static_cast<R*>(inst)->set_viewportstate(w, h, scale); },
                        [](void* inst, bool enable) { static_cast<R*>(inst)->set_vsync(enable); },
                        [](void* inst, void* texture) { static_cast<R*>(inst)->set_texturestate(static_cast<T*>(texture)); },
                        [](void* inst, sre_blendMode blending) { static_cast<R*>(inst)->set_blendstate(blending); },
                        [](void* inst, sre::unit x, sre::unit y) { static_cast<R*>(inst)->set_camerastate({x, y}); },
                        [](void* inst, const sre_rect2Di* rectangle) { static_cast<R*>(inst)->set_scissorstate(rectangle); },

                        [](void* inst, void* texture, sre_SDLpixelFormat format, int w, int h, sre_SDLpixelFormat* outformat) { return static_cast<R*>(inst)->texture_setup(static_cast<T*>(texture), format, w, h, outformat); },
                        [](void* inst, void* texture, const sre_rect2Di* region, const void* pixels, int pitch) { return static_cast<R*>(inst)->texture_update(static_cast<T*>(texture), region, pixels, pitch); },
                        [](void* inst, void* texture) { static_cast<R*>(inst)->texture_destroy(static_cast<T*>(texture)); }
                    };

                    int status = -1; // outstatus has a default value of 0, this allows constructors to return directly if they fail
                                        // Otherwise for returning success, you should write to it.
                    new(inst) R(window, &status);
                    if (status != SRE_RENDERSTATUS_SUCCEEDED) return status;

                    *interface = &this_interface;
                    return SRE_RENDERSTATUS_SUCCEEDED;
                },
                sizeof(R),
                sizeof(T),

                name,
                flags
            } {}
        };

        // Inheritable render driver template struct to be used with `RenderDriverHelper`
        struct RenderDriver
        {
            using texture_type = void*; // To be defined

            RenderDriver(SDL_Window* window, int* outstatus) {}
        protected:
            RenderDriver() = default;
        public:
            void draw1(const sre::RenderInstance1* instances, size_t instance_count);
            void draw2(const sre::RenderPoint* points, size_t point_count, sre::draw2primitive mode);
            
            void begin(const float clear[4]);
            void end();
    
            void set_viewportstate(int w, int h, sre::unit scale);
            void set_vsync(bool enable);
            void set_texturestate(texture_type* texture);
            void set_blendstate(sre::blendMode blending);
            void set_camerastate(sre::vec2ut camera);
            void set_scissorstate(const sre::rect2Di* rectangle);
                
            bool texture_setup(texture_type* texture, sre::SDLpixelFormat format, int w, int h, sre::SDLpixelFormat* outformat);
            bool texture_update(texture_type* texture, const sre::rect2Di* region, const void* pixels, int pitch);
            void texture_destroy(texture_type* texture);
        };
    }
#endif

#endif