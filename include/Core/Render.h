#ifndef SRE_RENDER_H
#define SRE_RENDER_H
#include <C_API.h>
#include <ints.h>

#if _WIN32
    #define SRE_RENDERCALL __cdecl
#else
    #define SRE_RENDERCALL
#endif

#include <Base/Pixel.h>

typedef struct sre_Sampler sre_Sampler;

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
    SRE_DRAWFLAG_CAMERA = 1 << 0
};

struct SDL_Window;

struct sre_SamplerNew
{
    int _refcount;
    int w, h;
    sre_pixelFormat format;
    void* renderer;
};

#ifndef __cplusplus
    #include <Datatypes/CRect.h>
    #include <Datatypes/CColor.h>
    #include <Datatypes/Units.h>

    SRE_RECT2DMAKE(sre_unit, ut);
    SRE_RECT2DMAKE(int, i);
    SRE_VEC2MAKE(float, f);

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
        sre_col4 color;
        sre_draw2mode mode;
        sre_vec2ut points[];
    } sre_RenderInstance2;

    // C interface, experimental
    struct _sre_RenderInterfacevft
    {
        void (SRE_RENDERCALL *const flush_queueinstances1)(void*, sre_Sampler*const* inst_textures, const sre_RenderInstance1* instances, size_t instance_count, sre_u32 flags);
        void (SRE_RENDERCALL *const flush_queueinstances2)(void*, const sre_RenderInstance2* instance, size_t point_count, sre_u32 flags);

        void (SRE_RENDERCALL *const present)(void*);
        bool (SRE_RENDERCALL *const clear)(void*, float color[3]);
        
        bool (SRE_RENDERCALL *const set_viewportstate)(void*, int w, int h, sre_unit scale);
        bool (SRE_RENDERCALL *const set_blendstate)(void*, sre_blendMode blending);
        bool (SRE_RENDERCALL *const set_camerastate)(void*, sre_unit x, sre_unit y);
        void (SRE_RENDERCALL *const set_clipstate)(void*, const sre_rect2Di* rectangle);
        void (SRE_RENDERCALL *const set_vsync)(void*, bool enable);

        bool (SRE_RENDERCALL *const setup_texture)(void*, sre_Sampler* texture, sre_pixelFormat format, int w, int h);
        bool (SRE_RENDERCALL *const update_texture)(void*, sre_Sampler* texture, const void* pixels, int pitch);
        bool (SRE_RENDERCALL *const query_texture)(void*, sre_Sampler* texture, int size[2], sre_pixelFormat* format);
        void (SRE_RENDERCALL *const destroy_texture)(void*, sre_Sampler* texture);
    };

    typedef struct sre_RenderInterface
    {
        const struct _sre_RenderInterfacevft* vftptr;

        void* _textures; // Arena structure
        sre_Sampler** _textures_fl;
        size_t _textures_flsize;
        size_t _textures_flcapacity;

        short _blendmode;

        void* _vector_data1[4][4];
    } sre_RenderInterface;
    extern sre_RenderInterface* sre_getrenderer();

    typedef struct sre_RenderDriverData
    {
        sre_RenderInterface* (*init)(struct SDL_Window* window);
        void (*destroy)(sre_RenderInterface* render);
        size_t texture_size;
    } sre_RenderDriverData;

    SRE_CAPI_BEGIN
    // You must use these functions to properly initialize and destroy the rendering interface if you're using the C API
    void sre_RIconstructor(sre_RenderInterface* interface);
    void sre_RIdestructor(sre_RenderInterface* interface);
    SRE_CAPI_END

#else
    #include <Datatypes/Rect.hpp>
    #include <Datatypes/Color.hpp>
    #include <Datatypes/Flags.hpp>

    #include <vector>

    namespace sre
    {
        using Sampler = sre_Sampler;
        using blendMode = sre_blendMode;
        using draw2Mode = sre_draw2mode;

        struct RenderInstance1 // Render instance for all rectangle draw calls
        {
            sre::rect2Dut rectangle; // rectangle area
            sre::vec2ut anchor; // anchor point
            sre::col4 color;
            float angle;

            sre::vec2f uv;
            sre::vec2f uv_offset;
        };
        struct RenderInstance2 // Render instance for all line & geometry draw calls
        {
            sre::col4 color;
            sre::draw2Mode mode;
            sre::vec2ut points[1]; // Size set to 1, don't bother, it's in the `point_count` parameter in `flush_queueinstances2`
        };

        struct RenderQueue
        {
            size_t count;
            char type; // 1 for RenderInstance1, or 2 for RenderInstance2
            short blendmode;
            sre::flags32 flags;
        };

        // Engine managed renderer class (it just has static functions that are wrapped by the engine)
        class CoreRenderer;

        // New future render driver interface class
        struct RenderInterface
        {
            friend class ::sre::CoreRenderer;

            void clip_reset();
            void clip_set(sre::rect2Dut zone);

            void blend(sre::blendMode blend) { m_blendmode = blend; }
            void blend() { m_blendmode = SRE_BLEND_DEFAULT; }

            void draw1(sre::flags32 flags, const RenderInstance1 instances[], size_t instcount, Sampler*const samplers[]=NULL);
            void draw2(sre::flags32 flags, sre::col4 color, const sre::vec2ut points[], size_t pcount, sre::draw2Mode mode=SRE_DRAW2_JOINED);

            template <size_t n>
            void draw1(sre::flags32 flags, const RenderInstance1 (&instances)[n], Sampler*const (&samplers)[n]={NULL}) { draw1(flags, instances, n, samplers); }

            template <size_t n>
            void draw2(sre::flags32 flags, sre::col4 color, const sre::vec2ut (&points)[n], sre::draw2Mode mode=SRE_DRAW2_JOINED) { return draw2(flags, color, points, n, mode); }

            void fill(sre::col4 color) { draw1(0, {{ {0, 65536}, 0, color }}); }

            // Create a "sampler", it's a piece of texture that can get rendered
            // It replaces the current textures
            inline Sampler* sampler(pixelFormat format, int x, int y);
            inline void sampler(Sampler* sampler_todestroy);
            inline bool sampler_update(Sampler* sampler, const void* pixels, int pitch);
            inline bool sampler_query(Sampler* sampler, sre::vec2i* size, pixelFormat* format);

            protected: // Full interface
                // Instance drawing functions
                virtual void SRE_RENDERCALL flush_queueinstances1(Sampler*const* inst_textures, const RenderInstance1* instances, size_t instance_count, sre::u32 flags) = 0;
                virtual void SRE_RENDERCALL flush_queueinstances2(const RenderInstance2& instance, size_t point_count, sre::u32 flags) = 0;

                virtual void SRE_RENDERCALL present() = 0; // Present the screen, acts as the last function to be run in the current frame
                virtual bool SRE_RENDERCALL clear(float color[3]) = 0; // Clear the screen and more likely the target buffer, also acts as the rendering frame setup function

                // State functions
                virtual bool SRE_RENDERCALL set_viewportstate(int w, int h, sre::unit scale) = 0;
                virtual bool SRE_RENDERCALL set_blendstate(blendMode blending) = 0;
                virtual bool SRE_RENDERCALL set_camerastate(sre::unit x, sre::unit y) = 0;
                virtual void SRE_RENDERCALL set_clipstate(const sre::rect2Di* rectangle) = 0;
                virtual void SRE_RENDERCALL set_vsync(bool enable) = 0;
                
                // Texture functions
                virtual bool SRE_RENDERCALL setup_texture(Sampler* texture, pixelFormat format, int x, int y) = 0;
                virtual bool SRE_RENDERCALL update_texture(Sampler* texture, const void* pixels, int pitch) = 0;
                virtual bool SRE_RENDERCALL query_texture(Sampler* texture, sre::vec2i* size, pixelFormat* format) = 0;
                virtual void SRE_RENDERCALL destroy_texture(Sampler* texture) = 0;
                
                RenderInterface();
                ~RenderInterface();
            private:

                void* textures; // Arena structure
                sre_Sampler** textures_fl;
                size_t textures_flsize;
                size_t textures_flcapacity;

                short m_blendmode = SRE_BLEND_DEFAULT;

                std::vector<Sampler*> m_texturecache;
                std::vector<RenderInstance1> m_rinst1cache;
                std::vector<sre::byte> m_rinst2cache; // sre::byte to be more like a point buffer. RenderInstance2's size varies

                // Render queue, what will be inserted in the draw functions
                std::vector<RenderQueue> m_renderqueues;
        };

        RenderInterface* get_renderer(void);

        // Structure used to define driver initialization data
        struct RenderDriverData
        {
            RenderInterface* (*initialize)(SDL_Window* window);
            void (*destroy)(RenderInterface* renderer);
            size_t texture_size; // The size, in bytes of a texture
        };
    }

    #define sre_RenderInterface sre::RenderInterface

#endif

SRE_CAPI_BEGIN
    sre_Sampler* sre_RI_sampler(sre_RenderInterface* render, sre_pixelFormat formathint, int w, int h);
    void sre_RI_samplerdestroy(sre_RenderInterface* render, sre_Sampler* sampler);
    bool sre_RI_samplerupdate(sre_RenderInterface* render, sre_Sampler* sampler, const void* pixels, int pitch);
    bool sre_RI_samplerquery(sre_RenderInterface* render, sre_Sampler* sampler, int size[2], sre_pixelFormat* format);
SRE_CAPI_END

#ifdef __cplusplus
    sre::Sampler* sre::RenderInterface::sampler(pixelFormat formathint, int w, int h) { return sre_RI_sampler(this, static_cast<sre_pixelFormat>(formathint), w, h); }
    void sre::RenderInterface::sampler(sre::Sampler* sampler_todestroy) { sre_RI_samplerdestroy(this, sampler_todestroy); }
    bool sre::RenderInterface::sampler_update(sre::Sampler* sampler, const void* pixels, int pitch) { return sre_RI_samplerupdate(this, sampler, pixels, pitch); }
    bool sre::RenderInterface::sampler_query(sre::Sampler* sampler, sre::vec2i* size, sre::pixelFormat* format) { return sre_RI_samplerquery(this, sampler, &size->x, reinterpret_cast<sre_pixelFormat*>(format)); }
#endif

#endif