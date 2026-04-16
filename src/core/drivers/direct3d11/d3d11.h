#ifndef SRE_D3D11
#define SRE_D3D11

// Direct3D 11 Story Royale renderer
// It'll be the MAIN renderer of the game, no excuse.
// Fallbacks for this: OpenGL, maybe D3D9 soon first, and finally SDL's renderer

#include <Core/Render.h>

#include <dxgi1_2.h>
#include <d3d11.h>
#include <dxcommon/dxcommon.h>

#include <array>

#define SRE_DX11CALL(x) SRE_DXCALL(x); m_success = m_success && SUCCEEDED(hr)

struct sre_Sampler
{
    int w, h;
    ID3D11Texture2D* dxtexture;
    ID3D11ShaderResourceView* dxsrv;
};

namespace sreD3D11
{
    struct Shaders
    {
        ID3D11InputLayout* d1IL;
        ID3D11InputLayout* d2IL;
        ID3D11VertexShader* d1VS;
        ID3D11VertexShader* d2VS;
        ID3D11PixelShader* cPS; // Common pixel shader

        private:
            friend struct Instance;

            bool setup(ID3D11Device* dxdevice);
    };

    struct alignas(16) CBuffer
    {
        FLOAT viewport[16];
        sre::vec2f camera;
    };

    struct InstCaches
    {
        FLOAT viewport[16]{};
        bool vsync{};
    };

    struct DrawBuffer
    {
        ID3D11Buffer* dxbuffer;
        UINT index;
        UINT capacity;

        ~DrawBuffer() { dxbuffer->Release(); }

        bool init(ID3D11Device* dxdevice, UINT base_capacity);
        void reset() { index = 0; }
        bool resize(ID3D11Device* dxdevice, UINT new_width);
        bool append(ID3D11DeviceContext* dxdevicecontext, const void* data, UINT size);
    };

    struct Instance: sre::RenderInterface
    {
        Instance(SDL_Window* window);
        ~Instance();

        bool successful() const { return m_success; }

        private:
            bool m_success = true;

            ID3D11Device* m_dxdevice{};
            ID3D11DeviceContext* m_dxdevicecontext{};
            IDXGISwapChain1* m_dxswapchain{};
            
            ID3D11RenderTargetView* m_dxrendertargetview{};

            Shaders m_shaders{};

            ID3D11BlendState* m_dxblendstates[5];
            ID3D11RasterizerState* m_dxrasterizerstate;
            ID3D11SamplerState* m_dxsamplerstate;

            DrawBuffer m_d1buffer;
            DrawBuffer m_d2bufferc; // Color draw2 buffer
            DrawBuffer m_d2bufferp; // Vertex draw2 buffer

            ID3D11Buffer* m_cbuffers[2];

            ID3D11ShaderResourceView* m_basictexture;

            InstCaches m_caches;
        protected:
            // Instance drawing functions
            virtual void SRE_RENDERCALL flush_queueinstances1(sre::Sampler* texture, const sre::RenderInstance1* instances, size_t instance_count, sre::u32 flags, sre::u32 switch_flags) override;
            virtual void SRE_RENDERCALL flush_queueinstances2(const sre::RenderInstance2& instance, size_t point_count, sre::u32 flags, sre::u32 switch_flags) override;
            
            virtual void SRE_RENDERCALL present() override;
            virtual bool SRE_RENDERCALL clear(float color[3]) override;
    
            // State functions
            virtual bool SRE_RENDERCALL set_viewportstate(int w, int h, sre::unit scale) override;
            virtual bool SRE_RENDERCALL set_blendstate(sre::blendMode blending) override;
            virtual bool SRE_RENDERCALL set_camerastate(sre::unit x, sre::unit y) override;
            virtual void SRE_RENDERCALL set_clipstate(const sre::rect2Di* rectangle) override;
            virtual void SRE_RENDERCALL set_vsync(bool enable) override { m_caches.vsync = enable; }
                
            // Texture functions
            virtual bool SRE_RENDERCALL setup_texture(sre::Sampler* texture, sre::pixelFormat format, int x, int y) override;
            virtual bool SRE_RENDERCALL update_texture(sre::Sampler* texture, const void* pixels, int pitch=0) override;
            virtual bool SRE_RENDERCALL query_texture(sre::Sampler* texture, sre::vec2i* size, sre::pixelFormat* format) override;
            virtual void SRE_RENDERCALL destroy_texture(sre::Sampler* texture) override;
    };
};

#endif