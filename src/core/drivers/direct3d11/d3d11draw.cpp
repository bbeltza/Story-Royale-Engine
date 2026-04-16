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

void Instance::flush_queueinstances1(sre::Sampler* texture, const sre::RenderInstance1* instances, size_t instance_count, sre::u32 flags, sre::u32 switch_flags)
{
    HRESULT hr;

    D3D11_MAPPED_SUBRESOURCE mapped;
    SRE_DXCALL(m_dxdevicecontext->Map(m_d1buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));
    memcpy(mapped.pData, instances, sizeof(*instances)*instance_count);
    m_dxdevicecontext->Unmap(m_d1buffer, 0);

    if (switch_flags & SRE_RENDER_SWITCHTYPE)
    {
        switch_flags |= SRE_RENDER_SWITCHTEXTURE | SRE_RENDER_SWITCHCAMERA; // Switch everything for now since draw2 doesn't handle the switching state yet

        UINT offsets[] = { 0 };
        UINT strides[] = { sizeof(sre::RenderInstance1) };
        m_dxdevicecontext->IASetVertexBuffers(0, 1, &m_d1buffer, strides, offsets);
    }

    if (switch_flags & SRE_RENDER_SWITCHCAMERA)
        m_dxdevicecontext->VSSetConstantBuffers(0, 1, m_cbuffers + ((flags & SRE_DRAWFLAG_CAMERA) != 0));

    if (switch_flags & SRE_RENDER_SWITCHTEXTURE)
    {
        m_dxdevicecontext->PSSetShaderResources(0, 1, texture ? &texture->dxsrv : &m_basictexture);
    }

    m_dxdevicecontext->DrawInstanced(4, static_cast<UINT>(instance_count), 0, 0);
}

void Instance::flush_queueinstances2(const sre::RenderInstance2& instance, size_t point_count, sre::u32 flags, sre::u32 switch_flags)
{

}