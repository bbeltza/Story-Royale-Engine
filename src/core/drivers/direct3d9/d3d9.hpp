#ifndef SRE_D3D9_H
#define SRE_D3D9_H

#include <Core/Render.h>
#include <d3d9.h>
#include <dxcommon/dxcommon.h>

namespace sreD3D9
{
    struct DLLS
    {
        HMODULE d3d9 = LoadLibrary("D3d9.dll");
        ~DLLS() { FreeLibrary(d3d9); }
    };

    struct DrawData
    {
        IDirect3DVertexDeclaration9* dxdecl;
        IDirect3DVertexBuffer9* dxbuff_vert;
        IDirect3DVertexBuffer9* dxbuff_inst;

        void releaseresources()
        {
            if (!dxdecl)
                return;

            dxdecl->Release();
            dxbuff_vert->Release();
            dxbuff_inst->Release();

            dxdecl = NULL;
            dxbuff_vert = NULL;
            dxbuff_inst = NULL;
        }
    };

    struct Draw1Data: DrawData
    {
        IDirect3DIndexBuffer9* dxibuff;

        void releaseresources()
        {
            DrawData::releaseresources();
            _released1resources();
        }
    private:
        void _released1resources()
        {
            if (!dxibuff)
                return;
            dxibuff->Release();
            dxibuff = NULL;
        }
    };

    struct Texture
    {
        IDirect3DTexture9* dxtexture;
        int height_cache;
    };

    struct Instance: sre::RenderDriver
    {
        using texture_type = Texture;
        friend struct ImGuiData;

        Instance(SDL_Window* window, int* outstatus);
        ~Instance();
    private:
        DLLS m_dlls;
    private:
        bool m_needsetup = true;
        IDirect3D9* m_dxd3d9;

        IDirect3DDevice9* m_dxdevice;

        IDirect3DPixelShader9* m_dxcps;
        IDirect3DVertexShader9* m_dxd1vs;
        IDirect3DVertexShader9* m_dxd2vs;

        IDirect3DTexture9* m_dxbasictexture{};

        Draw1Data m_d1data{};
        DrawData m_d2data{};

        D3DPRESENT_PARAMETERS m_pparamcache{};
        FLOAT m_viewportcache[16];
        FLOAT m_cameracache[2];
    public:
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
        bool texture_update(Texture* texture, const sre::rect2Di* region, const void* pixels, int pitch);
        void texture_destroy(Texture* texture);
    private:
        bool _shadersetup();
        bool _statesetup();
        void _resetdevice();
        void _releaseresources()
        {
            m_d1data.releaseresources();
            m_d2data.releaseresources();
        }

        void _invalidateimgui();
    };
}

#endif