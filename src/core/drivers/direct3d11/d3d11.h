#ifndef SRE_D3D11
#define SRE_D3D11

// Direct3D 11 Story Royale renderer
// It'll be the MAIN renderer of the game, no excuse.
// Fallbacks for this: OpenGL, maybe D3D9 soon first, and finally SDL's renderer

#include <Core/Render.h>

#include <dxgi1_2.h>
#include <d3d11.h>

namespace sred3d11
{
    struct Interface: sre::RenderInterface
    {
        Interface(SDL_Window* window);

        protected:
            // Instance drawing functions
            virtual void flush_queueinstances1(const sre::RI1Texture* inst_textures, const sre::RenderInstance1* instances, size_t instance_count, sre::flags32 flags) {}
            virtual void flush_queueinstances2(const sre::RenderInstance2& instance, size_t point_count, sre::flags32 flags) {}
    
            virtual void present() {}
            virtual bool clear(float color[3]) { return false; }
    
            // State functions
            virtual bool set_viewportstate(int w, int h, sre::unit scale) { return false; }
            virtual bool set_blendstate(sre::drawBlending blending) { return false; }
            virtual bool set_camerastate(sre::unit x, sre::unit y) { return false; }
                
            // Texture functions
            virtual bool setup_texture(sre::Sampler* texture, sre::pixelFormat format, int x, int y) { return false; }
            virtual bool update_texture(sre::Sampler* texture, const void* pixels, int pitch=0) { return false; }
            virtual bool query_texture(sre::Sampler* texture, sre::vec2i& size, sre::pixelFormat* format) { return false; }
    };
};

#endif