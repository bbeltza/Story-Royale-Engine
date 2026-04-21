#include "d3d11.h"

#include <SDL_syswm.h>
#include <SDL_hints.h>
#include <utils/mem.h>

extern "C" sre::RenderDriverHelper<sreD3D11::Instance> sred3d11{};

using namespace sreD3D11;

Instance::Instance(SDL_Window* window)
{
    // Get HWND
    SDL_SysWMinfo swm_info;
    SDL_GetVersion(&swm_info.version);
    if (!SDL_GetWindowWMInfo(window, &swm_info))
        return;
    
    // Setup
    HRESULT hr;

    { // Device and swapchain setup
        // Get necessary symbols (We are not linking D3D11.lib)
        SRE_DXGETADDR(D3D11CreateDevice, PFN_D3D11_CREATE_DEVICE, m_dlls.d3d11);
        SRE_DXGETADDR(CreateDXGIFactory1, PFN_CREATE_DXGI_FACTORY1, m_dlls.dxgi);
        if (!pD3D11CreateDevice || !pCreateDXGIFactory1)
        {
            m_success = false;
            return;
        }

        UINT device_flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
        #ifndef NDEBUG
            device_flags |= D3D11_CREATE_DEVICE_DEBUG;
        #endif

        SRE_DX11CALL(pD3D11CreateDevice(
            NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
            device_flags,
            NULL, 0,
            D3D11_SDK_VERSION,
            &m_dxdevice, NULL, &m_dxdevicecontext
        ));

        IDXGIFactory1* dxfactory;
        SRE_DX11CALL(pCreateDXGIFactory1(IID_PPV_ARGS(&dxfactory)));
        
        DXGI_SWAP_CHAIN_DESC swapchain_desc{};
        swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapchain_desc.BufferCount = 2;
        swapchain_desc.SampleDesc.Count = 1;
        swapchain_desc.Windowed = TRUE;
        swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapchain_desc.OutputWindow = swm_info.info.win.window;
        swapchain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

        SRE_DXCALL(dxfactory->CreateSwapChain(m_dxdevice, &swapchain_desc, &m_dxswapchain));
        if (FAILED(hr))
        {
            if (hr != DXGI_ERROR_INVALID_CALL)
            {
                m_success = false;
                return;
            }

            sre::log<sre::LOGCATEGORY_INFO>("[Direct3D11]: Switching to legacy swapchain");

            // Double-buffering might not be supported, create a legacy single-buffered swap-chain
            swapchain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
            swapchain_desc.Flags = 0;
            swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
            SRE_DX11CALL(dxfactory->CreateSwapChain(m_dxdevice, &swapchain_desc, &m_dxswapchain));
            m_uselegacy = true;
        }

        // Disable alt-enter automatic fullscreen toggling by DXGI. It switches to non-borderless fullscreen and that is not properly implemented

        IDXGIFactory1* parentfactory{};
        SRE_DXCALL(m_dxswapchain->GetParent(IID_PPV_ARGS(&parentfactory)));
        SRE_DXCALL(parentfactory->MakeWindowAssociation(swm_info.info.win.window, DXGI_MWA_NO_WINDOW_CHANGES));
        dxfactory->Release();
        parentfactory->Release();
    }

    m_success &= m_shaders.setup(m_dxdevice);

    {
        D3D11_RASTERIZER_DESC rasterizer_desc{};
        rasterizer_desc.FillMode = D3D11_FILL_SOLID;
        rasterizer_desc.CullMode = D3D11_CULL_NONE;
        rasterizer_desc.ScissorEnable = TRUE;

        SRE_DX11CALL(m_dxdevice->CreateRasterizerState(&rasterizer_desc, &m_dxrasterizerstate));
    }

    {
        D3D11_SAMPLER_DESC sampler_desc{};
        sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

        SRE_DX11CALL(m_dxdevice->CreateSamplerState(&sampler_desc, &m_dxsamplerstate));
    }

    m_success &= m_d1buffer.init(m_dxdevice, sizeof(sre::RenderInstance1) * 256);
    m_success &= m_d2bufferc.init(m_dxdevice, sizeof(sre::col4) * 256);
    m_success &= m_d2bufferp.init(m_dxdevice, sizeof(sre::vec2ut) * 256);
    {
        D3D11_BUFFER_DESC cbuffer_desc{};
        cbuffer_desc.ByteWidth = sizeof(CBuffer);
        cbuffer_desc.Usage = D3D11_USAGE_DYNAMIC;
        cbuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbuffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        for (int i = 0; i < 2; i++) {
            SRE_DX11CALL(m_dxdevice->CreateBuffer(&cbuffer_desc, NULL, &m_cbuffers[i]));
        }
    }

    for (int i = 0; i < 5; i++)
    {
        #define SRE_D3D11_BLEND_DESC(srcFactor, dstFactor, op) { FALSE, FALSE, { {TRUE, srcFactor, dstFactor, op, D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL} } }
        static D3D11_BLEND_DESC BLENDSTATES[5] = {
            { FALSE, FALSE, { {FALSE} } }, // TODO: None blend mode SHOULD be removed
            /* BLEND */ SRE_D3D11_BLEND_DESC(D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD),
            /* ADD   */ SRE_D3D11_BLEND_DESC(D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD),
            /* MOD   */ SRE_D3D11_BLEND_DESC(D3D11_BLEND_DEST_COLOR, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD),
            /* MUL   */ SRE_D3D11_BLEND_DESC(D3D11_BLEND_DEST_COLOR, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD)
        };

        SRE_DX11CALL(m_dxdevice->CreateBlendState(&BLENDSTATES[i], &m_dxblendstates[i]));
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
        SRE_DX11CALL(m_dxdevice->CreateTexture2D(&tex_desc, &subres_data, &basictex));

        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
        srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srv_desc.Texture2D.MipLevels = 1;

        SRE_DX11CALL(m_dxdevice->CreateShaderResourceView(basictex, &srv_desc, &m_basictexture));
        basictex->Release();
    }

    m_dxdevicecontext->PSSetSamplers(0, 1, &m_dxsamplerstate);
    m_dxdevicecontext->RSSetState(m_dxrasterizerstate);
    m_dxdevicecontext->PSSetShader(m_shaders.cPS, NULL, 0);
}

Instance::~Instance()
{
    m_dxdevice->Release();
    m_dxdevicecontext->Release();
    m_dxswapchain->Release();

    m_dxrendertargetview->Release();

    for (int i = 0; i < _countof(m_dxblendstates); i++)
        m_dxblendstates[i]->Release();
    
    for (int i = 0; i < _countof(m_cbuffers); i++)
        m_cbuffers[i]->Release();
    
    m_dxrasterizerstate->Release();
    m_dxsamplerstate->Release();
    m_basictexture->Release();
}