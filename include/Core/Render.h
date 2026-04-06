#ifndef SRE_RENDER_H
#define SRE_RENDER_H
#include <C_API.h>

#include <Draw.hpp> // Only for drawBlending, removing soon

typedef struct sre_Sampler sre_Sampler;

#ifndef __cplusplus

    struct _sre_RenderInterfacevft
    {
        void (const *destructor)(void*);

        void (const *present)();
        bool (const *clear)(float color[3]);

        bool (const *set_viewportstate)(int w, int h);
        bool (const *set_blendstate)(sre_DrawBlending blending);

        bool (const *setup_texture)(sre_Sampler* texture, pixelFormat format, int w, int h);
        bool (const *update_texture)(sre_Sampler* texture, const void* pixels);
    };

    typedef struct _sre_RenderInterface *sre_RenderInterface;
    extern sre_RenderInterface* sre_getrenderer();

#else

    namespace sre
    {
        enum pixelFormat;

        using Sampler = sre_Sampler;

        // New future render driver interface class
        struct RenderInterface
        {
            protected:
                virtual ~RenderInterface() {}

                // There's a lot more to add...
                
                virtual void present() = 0;
                virtual bool clear(float color[3]) = 0;

                virtual bool set_viewportstate(int w, int h) = 0;
                virtual bool set_blendstate(drawBlending blending) = 0;

                virtual bool setup_texture(Sampler* texture, pixelFormat format, int x, int y) = 0;
                virtual bool update_texture(Sampler* texture, const void* pixels) = 0;
                virtual bool query_texture(Sampler* texture, int size[2], pixelFormat* format) = 0;

                virtual bool draw_command() = 0;

        };

        RenderInterface* get_renderer(void);
    }

#endif

#endif