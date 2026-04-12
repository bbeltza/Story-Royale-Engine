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
            virtual void SRE_RENDERCALL flush_queueinstances1(sre::Sampler*const* inst_textures, const sre::RenderInstance1* instances, size_t instance_count, sre::u32 flags) override {}
            virtual void SRE_RENDERCALL flush_queueinstances2(const sre::RenderInstance2& instance, size_t point_count, sre::u32 flags) override {}
            
            virtual void SRE_RENDERCALL present() override {}
            virtual bool SRE_RENDERCALL clear(float color[3]) override { return false; }
    
            // State functions
            virtual bool SRE_RENDERCALL set_viewportstate(int w, int h, sre::unit scale) override { return false; }
            virtual bool SRE_RENDERCALL set_blendstate(sre::blendMode blending) override { return false; }
            virtual bool SRE_RENDERCALL set_camerastate(sre::unit x, sre::unit y) override { return false; }
            virtual void SRE_RENDERCALL set_clipstate(const sre::rect2Di* rectangle) override {}
            virtual void SRE_RENDERCALL set_vsync(bool enable) override {}
                
            // Texture functions
            virtual bool SRE_RENDERCALL setup_texture(sre::Sampler* texture, sre::pixelFormat format, int x, int y) override { return false; }
            virtual bool SRE_RENDERCALL update_texture(sre::Sampler* texture, const void* pixels, int pitch=0) override { return false; }
            virtual bool SRE_RENDERCALL query_texture(sre::Sampler* texture, sre::vec2i* size, sre::pixelFormat* format) override { return false; }
            virtual bool SRE_RENDERCALL destroy_texture(sre::Sampler* texture) override { return false; }
    };
};

#endif