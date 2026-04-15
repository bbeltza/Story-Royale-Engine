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

void Instance::flush_queueinstances1(sre::Sampler* const* inst_textures, const sre::RenderInstance1* instances, size_t instance_count, sre::u32 flags)
{
    HRESULT hr;
    UINT offs = 0;
    UINT stride = sizeof(*instances);

    D3D11_MAPPED_SUBRESOURCE mapped;
    SRE_DXCALL(m_dxdevicecontext->Map(m_d1buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));
    memcpy(mapped.pData, instances, sizeof(*instances)*instance_count);
    m_dxdevicecontext->Unmap(m_d1buffer, 0);

    m_dxdevicecontext->VSSetConstantBuffers(0, 1, m_cbuffers + ((flags & SRE_DRAWFLAG_CAMERA) != 0));
    m_dxdevicecontext->IASetVertexBuffers(0, 1, &m_d1buffer, &stride, &offs);
    m_dxdevicecontext->DrawInstanced(4, static_cast<UINT>(instance_count), 0, 0);
}