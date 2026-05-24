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

    // Dnyamic buffer abstraction also seen in d3d11's and d3d12's drivers
    struct DBuff
    {
        IDirect3DVertexBuffer9* dxbuff{};

        UINT pos;
        UINT cap;

        void reset() { pos = 0; }
        bool init(IDirect3DDevice9* dxdevice, UINT capacity);
        UINT append(IDirect3DDevice9* dxdevice, const void* data, UINT size);

        template <typename T>
        bool init(IDirect3DDevice9* dxdevice, UINT count) { return init(dxdevice, count*sizeof(T)); }
        template <typename T>
        UINT append(IDirect3DDevice9* dxdevice, const void* data, UINT count) { return append(dxdevice, data, count*sizeof(T)); }

        inline void release()
        {
            if (dxbuff)
            {
                dxbuff->Release();
                dxbuff = NULL;
            }
        }
    };

    struct Texture
    {
        IDirect3DTexture9* dxtexture;
        float potratiox, potratioy;
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

        struct {
            IDirect3DVertexDeclaration9* dxvertexdecl{};
            IDirect3DVertexBuffer9* dxpervertexbuf{};
            IDirect3DIndexBuffer9* dxindexbuf{};

            DBuff perinstancebuf{};

            void releaseresources()
            {
                perinstancebuf.release();
                if (dxvertexdecl)
                {
                    dxvertexdecl->Release();
                    dxindexbuf->Release();
                    dxpervertexbuf->Release();

                    dxvertexdecl = NULL;
                    dxindexbuf = NULL;
                    dxpervertexbuf = NULL;
                }
            }
        } m_d1data{};
        struct {
            IDirect3DVertexDeclaration9* dxvertexdecl{};
            DBuff pervertexbuf{};
            DBuff perinstancebuf{};

            void releaseresources()
            {
                perinstancebuf.release();
                pervertexbuf.release();
                if (dxvertexdecl)
                {
                    dxvertexdecl->Release();
                    dxvertexdecl = NULL;
                }
            }
        } m_d2data{};

        D3DPRESENT_PARAMETERS m_pparamcache{};
        FLOAT m_viewportcache[16];
        FLOAT m_cameracache[2];

        D3DCAPS9 m_devcaps;
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

        // @returns One of the render states to return
        int _checkdevice(UINT adapter, D3DDEVTYPE type);
    };
}

#endif