#ifndef SRE_D3D9_H
#define SRE_D3D9_H

#include <Core/Render.h>
#include <d3d9.h>
#include <dxcommon/dxcommon.h>

#define SRE_DX9CALL(x) SRE_DXCALL(x); m_success = m_success && SUCCEEDED(hr)

namespace sreD3D9
{
    struct DrawData
    {
        IDirect3DVertexDeclaration9* dxdecl;
        IDirect3DVertexBuffer9* dxbuff_vert;
        IDirect3DVertexBuffer9* dxbuff_inst;

        ~DrawData()
        {
            dxdecl->Release();
            dxbuff_vert->Release();
            dxbuff_inst->Release();
        }
    };

    struct Texture
    {
        IDirect3DTexture9* dxtexture;
    };

    struct Instance
    {
        using texture_type = Texture;

        Instance(SDL_Window* window);
        ~Instance();
    private:
        bool m_success = true;
        IDirect3D9* m_dxd3d9;
        HWND m_hwnd;

        IDirect3DDevice9* m_dxdevice;

        IDirect3DPixelShader9* m_dxcps;
        IDirect3DVertexShader9* m_dxd1vs;
        IDirect3DVertexShader9* m_dxd2vs;

        DrawData m_d1data{};
        DrawData m_d2data{};
    public:
        bool succeeded() const { return m_success; }

        void flush_queueinstances1(Texture* texture, const sre::RenderInstance1* instances, size_t instance_count, sre::u32 flags, sre::u32 switch_flags);
        void flush_queueinstances2(Texture* texture, const sre::RenderInstance2* instance, size_t point_count, sre::u32 flags, sre::u32 switch_flags);
            
        void present();
        bool clear(float color[3]);
    
        bool set_viewportstate(int w, int h, sre::unit scale);
        bool set_blendstate(sre::blendMode blending);
        bool set_camerastate(sre::unit x, sre::unit y);
        void set_clipstate(const sre::rect2Di* rectangle);
        void set_vsync(bool enable);
                
        bool texture_setup(Texture* texture, sre::pixelFormat format, int w, int h, sre::pixelFormat* outformat);
        bool texture_update(Texture* texture, const void* pixels, int pitch);
        void texture_destroy(Texture* texture);

    private:
        bool _shadersetup();
        bool _statesetup();
    };
}

#endif