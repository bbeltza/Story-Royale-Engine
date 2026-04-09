#ifndef SRE_RENDER_H
#define SRE_RENDER_H
#include <C_API.h>

typedef struct sre_Sampler sre_Sampler;

struct SDL_Window;

#ifndef __cplusplus

    // C interface, experimental and not even available
    struct _sre_RenderInterfacevft
    {
        void (const *present)(void*);
        bool (const *clear)(void*, float color[3]);

        bool (const *setviewportstate)(void*, int w, int h);
        bool (const *setblendstate)(void*, sre_DrawBlending blending);

        bool (const *setup_texture)(void*, sre_Sampler* texture, pixelFormat format, int w, int h);
        bool (const *update_texture)(void*, sre_Sampler* texture, const void* pixels);
    };

    typedef struct sre_RenderInterface
    {
        struct _sre_RenderInterfacevft* vftptr;
    };
    extern sre_RenderInterface* sre_getrenderer();

    typedef struct sre_RenderDriverData
    {
        sre_RenderInterface* (*initialize)(SDL_Window* window);
        void (*destroy)(sre_RenderInterface* render);
        size_t texture_size;
    } sre_RenderDriverData;

#else
    #include <Core/Draw.hpp> // Only for drawBlending, removing soon
    #include <Datatypes/Rect.hpp>
    #include <Datatypes/Color.hpp>
    #include <Datatypes/Flags.hpp>

    #include <vector>

    namespace sre
    {
        using Sampler = sre_Sampler;
        enum pixelFormat;

        struct RI1Texture // Used with `RenderInstance1` to define textures to draw
        {
            Sampler* texture;
        };
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

            void draw1(sre::flags32 flags, const RenderInstance1 instances[], size_t instcount, Sampler*const samplers[]=NULL);
            void draw2(sre::flags32 flags, sre::col4 color, const sre::vec2ut points[], size_t pcount);

            template <size_t n>
            void draw1(sre::flags32 flags, const RenderInstance1 (&instances)[n], Sampler*const (&samplers)[n]={NULL}) { draw1(flags, instances, n, samplers); }

            protected: // Full interface
                // Instance drawing functions
                virtual void flush_queueinstances1(const RI1Texture* inst_textures, const RenderInstance1* instances, size_t instance_count, sre::flags32 flags) = 0;
                virtual void flush_queueinstances2(const RenderInstance2& instance, size_t point_count, sre::flags32 flags) = 0;

                virtual void present() = 0; // Present the screen, acts as the last function to be run in the current frame
                virtual bool clear(float color[3]) = 0; // Clear the screen and more likely the target buffer, also acts as the rendering frame setup function

                // State functions
                virtual bool set_viewportstate(int w, int h, sre::unit scale) = 0;
                virtual bool set_blendstate(drawBlending blending) = 0;
                virtual bool set_camerastate(sre::unit x, sre::unit y) = 0;
                
                // Texture functions
                virtual bool setup_texture(Sampler* texture, pixelFormat format, int x, int y) = 0;
                virtual bool update_texture(Sampler* texture, const void* pixels, int pitch=0) = 0;
                virtual bool query_texture(Sampler* texture, sre::vec2i& size, pixelFormat* format) = 0;
            private:
                std::vector<RI1Texture> m_texturecache;
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

#endif

#endif