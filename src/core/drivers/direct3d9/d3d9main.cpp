#ifndef NDEBUG
    #define D3D_DEBUG_INFO
#endif

#include "d3d9.hpp"
#include <SDL_syswm.h>
#include <assert.h>

SRE_EXTERN_C_VAR sre::RenderDriverHelper<sreD3D9::Instance> sred3d9{"Direct3D 9"};

using namespace sreD3D9;

int Instance::_checkdevice(UINT adapter, D3DDEVTYPE devicetype)
{
    HRESULT hr;
    SRE_DXCALL(m_dxd3d9->GetDeviceCaps(adapter, devicetype, &m_devcaps));
    if (hr == D3DERR_NOTAVAILABLE)
        return SRE_RENDERSTATUS_UNSUPPORTED;
    if (hr != S_OK)
       return SRE_RENDERSTATUS_FAILED;

    if (!(m_devcaps.TextureCaps | D3DPTEXTURECAPS_ALPHA)) // Textures have to support alpha... Please!!...
        return SRE_RENDERSTATUS_UNSUPPORTED;

    return SRE_RENDERSTATUS_SUCCEEDED;
}

Instance::Instance(SDL_Window* window, int* outstatus)
{
    SDL_SysWMinfo wminfo;
    SDL_GetVersion(&wminfo.version);
    if (!SDL_GetWindowWMInfo(window, &wminfo) || wminfo.subsystem != SDL_SYSWM_WINDOWS)
    {
        *outstatus = SRE_RENDERSTATUS_UNSUPPORTED;
        return;
    }

    using PFN_DIRECT3DCREATE9_PROC = IDirect3D9 * (WINAPI*)(UINT SDKVersion);
    SRE_DXGETADDR(Direct3DCreate9, PFN_DIRECT3DCREATE9_PROC, m_dlls.d3d9);

    HRESULT hr;
    m_dxd3d9 = pDirect3DCreate9(D3D_SDK_VERSION);
    if (!m_dxd3d9)
    {
        *outstatus = SRE_RENDERSTATUS_UNSUPPORTED;
        return;
    }

    UINT adaptercount = m_dxd3d9->GetAdapterCount();
    UINT adapter = static_cast<UINT>(-1);
    D3DDEVTYPE devtype = D3DDEVTYPE_HAL;

    if (!adaptercount)
    {
        *outstatus = SRE_RENDERSTATUS_UNSUPPORTED;
        return;
    }

    for (UINT i = 0; i < adaptercount; i++)
    {
        switch (_checkdevice(i, D3DDEVTYPE_HAL))
        {
            case SRE_RENDERSTATUS_FAILED: return;
            case SRE_RENDERSTATUS_UNSUPPORTED: continue;
            default: break;
        }

        adapter = i;
        break;
    }

    /* REF devices need a LOT of adjustments over resources, so it's just better to not support it
    if (adapter == static_cast<UINT>(-1))
    {
        // Check for REF devices, there are not HAL devices available
        devtype = D3DDEVTYPE_REF;
        for (UINT i = 0; i < adaptercount; i++)
        {
            switch (_checkdevice(i, D3DDEVTYPE_REF))
            {
                case SRE_RENDERSTATUS_FAILED: return;
                case SRE_RENDERSTATUS_UNSUPPORTED: continue;
                default: break;
            }

            adapter = i;
            break;
        }
    }
    */

    if (adapter == static_cast<UINT>(-1))
    {
        *outstatus = SRE_RENDERSTATUS_UNSUPPORTED;
        return;
    }

    m_pparamcache.hDeviceWindow = wminfo.info.win.window;
    m_pparamcache.BackBufferFormat = D3DFMT_UNKNOWN;
    m_pparamcache.BackBufferCount = 1; // Use two buffers? Or one? I've seen performance be better on only 1 back-buffer
    m_pparamcache.SwapEffect = D3DSWAPEFFECT_DISCARD;
    m_pparamcache.Windowed = TRUE;
    m_pparamcache.Flags = D3DPRESENTFLAG_VIDEO;
    m_pparamcache.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    SRE_DXCALL(m_dxd3d9->CreateDevice(adapter,
                                       devtype,
                                       wminfo.info.win.window,
                                       D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_NOWINDOWCHANGES | D3DCREATE_FPU_PRESERVE,
                                       &m_pparamcache,
                                       &m_dxdevice
    ));

    if (hr == D3DERR_NOTAVAILABLE)
    {
        *outstatus = SRE_RENDERSTATUS_UNSUPPORTED;
        return;
    }
    if (hr != S_OK)
        return;

    if (!_shadersetup())
        return;

    D3DLOCKED_RECT rect;
    SRE_DXCALLC(m_dxdevice->CreateTexture(1, 1, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &m_dxbasictexture, NULL));
    SRE_DXCALLC(m_dxbasictexture->LockRect(0, &rect, NULL, D3DLOCK_DISCARD));
        *static_cast<DWORD*>(rect.pBits) = 0xFFFFFFFF;
    SRE_DXCALLC(m_dxbasictexture->UnlockRect(0));

    *outstatus = SRE_RENDERSTATUS_SUCCEEDED;
}

Instance::~Instance()
{
    m_dxd3d9->Release();
    m_dxdevice->Release();

    m_dxcps->Release();
    m_dxd1vs->Release();
    m_dxd2vs->Release();

    m_dxbasictexture->Release();

    _releaseresources();
}

bool Instance::_statesetup()
{
    HRESULT hr;

    static const D3DVERTEXELEMENT9 D1_DECLARATION[] = {
        {0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        {1, offsetof(sre::RenderInstance1, rectangle), D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 1 },
        {1, offsetof(sre::RenderInstance1, anchor), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 2 },
        {1, offsetof(sre::RenderInstance1, color), D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        {1, offsetof(sre::RenderInstance1, angle), D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_PSIZE, 0 },
        {1, offsetof(sre::RenderInstance1, uv), D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END()
    };

    static const D3DVERTEXELEMENT9 D2_DECLARATION[] = {
        {0, offsetof(sre::RenderPoint, pos), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        {0, offsetof(sre::RenderPoint, uv), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        {1, 0, D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        D3DDECL_END()
    };

    SRE_DXCALLF(m_dxdevice->CreateVertexDeclaration(D1_DECLARATION, &m_d1data.dxvertexdecl));
    SRE_DXCALLF(m_dxdevice->CreateVertexDeclaration(D2_DECLARATION, &m_d2data.dxvertexdecl));

    SRE_DXCALLF(m_dxdevice->CreateVertexBuffer(sizeof(FLOAT[2*4]), 0, 0, D3DPOOL_DEFAULT, &m_d1data.dxpervertexbuf, NULL));
    SRE_DXCALLF(m_dxdevice->CreateIndexBuffer(sizeof(USHORT)*6, 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_d1data.dxindexbuf, NULL));

    if (!m_d1data.perinstancebuf.init<sre::RenderInstance1>(m_dxdevice, 255))
        return false;
    if (!m_d2data.pervertexbuf.init<sre::RenderPoint>(m_dxdevice, 25))
        return false;
    if (!m_d2data.perinstancebuf.init<sre::col4>(m_dxdevice, 255))
        return false;

    {
        FLOAT* mapped_vertices = NULL;
        USHORT* mapped_indices = NULL;
        SRE_DXCALLF(m_d1data.dxpervertexbuf->Lock(0, 0, reinterpret_cast<void**>(&mapped_vertices), D3DLOCK_DISCARD));
            mapped_vertices[0] = 0.0f;
            mapped_vertices[1] = 0.0f;

            mapped_vertices[2] = 0.0f;
            mapped_vertices[3] = 1.0f;

            mapped_vertices[4] = 1.0f;
            mapped_vertices[5] = 1.0f;
        
            mapped_vertices[6] = 1.0f;
            mapped_vertices[7] = 0.0f;
        SRE_DXCALLF(m_d1data.dxpervertexbuf->Unlock());
        SRE_DXCALLF(m_d1data.dxindexbuf->Lock(0, 0, reinterpret_cast<void**>(&mapped_indices), D3DLOCK_DISCARD));
            mapped_indices[0] = 0;
            mapped_indices[1] = 1;
            mapped_indices[2] = 2;
            mapped_indices[3] = 2;
            mapped_indices[4] = 3;
            mapped_indices[5] = 0;
        SRE_DXCALLF(m_d1data.dxindexbuf->Unlock());
    }

    SRE_DXCALLF(m_dxdevice->SetPixelShader(m_dxcps));
    SRE_DXCALLF(m_dxdevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE));
    SRE_DXCALLF(m_dxdevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
    SRE_DXCALLF(m_dxdevice->SetRenderState(D3DRS_LIGHTING, FALSE));

    SRE_DXCALLF(m_dxdevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP));
    SRE_DXCALLF(m_dxdevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP));
    SRE_DXCALLF(m_dxdevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
    SRE_DXCALLF(m_dxdevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT));

    SRE_DXCALL(m_dxdevice->SetVertexShaderConstantF(0, m_viewportcache, 4));

    m_needsetup = false;
    return true;
}

void Instance::_resetdevice()
{
    HRESULT hr;
    _releaseresources();
    SRE_DXCALL(m_dxdevice->Reset(&m_pparamcache));
    if (FAILED(hr))
        abort();
    
    m_needsetup = true;
}

//

void Instance::draw1(const sre::RenderInstance1* instances, size_t instance_count)
{
    HRESULT hr;
    if (1)
    {
        SRE_DXCALL(m_dxdevice->SetVertexDeclaration(m_d1data.dxvertexdecl));
        SRE_DXCALL(m_dxdevice->SetVertexShader(m_dxd1vs));
        SRE_DXCALL(m_dxdevice->SetIndices(m_d1data.dxindexbuf));
        SRE_DXCALL(m_dxdevice->SetStreamSource(0, m_d1data.dxpervertexbuf, 0, sizeof(FLOAT[2])));
        SRE_DXCALL(m_dxdevice->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1));
    }

    UINT UINT_instcount = static_cast<UINT>(instance_count);

    if (m_usepot)
    {
        assert(m_potratio.x != 1.0f || m_potratio.y != 1.0f);

        // The interface spec will state that `instances` is in memory free to use by the driver, and only to be used in this single function call, so you're totally free to modify it.
        // Setting it as non-const will be possible too
        for (UINT i = 0; i < UINT_instcount; i++)
        {
            const_cast<sre::RenderInstance1*>(instances)[i].uv *= m_potratio;
            const_cast<sre::RenderInstance1*>(instances)[i].uv_offset *= m_potratio;
        }
    }

    UINT offset = m_d1data.perinstancebuf.append<sre::RenderInstance1>(m_dxdevice, instances, UINT_instcount);
    SRE_DXCALL(m_dxdevice->SetStreamSource(1, m_d1data.perinstancebuf.dxbuff, offset, sizeof(sre::RenderInstance1)));
    SRE_DXCALL(m_dxdevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | UINT_instcount));

    SRE_DXCALL(m_dxdevice->SetPixelShader(m_dxcps));
    SRE_DXCALL(m_dxdevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2*UINT_instcount));
}

void Instance::draw2(const sre::RenderInstance2* instance, size_t point_count)
{
    HRESULT hr;

    if (1)
    {
        SRE_DXCALL(m_dxdevice->SetStreamSourceFreq(0, 1));
        SRE_DXCALL(m_dxdevice->SetStreamSourceFreq(1, 1));

        SRE_DXCALL(m_dxdevice->SetVertexDeclaration(m_d2data.dxvertexdecl));
        SRE_DXCALL(m_dxdevice->SetVertexShader(m_dxd2vs));
    }

    if (m_usepot)
    {
        assert(m_potratio.x != 1.0f || m_potratio.y != 1.0f);
        
        for (size_t i = 0; i < point_count; i++)
        {
            const_cast<sre::RenderInstance2*>(instance)->points[i].uv *= m_potratio;
        }
    }

    UINT offsetp = m_d2data.pervertexbuf.append<sre::RenderPoint>(m_dxdevice, instance->points, static_cast<UINT>(point_count));
    UINT offsetc = m_d2data.perinstancebuf.append<sre::col4>(m_dxdevice, &instance->color, 1);
    SRE_DXCALL(m_dxdevice->SetStreamSource(0, m_d2data.pervertexbuf.dxbuff, offsetp, sizeof(sre::RenderPoint)));
    SRE_DXCALL(m_dxdevice->SetStreamSource(1, m_d2data.perinstancebuf.dxbuff, offsetc, 0));

    size_t primcount;
    D3DPRIMITIVETYPE primtype;
    switch (instance->mode)
    {
        case SRE_PRIMITIVE_TRIANGLES:
            primcount = point_count/3;
            primtype = D3DPT_TRIANGLELIST;
            break;
        case SRE_PRIMITIVE_TRIANGLESTRIP:
            primcount = 1 + (point_count-3);
            primtype = D3DPT_TRIANGLESTRIP;
            break;
        case SRE_PRIMITIVE_LINEPERLINE:
            primcount = point_count/2;
            primtype = D3DPT_LINELIST;
            break;
        case SRE_PRIMITIVE_LINESTRIP:
            primcount = 1 + (point_count-2);
            primtype = D3DPT_LINESTRIP;
            break;
        case SRE_PRIMITIVE_POINTS:
            primcount = point_count;
            primtype = D3DPT_POINTLIST;
            break;
        default:
            abort();
    }

    SRE_DXCALL(m_dxdevice->DrawPrimitive(primtype, 0, static_cast<UINT>(primcount)));
}

//

void Instance::begin(const float clear[4])
{
    HRESULT hr;
    if (m_needsetup)
    {
        if (!_statesetup())
            abort();
    }

    SRE_DXCALL(m_dxdevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_COLORVALUE(clear[0], clear[1], clear[2], clear[3]), 0, 0));
    SRE_DXCALL(m_dxdevice->BeginScene());
}

void Instance::end()
{
    HRESULT hr;
    SRE_DXCALL(m_dxdevice->EndScene());
    SRE_DXCALL(m_dxdevice->Present(NULL, NULL, NULL, NULL));
}

//
    
void Instance::set_viewportstate(int w, int h, sre::unit scale)
{
    m_pparamcache.BackBufferWidth = w;
    m_pparamcache.BackBufferHeight = h;
    _resetdevice();

    FLOAT VPMATRIX[] = {
        2.0f/w, 0.0f, 0.0f, 0.0f,
        0.0f, -2.0f/h, 0.0f, 0.0f,
        0.0f, 0.0f, scale, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f
    };
    memcpy(m_viewportcache, VPMATRIX, sizeof(m_viewportcache));
}

static D3DBLEND D3D9_BLENDMODES[][2] = {
    { D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA },
    { D3DBLEND_SRCALPHA, D3DBLEND_ONE },
    { D3DBLEND_DESTCOLOR, D3DBLEND_ZERO },
    { D3DBLEND_DESTCOLOR, D3DBLEND_INVSRCALPHA }
};

void Instance::set_blendstate(sre::blendMode mode)
{
    HRESULT hr;
    if (0)
    {
        SRE_DXCALL(m_dxdevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE));
        return;
    }
    
    SRE_DXCALL(m_dxdevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE));
    SRE_DXCALL(m_dxdevice->SetRenderState(D3DRS_SRCBLEND, D3D9_BLENDMODES[mode][0]));
    SRE_DXCALL(m_dxdevice->SetRenderState(D3DRS_DESTBLEND, D3D9_BLENDMODES[mode][1]));
}

void Instance::set_camerastate(sre::vec2ut camera)
{
    HRESULT hr;
    SRE_DXCALL(m_dxdevice->SetVertexShaderConstantF(4, &camera.x, 1));
}

void Instance::set_scissorstate(const sre::rect2Di* rectangle)
{
    RECT rect = {
        rectangle->position.x,
        rectangle->position.y,
        rectangle->position.x + rectangle->size.x,
        rectangle->position.y + rectangle->size.y
    };

    HRESULT hr;
    SRE_DXCALL(m_dxdevice->SetScissorRect(&rect));
}

void Instance::set_vsync(bool enable)
{
    m_pparamcache.PresentationInterval = enable ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;

    _resetdevice();
}

void Instance::set_texturestate(texture_type* texture)
{
    HRESULT hr;
    SRE_DXCALL(m_dxdevice->SetTexture(0, texture ? texture->dxtexture : m_dxbasictexture));
    if (texture && (texture->potratiox != 1.0f || texture->potratioy != 1.0f))
    {
        m_usepot = true;
        m_potratio.x = texture->potratiox;
        m_potratio.y = texture->potratioy;
    }
    else
    {
        m_usepot = false;
    }
}

//

extern "C" int sregl11_padbypowerof2(int x);
                
bool Instance::texture_setup(Texture* texture, sre::SDLpixelFormat format, int w, int h, sre::SDLpixelFormat* outformat)
{
    HRESULT hr;
    float potratiox = 1.0f;
    float potratioy = 1.0f;

    if (!(m_devcaps.TextureCaps | D3DPTEXTURECAPS_NONPOW2CONDITIONAL))
    {
        int ow = w;
        int oh = h;

        w = sregl11_padbypowerof2(w);
        h = sregl11_padbypowerof2(h);

        potratiox = ow / static_cast<float>(w);
        potratioy = oh / static_cast<float>(h);
    }

    SRE_DXCALLF(m_dxdevice->CreateTexture(w, h, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture->dxtexture, NULL ));
    texture->potratiox = potratiox;
    texture->potratioy = potratioy;
    *outformat = SDL_PIXELFORMAT_BGRA32;
    return true;
}

bool Instance::texture_update(Texture* texture, const sre::rect2Di* region, const void* pixels, int pitch)
{
    HRESULT hr;
    D3DLOCKED_RECT locked;
    RECT rect;
    rect.left = region->position.x;
    rect.top = region->position.y;
    rect.right = region->position.x + region->size.x;
    rect.bottom = region->position.y + region->size.y;

    SRE_DXCALLF(texture->dxtexture->LockRect(0, &locked, &rect, D3DLOCK_NOOVERWRITE));
        if (!pitch || locked.Pitch == pitch)
            memcpy(locked.pBits, pixels, locked.Pitch * region->size.y); // Texture update with identical pitch
        else
        {
            for (int i = 0; i < region->size.y; i++)
                memcpy(static_cast<BYTE*>(locked.pBits) + i*locked.Pitch, static_cast<const BYTE*>(pixels) + i*pitch, pitch);
        }
    SRE_DXCALLF(texture->dxtexture->UnlockRect(0));
    return true;
}

void Instance::texture_destroy(Texture* texture)
{
    texture->dxtexture->Release();
}

//

bool DBuff::init(IDirect3DDevice9* dxdevice, UINT capacity)
{
    HRESULT hr;
    SRE_DXCALLF(dxdevice->CreateVertexBuffer(capacity, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &dxbuff, NULL));
    
    this->pos = 0;
    this->cap = capacity;
    return true;
}

UINT DBuff::append(IDirect3DDevice9* dxdevice, const void* data, UINT size)
{
    HRESULT hr;
    UINT newpos = pos + size;
    void* pdata;
    if (newpos > cap)
    {
        // Wait for the device to finish using the buffer, if it hasn't been?
        //SRE_DXCALLF(dxbuff->Lock(0, 0, &pdata, 0));
        //SRE_DXCALLF(dxbuff->Unlock());
        release();
        if (!init(dxdevice, cap*2 + size))
            return false;
    }

    SRE_DXCALLF(dxbuff->Lock(pos, size, &pdata, D3DLOCK_DISCARD | D3DLOCK_NOOVERWRITE));
        memcpy(pdata, data, size);
    SRE_DXCALLF(dxbuff->Unlock());

    UINT oldpos = pos;
    pos = newpos;
    return oldpos;
}