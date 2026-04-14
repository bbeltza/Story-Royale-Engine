#include "d3d11.h"

using namespace sreD3D11;

void Instance::present()
{
    HRESULT hr;
    
    SRE_DXCALL(m_dxswapchain->Present(0, 0));
}

bool Instance::clear(float color[3])
{
    m_dxdevicecontext->ClearRenderTargetView(m_dxrendertargetview, color);
    return true;
}