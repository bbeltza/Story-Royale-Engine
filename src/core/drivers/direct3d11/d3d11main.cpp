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

    {   // Setup render target view (only 1 buffer... Is there double-buffering already...?)
        ID3D11Resource* rtres{};
        SRE_DX11CALL(m_dxswapchain->GetBuffer(0, IID_PPV_ARGS(&rtres)));
        SRE_DX11CALL(m_dxdevice->CreateRenderTargetView(rtres, NULL, &m_dxrendertargetview));
        rtres->Release();
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
        buffer_desc.StructureByteStride = sizeof(sre::RenderInstance1);
        
        SRE_DX11CALL(m_dxdevice->CreateBuffer(&buffer_desc, NULL, &m_d1buffer));
    }

    m_dxdevicecontext->PSSetSamplers(0, 1, &m_dxsamplerstate);
    m_dxdevicecontext->PSSetShader(m_shaders.cPS, NULL, 0);
    m_dxdevicecontext->VSSetShader(m_shaders.d1VS, NULL, 0);
    m_dxdevicecontext->RSSetState(m_dxrasterizerstate);
    m_dxdevicecontext->IASetInputLayout(m_shaders.d1IL);
    m_dxdevicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    m_dxdevicecontext->OMSetRenderTargets(1, &m_dxrendertargetview, NULL);
}

Instance::~Instance()
{
    m_dxdevice->Release();
    m_dxdevicecontext->Release();
    m_dxswapchain->Release();

    m_dxrendertargetview->Release();
}