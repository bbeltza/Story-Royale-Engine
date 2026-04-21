#ifndef NDEBUG
    #define D3D_DEBUG_INFO
#endif

#include "d3d9.hpp"
#include <SDL_syswm.h>

extern "C" sre::RenderDriverHelper<sreD3D9::Instance> sred3d9{};

using namespace sreD3D9;

#define CONSTRUCTOR_FAIL { m_success = false; return; } do; while(0)

Instance::Instance(SDL_Window* window)
{
    SDL_SysWMinfo wminfo;
    SDL_GetVersion(&wminfo.version);
    if (!SDL_GetWindowWMInfo(window, &wminfo) || wminfo.subsystem != SDL_SYSWM_WINDOWS)
        CONSTRUCTOR_FAIL;

    HRESULT hr;
    m_dxd3d9 = Direct3DCreate9(D3D_SDK_VERSION);
    if (!m_dxd3d9)
        CONSTRUCTOR_FAIL;

    m_pparamcache.hDeviceWindow = wminfo.info.win.window;
    m_pparamcache.BackBufferFormat = D3DFMT_UNKNOWN;
    m_pparamcache.BackBufferCount = 2;
    m_pparamcache.SwapEffect = D3DSWAPEFFECT_FLIP;
    m_pparamcache.Windowed = TRUE;
    m_pparamcache.Flags = D3DPRESENTFLAG_VIDEO;
    m_pparamcache.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    SRE_DX9CALL(m_dxd3d9->CreateDevice(D3DADAPTER_DEFAULT,
                                   D3DDEVTYPE_HAL,
                                   NULL,
                                   D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_NOWINDOWCHANGES | D3DCREATE_FPU_PRESERVE,
                                   &m_pparamcache,
                                   &m_dxdevice
    ));

    if (!_shadersetup())
        CONSTRUCTOR_FAIL;

    D3DLOCKED_RECT rect;
    SRE_DX9CALL(m_dxdevice->CreateTexture(1, 1, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &m_dxbasictexture, NULL));
    SRE_DX9CALL(m_dxbasictexture->LockRect(0, &rect, NULL, D3DLOCK_DISCARD));
        *static_cast<DWORD*>(rect.pBits) = 0xFFFFFFFF;
    SRE_DX9CALL(m_dxbasictexture->UnlockRect(0));
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
        {0, 0, D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        {1, offsetof(sre::RenderPoint, pos), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        {1, offsetof(sre::RenderPoint, uv), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END()
    };

    SRE_DXCALLF(m_dxdevice->CreateVertexDeclaration(D1_DECLARATION, &m_d1data.dxdecl));
    SRE_DXCALLF(m_dxdevice->CreateVertexDeclaration(D2_DECLARATION, &m_d2data.dxdecl));

    SRE_DXCALLF(m_dxdevice->CreateVertexBuffer(sizeof(FLOAT[2*4]), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_d1data.dxbuff_vert, NULL));
    SRE_DXCALLF(m_dxdevice->CreateVertexBuffer(sizeof(sre::RenderInstance1)*255, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_d1data.dxbuff_inst, NULL));
    
    SRE_DXCALLF(m_dxdevice->CreateVertexBuffer(sizeof(sre::RenderPoint)*255, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_d2data.dxbuff_vert, NULL));
    SRE_DXCALLF(m_dxdevice->CreateVertexBuffer(sizeof(sre::col4), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_d2data.dxbuff_inst, NULL));

    SRE_DXCALLF(m_dxdevice->CreateIndexBuffer(sizeof(USHORT)*6, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_d1data.dxibuff, NULL));

    {
        FLOAT* mapped_vertices = NULL;
        USHORT* mapped_indices = NULL;
        SRE_DXCALLF(m_d1data.dxbuff_vert->Lock(0, 0, reinterpret_cast<void**>(&mapped_vertices), D3DLOCK_NOOVERWRITE));
            mapped_vertices[0] = 0.0f;
            mapped_vertices[1] = 0.0f;

            mapped_vertices[2] = 1.0f;
            mapped_vertices[3] = 0.0f;

            mapped_vertices[4] = 0.0f;
            mapped_vertices[5] = 1.0f;

            mapped_vertices[6] = 1.0f;
            mapped_vertices[7] = 1.0f;
        SRE_DXCALLF(m_d1data.dxbuff_vert->Unlock());
        SRE_DXCALLF(m_d1data.dxibuff->Lock(0, 0, reinterpret_cast<void**>(&mapped_indices), D3DLOCK_NOOVERWRITE));
            mapped_indices[0] = 0;
            mapped_indices[1] = 1;
            mapped_indices[2] = 2;
            mapped_indices[3] = 1;
            mapped_indices[4] = 2;
            mapped_indices[5] = 3;
        SRE_DXCALLF(m_d1data.dxibuff->Unlock());
    }

    SRE_DXCALLF(m_dxdevice->SetPixelShader(m_dxcps));
    SRE_DXCALLF(m_dxdevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
    SRE_DXCALLF(m_dxdevice->SetRenderState(D3DRS_LIGHTING, FALSE));

    SRE_DXCALLF(m_dxdevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP));
    SRE_DXCALLF(m_dxdevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP));
    SRE_DXCALLF(m_dxdevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT));
    SRE_DXCALLF(m_dxdevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT));

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

void Instance::flush_queueinstances1(Texture* texture, const sre::RenderInstance1* instances, size_t instance_count, sre::u32 flags, sre::u32 switch_flags)
{
    HRESULT hr;
    if (switch_flags & SRE_RENDER_SWITCHTYPE)
    {
        switch_flags |= SRE_RENDER_SWITCHCAMERA | SRE_RENDER_SWITCHTEXTURE;

        SRE_DXCALL(m_dxdevice->SetVertexDeclaration(m_d1data.dxdecl));
        SRE_DXCALL(m_dxdevice->SetVertexShader(m_dxd1vs));
        SRE_DXCALL(m_dxdevice->SetIndices(m_d1data.dxibuff));
        SRE_DXCALL(m_dxdevice->SetStreamSource(0, m_d1data.dxbuff_vert, 0, sizeof(FLOAT[2])));
        SRE_DXCALL(m_dxdevice->SetStreamSource(1, m_d1data.dxbuff_inst, 0, sizeof(sre::RenderInstance1)));
        SRE_DXCALL(m_dxdevice->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1));
    }

    if (switch_flags & SRE_RENDER_SWITCHCAMERA)
    {
        sre::vec2f camera = flags & SRE_DRAWFLAG_CAMERA ? sre::vec2f{m_cameracache[0], m_cameracache[1]} : sre::vec2f::ZERO;
        SRE_DXCALL(m_dxdevice->SetVertexShaderConstantF(4, &camera.x, 1));
    }

    if (switch_flags & SRE_RENDER_SWITCHTEXTURE)
    {
        SRE_DXCALL(m_dxdevice->SetTexture(0, texture ? texture->dxtexture : m_dxbasictexture));
    }

    void* mapped;
    SRE_DXCALL(m_d1data.dxbuff_inst->Lock(0, sizeof(sre::RenderInstance1)*instance_count, &mapped, D3DLOCK_DISCARD));
        memcpy(mapped, instances, sizeof(sre::RenderInstance1)*instance_count);
    SRE_DXCALL(m_d1data.dxbuff_inst->Unlock());

    SRE_DXCALL(m_dxdevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | instance_count));
    SRE_DXCALL(m_dxdevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2*instance_count));
}

void Instance::flush_queueinstances2(Texture* texture, const sre::RenderInstance2* instance, size_t point_count, sre::u32 flags, sre::u32 switch_flags)
{
    HRESULT hr;
    void* mapped_points;
    void* mapped_color;
    // For now we will just use D3DLOCK_DISCARD, I'll be looking for D3DLOCK_NOOVERWRITE
    SRE_DXCALL(m_d2data.dxbuff_vert->Lock(0, static_cast<UINT>(sizeof(instance->points[0])*point_count), &mapped_points, D3DLOCK_DISCARD));
    SRE_DXCALL(m_d2data.dxbuff_inst->Lock(0, static_cast<UINT>(sizeof(instance->color)), &mapped_color, D3DLOCK_DISCARD));
    memcpy(mapped_color, &instance->color, sizeof(instance->color));
    memcpy(mapped_points, instance->points, sizeof(instance->points[0])*point_count);
    SRE_DXCALL(m_d2data.dxbuff_vert->Unlock());
    SRE_DXCALL(m_d2data.dxbuff_inst->Unlock());

    if (switch_flags & SRE_RENDER_SWITCHTYPE)
    {
        SRE_DXCALL(m_dxdevice->SetStreamSourceFreq(0, 1));
        SRE_DXCALL(m_dxdevice->SetStreamSourceFreq(1, 1));
        
        SRE_DXCALL(m_dxdevice->SetStreamSource(0, m_d2data.dxbuff_vert, 0, sizeof(sre::RenderPoint)));
        SRE_DXCALL(m_dxdevice->SetStreamSource(1, m_d2data.dxbuff_inst, 0, 0));

        SRE_DXCALL(m_dxdevice->SetVertexDeclaration(m_d2data.dxdecl));
        SRE_DXCALL(m_dxdevice->SetVertexShader(m_dxd2vs));
    }

    SRE_DXCALL(m_dxdevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, point_count/3));
}

//
            
void Instance::present()
{
    HRESULT hr;
    SRE_DXCALL(m_dxdevice->EndScene());
    SRE_DXCALL(m_dxdevice->Present(NULL, NULL, NULL, NULL));
}

bool Instance::clear(float color[3])
{
    HRESULT hr;
    if (m_needsetup)
    {
        if (!_statesetup())
            abort();
    }

    SRE_DXCALLF(m_dxdevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_COLORVALUE(color[0], color[1], color[2], 1), 0, 0));
    SRE_DXCALLF(m_dxdevice->BeginScene());
    return true;
}

//
    
bool Instance::set_viewportstate(int w, int h, sre::unit scale)
{
    m_pparamcache.BackBufferWidth = w;
    m_pparamcache.BackBufferHeight = h;
    _resetdevice();

    HRESULT hr;
    float VPMATRIX[] = {
        2.0f/w, 0.0f, 0.0f, 0.0f,
        0.0f, -2.0f/h, 0.0f, 0.0f,
        0.0f, 0.0f, scale, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f
    };
    SRE_DXCALLF(m_dxdevice->SetVertexShaderConstantF(0, VPMATRIX, 4));
    return true;
}

static D3DBLEND D3D9_BLENDMODES[][2] = {
    { D3DBLEND_ONE, D3DBLEND_ZERO }, // Not used, just some padding to index with `blending` properly. BLEND_NONE will disable blending completely
    { D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA },
    { D3DBLEND_SRCALPHA, D3DBLEND_ONE },
    { D3DBLEND_DESTCOLOR, D3DBLEND_ZERO },
    { D3DBLEND_DESTCOLOR, D3DBLEND_INVSRCALPHA }
};

bool Instance::set_blendstate(sre::blendMode blending)
{
    HRESULT hr;
    if (blending == SRE_BLEND_NONE)
    {
        SRE_DXCALLF(m_dxdevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE));
        return true;
    }
    //SRE_DXCALLF(m_dxdevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD));
    
    SRE_DXCALLF(m_dxdevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE));
    SRE_DXCALLF(m_dxdevice->SetRenderState(D3DRS_SRCBLEND, D3D9_BLENDMODES[blending][0]));
    SRE_DXCALLF(m_dxdevice->SetRenderState(D3DRS_DESTBLEND, D3D9_BLENDMODES[blending][1]));
    return true;
}

bool Instance::set_camerastate(sre::unit x, sre::unit y)
{
    m_cameracache[0] = x;
    m_cameracache[1] = y;
    return true;
}

void Instance::set_clipstate(const sre::rect2Di* rectangle)
{

}

void Instance::set_vsync(bool enable)
{

}

//
                
bool Instance::texture_setup(Texture* texture, sre::pixelFormat format, int w, int h, sre::pixelFormat* outformat)
{
    HRESULT hr;
    SRE_DXCALLF(m_dxdevice->CreateTexture(w, h, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture->dxtexture, NULL ));
    texture->height_cache = h;
    *outformat = SDL_PIXELFORMAT_BGRA32;
    return true;
}

bool Instance::texture_update(Texture* texture, const void* pixels, int pitch)
{
    HRESULT hr;
    D3DLOCKED_RECT rect;
    SRE_DXCALLF(texture->dxtexture->LockRect(0, &rect, NULL, D3DLOCK_DISCARD));
        memcpy(rect.pBits, pixels, rect.Pitch * texture->height_cache); // Texture update with identical pitch
    SRE_DXCALLF(texture->dxtexture->UnlockRect(0));
    return true;
}

void Instance::texture_destroy(Texture* texture)
{
    texture->dxtexture->Release();
}
