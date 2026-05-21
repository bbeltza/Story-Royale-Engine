#ifndef SRE_D3D11
#define SRE_D3D11

// Direct3D 11 Story Royale renderer
// It'll be the MAIN renderer of the game, no excuse.
// Fallbacks for this: OpenGL, maybe D3D9 soon first, and finally SDL's renderer

#include <Core/Render.h>

#include <d3d11.h>
#include <dxcommon/dxcommon.h>

#include <array>

namespace sreD3D11
{
    struct Texture
    {
        ID3D11Texture2D* dxtexture;
        ID3D11ShaderResourceView* dxsrv;
    };

    struct Shaders
    {
        ID3D11InputLayout* d1IL;
        ID3D11InputLayout* d2IL;
        ID3D11VertexShader* d1VS;
        ID3D11VertexShader* d2VS;
        ID3D11PixelShader* cPS; // Common pixel shader

        ~Shaders()
        {
            d1IL->Release();
            d2IL->Release();
            d1VS->Release();
            d2VS->Release();
            cPS->Release();
        }
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

    struct DLLS
    {
        HMODULE d3d11 = LoadLibrary("d3d11.dll");
        HMODULE dxgi = LoadLibrary("dxgi.dll");

        ~DLLS()
        {
            FreeLibrary(d3d11);
            FreeLibrary(dxgi);
        }
    };

    struct Instance: sre::RenderDriver
    {
        using texture_type = Texture;
        friend struct ImGuiData;

        Instance(SDL_Window* window, int* outstatus);
        ~Instance();

        private:
            //bool m_uselegacy = false;

            DLLS m_dlls{};

            ID3D11Device* m_dxdevice{};
            ID3D11DeviceContext* m_dxdevicecontext{};
            IDXGISwapChain* m_dxswapchain{};
            
            ID3D11RenderTargetView* m_dxrendertargetview{};

            Shaders m_shaders{};

            ID3D11BlendState* m_dxblendstates[5];
            ID3D11RasterizerState* m_dxrasterizerstate;
            ID3D11SamplerState* m_dxsamplerstate;

            DrawBuffer m_d1buffer;
            DrawBuffer m_d2bufferc; // Color draw2 buffer
            DrawBuffer m_d2bufferp; // Vertex draw2 buffer

            ID3D11Buffer* m_cbuffer;
            ID3D11Buffer* m_camveccbuffers[4];

            ID3D11ShaderResourceView* m_basictexture;

            InstCaches m_caches;
        public:
            // Instance drawing functions
            void flush_queueinstances1(Texture* texture, const sre::RenderInstance1* instances, size_t instance_count, sre::u32 flags, sre::u32 switch_flags);
            void flush_queueinstances2(Texture* texture, const sre::RenderInstance2* instance, size_t point_count, sre::u32 flags, sre::u32 switch_flags);
            
            void present();
            bool clear(float color[3]);
    
            // State functions
            bool set_viewportstate(int w, int h, sre::unit scale);
            bool set_blendstate(sre::blendMode blending);
            bool set_camerastate(sre::unit x, sre::unit y);
            void set_clipstate(const sre::rect2Di* rectangle);
            void set_vsync(bool enable) { m_caches.vsync = enable; }
                
            // Texture functions
            bool texture_setup(Texture* texture, sre::pixelFormat format, int w, int h, sre::pixelFormat* outformat);
            bool texture_update(Texture* texture, const sre::rect2Di* region, const void* pixels, int pitch);
            void texture_destroy(Texture* texture);
        private:
            bool _setuprendertargets();
    };
};

#endif