#define D3D_DEBUG_INFO

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

    D3DPRESENT_PARAMETERS present_params{};
    present_params.hDeviceWindow = wminfo.info.win.window;
    present_params.BackBufferFormat = D3DFMT_A8R8G8B8;
    present_params.BackBufferCount = 2;
    present_params.SwapEffect = D3DSWAPEFFECT_FLIP;
    present_params.Windowed = TRUE;
    present_params.Flags = D3DPRESENTFLAG_VIDEO;
    present_params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

    SRE_DX9CALL(m_dxd3d9->CreateDevice(D3DADAPTER_DEFAULT,
                                   D3DDEVTYPE_HAL,
                                   NULL,
                                   D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_NOWINDOWCHANGES,
                                   &present_params,
                                   &m_dxdevice
    ));

    if (!_shadersetup())
        CONSTRUCTOR_FAIL;

    m_hwnd = wminfo.info.win.window;
}

Instance::~Instance()
{
    m_dxd3d9->Release();
    m_dxdevice->Release();

    m_dxcps->Release();
    m_dxd1vs->Release();
    m_dxd2vs->Release();
}

bool Instance::_statesetup()
{
    HRESULT hr;

    static const D3DVERTEXELEMENT9 D1_DECLARATION[] = {
        {0, offsetof(sre::RenderInstance1, rectangle), D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        {0, offsetof(sre::RenderInstance1, anchor), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 1 },
        {0, offsetof(sre::RenderInstance1, anchor), D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        {0, offsetof(sre::RenderInstance1, angle), D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_PSIZE, 0 },
        {0, offsetof(sre::RenderInstance1, uv), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        {0, offsetof(sre::RenderInstance1, uv_offset), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
        {1, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 2 },
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

    SRE_DX9CALL(m_dxdevice->CreateVertexBuffer(sizeof(FLOAT[2*4]), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_d1data.dxbuff_vert, NULL));
    SRE_DX9CALL(m_dxdevice->CreateVertexBuffer(sizeof(sre::RenderInstance1)*255, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_d1data.dxbuff_inst, NULL));
    
    SRE_DX9CALL(m_dxdevice->CreateVertexBuffer(sizeof(sre::RenderPoint)*255, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_d2data.dxbuff_vert, NULL));
    SRE_DX9CALL(m_dxdevice->CreateVertexBuffer(sizeof(sre::col4), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_d2data.dxbuff_inst, NULL));

    {
        FLOAT* mapped_vertices;
        SRE_DX9CALL(m_d1data.dxbuff_vert->Lock(0, sizeof(FLOAT[2*4]), reinterpret_cast<void**>(&mapped_vertices), D3DLOCK_NOOVERWRITE));
            mapped_vertices[0] = 0.0f;
            mapped_vertices[1] = 0.0f;

            mapped_vertices[2] = 1.0f;
            mapped_vertices[3] = 0.0f;

            mapped_vertices[4] = 0.0f;
            mapped_vertices[5] = 1.0f;

            mapped_vertices[6] = 1.0f;
            mapped_vertices[7] = 1.0f;
        SRE_DX9CALL(m_d1data.dxbuff_vert->Unlock());
    }

    return true;
}

//

void Instance::flush_queueinstances1(Texture* texture, const sre::RenderInstance1* instances, size_t instance_count, sre::u32 flags, sre::u32 switch_flags)
{

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

    SRE_DXCALL(m_dxdevice->BeginScene());
    if (switch_flags & SRE_RENDER_SWITCHTYPE)
    {
        SRE_DXCALL(m_dxdevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | point_count));
        SRE_DXCALL(m_dxdevice->SetStreamSource(0, m_d2data.dxbuff_vert, 0, sizeof(sre::RenderPoint)));
        
        SRE_DXCALL(m_dxdevice->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1));
        SRE_DXCALL(m_dxdevice->SetStreamSource(1, m_d2data.dxbuff_inst, 0, sizeof(sre::col4)));

        SRE_DXCALL(m_dxdevice->SetVertexDeclaration(m_d2data.dxdecl));
        SRE_DXCALL(m_dxdevice->SetVertexShader(m_dxd2vs));
        SRE_DXCALL(m_dxdevice->SetPixelShader(m_dxcps));
    }

    SRE_DXCALL(m_dxdevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, point_count/3));
    SRE_DXCALL(m_dxdevice->EndScene());
}

//
            
void Instance::present()
{
    HRESULT hr;
    SRE_DXCALL(m_dxdevice->Present(NULL, NULL, NULL, NULL));
}

bool Instance::clear(float color[3])
{
    HRESULT hr;
    SRE_DXCALLF(m_dxdevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_COLORVALUE(color[0], color[1], color[2], 1), 0, 0));
    return true;
}

//
    
bool Instance::set_viewportstate(int w, int h, sre::unit scale)
{
    HRESULT hr;
    D3DVIEWPORT9 vp{};
    vp.Width = w;
    vp.Height = h;

    D3DPRESENT_PARAMETERS present_params{};
    present_params.hDeviceWindow = m_hwnd;
    present_params.BackBufferFormat = D3DFMT_UNKNOWN;
    present_params.BackBufferCount = 2;
    present_params.SwapEffect = D3DSWAPEFFECT_FLIP;
    present_params.Windowed = TRUE;
    present_params.Flags = D3DPRESENTFLAG_VIDEO;
    present_params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

    SRE_DXCALLF(m_dxdevice->Reset(&present_params));
    return _statesetup();
}

bool Instance::set_blendstate(sre::blendMode blending)
{
    return false;
}

bool Instance::set_camerastate(sre::unit x, sre::unit y)
{
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

    //SRE_DXCALLF(m_dxdevice->CreateTexture(w, h, 1, D3DUSAGE_WRITEONLY, ));
    *outformat = SDL_PIXELFORMAT_RGBA32;
    return true;
}

bool Instance::texture_update(Texture* texture, const void* pixels, int pitch)
{
    return true;
}

void Instance::texture_destroy(Texture* texture)
{
    //texture->dxtexture->Release();
}
