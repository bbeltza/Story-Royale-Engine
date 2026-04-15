#include "d3d11.h"

#include <SDL_syswm.h>
#include <utils/mem.h>

extern "C" sre::RenderDriverData sred3d11 = {
    [](SDL_Window* window) -> sre::RenderInterface* { auto inst = new sreD3D11::Instance(window); if (inst->successful()) return inst; delete inst; return NULL; },
    [](sre::RenderInterface* renderer) { delete static_cast<sreD3D11::Instance*>(renderer); },
    sizeof(sre::Sampler) //-> struct sre_Sampler { ... } in the "d3d11.h" header
};

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
        HMODULE d3d11dll = LoadLibraryA("D3D11.dll");
        auto pD3D11CreateDeviceAndSwapChain = reinterpret_cast<PFN_D3D11_CREATE_DEVICE_AND_SWAP_CHAIN>(GetProcAddress(d3d11dll, "D3D11CreateDeviceAndSwapChain"));
        if (!pD3D11CreateDeviceAndSwapChain)
        {
            m_success = false;
            return;
        }

        DXGI_SWAP_CHAIN_DESC swapchain_desc{};
        swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapchain_desc.BufferCount = 1;
        swapchain_desc.SampleDesc.Count = 1;
        swapchain_desc.Windowed = TRUE;
        //swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        swapchain_desc.OutputWindow = swm_info.info.win.window;

        #ifndef NDEBUG
            #define _DBGFLAGS D3D11_CREATE_DEVICE_DEBUG
        #else
            #define _DBGFLAGS 0
        #endif

        IDXGISwapChain* dxswapchain;
        SRE_DX11CALL(pD3D11CreateDeviceAndSwapChain(
            NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
            _DBGFLAGS |
            D3D11_CREATE_DEVICE_SINGLETHREADED,
            NULL, 0,
            D3D11_SDK_VERSION,
            &swapchain_desc, &dxswapchain,
            &m_dxdevice, NULL, &m_dxdevicecontext
        ));
        FreeLibrary(d3d11dll);

        SRE_DX11CALL(dxswapchain->QueryInterface(&m_dxswapchain));
        dxswapchain->Release();
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
        sampler_desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
        sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

        SRE_DX11CALL(m_dxdevice->CreateSamplerState(&sampler_desc, &m_dxsamplerstate));
    }

    {
        D3D11_BUFFER_DESC buffer_desc{};
        buffer_desc.ByteWidth = sizeof(sre::RenderInstance1) * 256;
        buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
        buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        SRE_DX11CALL(m_dxdevice->CreateBuffer(&buffer_desc, NULL, &m_d1buffer));
    }

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
        static D3D11_BLEND_DESC BLENDSTATES[5] = {
            { FALSE, FALSE, { {FALSE} } },
            { FALSE, FALSE, { {TRUE, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD, D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL} } }
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
    }

    m_dxdevicecontext->PSSetSamplers(0, 1, &m_dxsamplerstate);
    m_dxdevicecontext->PSSetShader(m_shaders.cPS, NULL, 0);
    m_dxdevicecontext->VSSetShader(m_shaders.d1VS, NULL, 0);
    m_dxdevicecontext->RSSetState(m_dxrasterizerstate);
    m_dxdevicecontext->IASetInputLayout(m_shaders.d1IL);
    m_dxdevicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

Instance::~Instance()
{
    m_dxdevice->Release();
    m_dxdevicecontext->Release();
    m_dxswapchain->Release();

    m_dxrendertargetview->Release();
}