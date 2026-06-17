#include "d3d11.hpp"

using namespace sreD3D11;

void Instance::begin(const float clear[4])
{
    if (m_dxrendertargetview == NULL) sre::critical(SRE_ERR_INVALID_STATE, "Assertion 'm_dxrendertargetview != NULL' failed");

	m_dxdevicecontext->OMSetRenderTargets(1, &m_dxrendertargetview, NULL);
    m_dxdevicecontext->ClearRenderTargetView(m_dxrendertargetview, clear);
    
    m_d1buffer.reset();
    m_d2buffer.reset();
}

void Instance::end()
{
    HRESULT hr;
    UINT flags;
    #ifdef DXGI_PRESENT_DO_NOT_WAIT
        if (m_caches.vsync)
        {
            flags = 0;
        }
        else
        {
            flags = DXGI_PRESENT_DO_NOT_WAIT;
        }
    #else
        flags = 0;
    #endif

    hr = m_dxswapchain->Present(m_caches.vsync, flags);
    if (FAILED(hr))
    {
        if (hr == DXGI_ERROR_WAS_STILL_DRAWING)
        {
            return;
        }

        sre::error(SRE_ERR_DIRECTX_HR, "IDXGISwapChain::Present() failed: ", DXHRTOSTRING(hr), hr);
    }
}

void Instance::draw1(const sre::RenderInstance1* instances, size_t instance_count)
{
    UINT inst_num = m_d1buffer.index / sizeof(sre::RenderInstance1);
    if (m_d1buffer.append(m_dxdevicecontext, instances, static_cast<UINT>(sizeof(*instances)*instance_count)))
    {
        inst_num = 0;
    }

    if (1)
    {
        UINT offsets[] = { 0 };
        UINT strides[] = { sizeof(sre::RenderInstance1) };
        m_dxdevicecontext->IASetVertexBuffers(0, 1, &m_d1buffer.dxbuffer, strides, offsets);
        m_dxdevicecontext->VSSetShader(m_shaders.d1VS, NULL, 0);
        m_dxdevicecontext->IASetInputLayout(m_shaders.d1IL);
        m_dxdevicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    }

    m_dxdevicecontext->DrawInstanced(4, static_cast<UINT>(instance_count), 0, inst_num);
}

void Instance::draw2(const sre::RenderPoint* points, size_t point_count, sre::draw2primitive mode)
{
    UINT index = m_d2buffer.index;
    if (m_d2buffer.append(m_dxdevicecontext, points, static_cast<UINT>(sizeof(*points)*point_count))) {
        index = 0;
    }

    if (1) {
        m_dxdevicecontext->VSSetShader(m_shaders.d2VS, NULL, 0);
        m_dxdevicecontext->IASetInputLayout(m_shaders.d2IL);
    }

    {
        UINT offsets[] = { index };
        UINT strides[] = { sizeof(sre::RenderPoint) };
        ID3D11Buffer* buffers[] = { m_d2buffer.dxbuffer };
        m_dxdevicecontext->IASetVertexBuffers(0, 1, buffers, strides, offsets);
    }

    D3D11_PRIMITIVE_TOPOLOGY topology;
    switch (mode)
    {
        case SRE_PRIMITIVE_TRIANGLES: topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break;
        case SRE_PRIMITIVE_TRIANGLESTRIP: topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
        case SRE_PRIMITIVE_LINEPERLINE: topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST; break;
        case SRE_PRIMITIVE_LINESTRIP:
        case SRE_PRIMITIVE_LINELOOP: topology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP; break;
        case SRE_PRIMITIVE_POINTS: topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST; break;
        default: abort();
    }

    m_dxdevicecontext->IASetPrimitiveTopology(topology);
    m_dxdevicecontext->Draw(static_cast<UINT>(point_count), 0);
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