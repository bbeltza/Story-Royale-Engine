#include "d3d11.h"

#include <SDL_syswm.h>

extern "C" sre::RenderDriverData sre_d3d11 = {
    [](SDL_Window* window) -> sre::RenderInterface* { return new sred3d11::Interface(window); },
    [](sre::RenderInterface* renderer) { delete static_cast<sred3d11::Interface*>(renderer); },
    0 // sizeof(sre::Sampler) -> struct sre::Sampler { ... } in the "d3d11.h" header
};

using namespace sred3d11;

Interface::Interface(SDL_Window* window)
{
    // Get HWND
    SDL_SysWMinfo swm_info;
    SDL_GetVersion(&swm_info.version);
    if (!SDL_GetWindowWMInfo(window, &swm_info))
        return;

    // Get necessary symbols (We are not linking D3D11.lib)
    HMODULE d3d11dll = LoadLibraryA("D3D11.dll");
    auto pD3D11CreateDeviceAndSwapChain = reinterpret_cast<PFN_D3D11_CREATE_DEVICE_AND_SWAP_CHAIN>(GetProcAddress(d3d11dll, "D3D11CreateDeviceAndSwapChain"));
    FreeLibrary(d3d11dll);

    // Setup
    HRESULT hr{};
    ID3D11Device* dxdevice;
    ID3D11DeviceContext* dxdevicecontext;
    IDXGISwapChain1* dxswapchain;

    DXGI_SWAP_CHAIN_DESC swapchain_desc{};
    swapchain_desc.BufferCount = 2;
    swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchain_desc.SampleDesc.Count = 1;
    swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapchain_desc.OutputWindow = swm_info.info.win.window;

    pD3D11CreateDeviceAndSwapChain(
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        #ifndef NDEBUG
            D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_DEBUGGABLE |
        #endif
        0,
        NULL, 0,
        D3D11_SDK_VERSION,
        &swapchain_desc, reinterpret_cast<IDXGISwapChain**>(&dxswapchain),
        &dxdevice, NULL, &dxdevicecontext
    );
}