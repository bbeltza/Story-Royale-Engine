#include "d3d11.h"

using namespace sreD3D11;

void Instance::present()
{
    HRESULT hr;
    
    SRE_DXCALL(m_dxswapchain->Present(m_caches.vsync, 0));
}

bool Instance::clear(float color[3])
{
    const FLOAT color4[4] = {
        color[0], color[1], color[2], 1.0f
    };

	m_dxdevicecontext->OMSetRenderTargets(1, &m_dxrendertargetview, NULL);
    m_dxdevicecontext->ClearRenderTargetView(m_dxrendertargetview, color4);
    m_d1buffer.reset();
    m_d2buffer.reset();

    return true;
}

void Instance::flush_queueinstances1(sre::Sampler* texture, const sre::RenderInstance1* instances, size_t instance_count, sre::u32 flags, sre::u32 switch_flags)
{
    UINT inst_num = m_d1buffer.index / sizeof(sre::RenderInstance1);
    if (m_d1buffer.append(m_dxdevicecontext, instances, sizeof(*instances)*instance_count))
    {
        switch_flags |= SRE_RENDER_SWITCHTYPE;    
        inst_num = 0;
    }

    if (switch_flags & SRE_RENDER_SWITCHTYPE)
    {
        switch_flags |= SRE_RENDER_SWITCHTEXTURE | SRE_RENDER_SWITCHCAMERA; // Switch everything for now since draw2 doesn't handle the switching state yet

        UINT offsets[] = { 0 };
        UINT strides[] = { sizeof(sre::RenderInstance1) };
        m_dxdevicecontext->IASetVertexBuffers(0, 1, &m_d1buffer.dxbuffer, strides, offsets);
    }

    if (switch_flags & SRE_RENDER_SWITCHCAMERA)
        m_dxdevicecontext->VSSetConstantBuffers(0, 1, m_cbuffers + ((flags & SRE_DRAWFLAG_CAMERA) != 0));

    if (switch_flags & SRE_RENDER_SWITCHTEXTURE)
    {
        m_dxdevicecontext->PSSetShaderResources(0, 1, texture ? &texture->dxsrv : &m_basictexture);
    }

    m_dxdevicecontext->DrawInstanced(4, static_cast<UINT>(instance_count), 0, inst_num);
}

void Instance::flush_queueinstances2(const sre::RenderInstance2& instance, size_t point_count, sre::u32 flags, sre::u32 switch_flags)
{

}

//

bool DrawBuffer::init(ID3D11Device* dxdevice, UINT base_capacity)
{
    HRESULT hr;
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = base_capacity;
    buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    index = 0;
    capacity = base_capacity;

    SRE_DXCALL(dxdevice->CreateBuffer(&buffer_desc, NULL, &dxbuffer));
    return SUCCEEDED(hr);
}

bool DrawBuffer::resize(ID3D11Device* dxdevice, UINT new_capacity)
{
    dxbuffer->Release();
    return init(dxdevice, new_capacity);
}

bool DrawBuffer::append(ID3D11DeviceContext* dxdevicecontext, const void* data, UINT size)
{
    bool resized = false;
    if (size + index > capacity)
    {
        resized = true;

        ID3D11Device* dxdevice;
        dxdevicecontext->GetDevice(&dxdevice);

        bool r = resize(dxdevice, capacity * 2);
        dxdevice->Release();
    }

    HRESULT hr;
    D3D11_MAPPED_SUBRESOURCE mapped;
                                                // We need to wait for the gpu to use the vertex data in case it's still using data from index 0 from the last frame. Otherwise, don't wait for it and append more data
    SRE_DXCALL(dxdevicecontext->Map(dxbuffer, 0, index == 0 ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mapped));
    memcpy(static_cast<BYTE*>(mapped.pData) + index, data, size);
    dxdevicecontext->Unmap(dxbuffer, 0);

    index += size;
    return resized;
}