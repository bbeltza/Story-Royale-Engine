#include "d3d11.hpp"

#include <SDL_syswm.h>
#include <SDL_hints.h>
#include <utils/mem.h>

SRE_EXTERN_C_VAR sre::RenderDriverHelper<sreD3D11::Instance> sred3d11{"Direct3D 11"};

using namespace sreD3D11;

Instance::Instance(SDL_Window* window, int* outstatus)
{
    // Get HWND
    SDL_SysWMinfo wminfo;
    SDL_GetVersion(&wminfo.version);
    if (!SDL_GetWindowWMInfo(window, &wminfo) || wminfo.subsystem != SDL_SYSWM_WINDOWS)
    {
        *outstatus = SRE_RENDERSTATUS_UNSUPPORTED;
        return;
    }

    // Setup
    HRESULT hr;

    { // Device and swapchain setup       

        // Get necessary symbols (We are not linking D3D11.lib)
        SRE_DXGETADDR(D3D11CreateDevice, PFN_D3D11_CREATE_DEVICE, m_dlls.d3d11);
        SRE_DXGETADDR(CreateDXGIFactory, PFN_CREATE_DXGI_FACTORY1, m_dlls.dxgi);
        if (!pD3D11CreateDevice || !pCreateDXGIFactory)
        {
            *outstatus = SRE_RENDERSTATUS_UNSUPPORTED;
            return;
        }

        IDXGIFactory* dxfactory;
        IDXGIAdapter* dxadapter;
        SRE_DXCALLC(pCreateDXGIFactory(IID_PPV_ARGS(&dxfactory)));
        SRE_DXCALLC(dxfactory->EnumAdapters(0, &dxadapter));

        DXGI_ADAPTER_DESC adapterdesc{};
        SRE_DXCALLC(dxadapter->GetDesc(&adapterdesc));
        // sre::log("Description: %ls ; VendorId: %u ; DeviceId: %u ; SubsysId: %u ; Revision: %u ; DedicatedVideoMemory: %zu ; DedicatedSystemMemory: %zu ; SharedSystemMemory: %zu ; AdapterLuid: %p",
        //     adapterdesc.Description, adapterdesc.VendorId, adapterdesc.DeviceId, adapterdesc.SubSysId, adapterdesc.Revision, adapterdesc.DedicatedVideoMemory, adapterdesc.DedicatedSystemMemory, adapterdesc.SharedSystemMemory, adapterdesc.AdapterLuid);

        UINT device_flags = D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT;
        #ifndef NDEBUG
        device_flags |= D3D11_CREATE_DEVICE_DEBUG;
        #endif
        
        D3D_FEATURE_LEVEL feature;
        SRE_DXCALLC(pD3D11CreateDevice(
            dxadapter,
            D3D_DRIVER_TYPE_UNKNOWN,
            NULL,
            device_flags,
            NULL, 0,
            D3D11_SDK_VERSION,
            &m_dxdevice, &feature, &m_dxdevicecontext
        ));

        #if WINVER <= _WIN32_WINNT_WIN10
            #define USE_DXGI_SWAPEFFECT DXGI_SWAP_EFFECT_FLIP_DISCARD
        #elif WINVER <= _WIN32_WINNT_WIN8
            #define USE_DXGI_SWAPEFFECT DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL
        #else
            #define USE_DXGI_SWAPEFFECT static_cast<DXGI_SWAP_EFFECT>(3)
        #endif

        #if WINVER < _WIN32_WINNT_WIN10
            #define DXGI_SWAP_EFFECT_FLIP_DISCARD DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL

        #endif
        
        DXGI_SWAP_CHAIN_DESC swapchain_desc{};
        swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchain_desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swapchain_desc.BufferCount = 2;
        swapchain_desc.SampleDesc.Count = 1;
        swapchain_desc.Windowed = TRUE;
        swapchain_desc.SwapEffect = USE_DXGI_SWAPEFFECT;
        swapchain_desc.OutputWindow = wminfo.info.win.window;

        SRE_DXCALL(dxfactory->CreateSwapChain(m_dxdevice, &swapchain_desc, &m_dxswapchain));
        if (FAILED(hr))
        {
            if (hr != DXGI_ERROR_INVALID_CALL)
                return;

            sre::log(SRE_LOG_INFO "[Direct3D11]: Switching to legacy swapchain...");

            // Double-buffering and thus FLIP swap effects might not be supported, create a legacy single-buffered swap-chain
            swapchain_desc.BufferCount = 1;
            swapchain_desc.Flags = 0;
            swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
            SRE_DXCALLC(dxfactory->CreateSwapChain(m_dxdevice, &swapchain_desc, &m_dxswapchain));
            // m_uselegacy = true;
        }

        // Disable alt-enter automatic fullscreen toggling by DXGI. It switches to non-borderless fullscreen and that is not properly implemented
        IDXGIFactory* parentfactory{};
        SRE_DXCALLC(m_dxswapchain->GetParent(IID_PPV_ARGS(&parentfactory)));
        SRE_DXCALLC(parentfactory->MakeWindowAssociation(wminfo.info.win.window, DXGI_MWA_NO_WINDOW_CHANGES));
        dxadapter->Release();
        dxfactory->Release();
        parentfactory->Release();

        IDXGIDevice1* dxgidevice{};
        SRE_DXCALL(m_dxdevice->QueryInterface(IID_PPV_ARGS(&dxgidevice)));
        if (dxgidevice)
        {
            // This apparently improves performance, by a LOT. It's used in SDL's implementation too
            dxgidevice->SetMaximumFrameLatency(1);
            dxgidevice->Release();
        }
    }
    
    if (!m_shaders.setup(m_dxdevice))
        return;

    {
        D3D11_RASTERIZER_DESC rasterizer_desc{};
        rasterizer_desc.FillMode = D3D11_FILL_SOLID;
        rasterizer_desc.CullMode = D3D11_CULL_NONE;
        rasterizer_desc.ScissorEnable = TRUE;

        SRE_DXCALLC(m_dxdevice->CreateRasterizerState(&rasterizer_desc, &m_dxrasterizerstate));
    }

    {
        D3D11_SAMPLER_DESC sampler_desc{};
        sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

        SRE_DXCALLC(m_dxdevice->CreateSamplerState(&sampler_desc, &m_dxsamplerstate));
    }

    if (!m_d1buffer.init(m_dxdevice, sizeof(sre::RenderInstance1) * 255))
        return;
    if (!m_d2buffer.init(m_dxdevice, sizeof(sre::RenderPoint) * 255))
        return;

    {
        D3D11_BUFFER_DESC cbuffer_desc{};
        cbuffer_desc.ByteWidth = sizeof(CBuffer);
        cbuffer_desc.Usage = D3D11_USAGE_DYNAMIC;
        cbuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbuffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        
        SRE_DXCALLC(m_dxdevice->CreateBuffer(&cbuffer_desc, NULL, &m_cbuffer));

        cbuffer_desc.ByteWidth = sizeof(CCamBuffer);
        SRE_DXCALLC(m_dxdevice->CreateBuffer(&cbuffer_desc, NULL, &m_ccambuffer));
    }

    for (int i = 0; i < 4; i++)
    {
        #define SRE_D3D11_BLEND_DESC(srcFactor, dstFactor, op) { FALSE, FALSE, { {TRUE, srcFactor, dstFactor, op, D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL} } }
        static D3D11_BLEND_DESC BLENDSTATES[5] = {
            /* BLEND */ SRE_D3D11_BLEND_DESC(D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD),
            /* ADD   */ SRE_D3D11_BLEND_DESC(D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD),
            /* MOD   */ SRE_D3D11_BLEND_DESC(D3D11_BLEND_DEST_COLOR, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD),
            /* MUL   */ SRE_D3D11_BLEND_DESC(D3D11_BLEND_DEST_COLOR, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD)
        };

        SRE_DXCALLC(m_dxdevice->CreateBlendState(&BLENDSTATES[i], &m_dxblendstates[i]));
    }

    {
        D3D11_TEXTURE2D_DESC tex_desc{};
        tex_desc.Width = 1;
        tex_desc.Height = 1;
        tex_desc.ArraySize = 1;
		tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.Usage = D3D11_USAGE_IMMUTABLE;
		tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        const UINT WHITE = UINT_MAX;
        D3D11_SUBRESOURCE_DATA subres_data{};
        subres_data.pSysMem = &WHITE;
        subres_data.SysMemPitch = 4;
        subres_data.SysMemSlicePitch = 4;

        ID3D11Texture2D* basictex;
        SRE_DXCALLC(m_dxdevice->CreateTexture2D(&tex_desc, &subres_data, &basictex));

        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
        srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srv_desc.Texture2D.MipLevels = 1;

        SRE_DXCALLC(m_dxdevice->CreateShaderResourceView(basictex, &srv_desc, &m_basictexture));
        basictex->Release();
    }

    m_dxdevicecontext->PSSetSamplers(0, 1, &m_dxsamplerstate);
    m_dxdevicecontext->RSSetState(m_dxrasterizerstate);
    m_dxdevicecontext->PSSetShader(m_shaders.cPS, NULL, 0);
    m_dxdevicecontext->VSSetConstantBuffers(0, 1, &m_cbuffer);
    m_dxdevicecontext->VSSetConstantBuffers(1, 1, &m_ccambuffer);

    *outstatus = SRE_RENDERSTATUS_SUCCEEDED;
}

Instance::~Instance()
{
    m_dxdevice->Release();
    m_dxdevicecontext->Release();
    m_dxswapchain->Release();

    m_dxrendertargetview->Release();

    for (int i = 0; i < _countof(m_dxblendstates); i++)
        m_dxblendstates[i]->Release();
    
    m_cbuffer->Release();
    m_ccambuffer->Release();
    
    m_dxrasterizerstate->Release();
    m_dxsamplerstate->Release();
    m_basictexture->Release();
}