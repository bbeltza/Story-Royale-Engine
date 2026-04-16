#include "d3d11.h"

using namespace sreD3D11;

bool Instance::set_viewportstate(int w, int h, sre::unit scale)
{
	HRESULT hr;

	if (m_dxrendertargetview)
	{
		m_dxdevicecontext->OMSetRenderTargets(0, NULL, NULL);
		m_dxrendertargetview->Release();
		m_dxrendertargetview = NULL;
	}
	SRE_DXCALL(m_dxswapchain->ResizeBuffers(2, w, h, DXGI_FORMAT_UNKNOWN, 0));

	{
        ID3D11Resource* rtres{};
        SRE_DX11CALL(m_dxswapchain->GetBuffer(0, IID_PPV_ARGS(&rtres)));
        SRE_DX11CALL(m_dxdevice->CreateRenderTargetView(rtres, NULL, &m_dxrendertargetview));
        rtres->Release();
    }

	D3D11_VIEWPORT viewport{ 0, 0, static_cast<FLOAT>(w), static_cast<FLOAT>(h) };
	m_dxdevicecontext->RSSetViewports(1, &viewport);
	{
		m_caches.viewport[0] = 2.0f/w;
		m_caches.viewport[5] = -2.0f/h;
		m_caches.viewport[10] = scale;
		m_caches.viewport[12] = -1;
		m_caches.viewport[13] = 1;
		m_caches.viewport[15] = 1;

		D3D11_MAPPED_SUBRESOURCE mapped;
		for (int i = 0; i < 2; i++)
		{
			SRE_DXCALL(m_dxdevicecontext->Map(m_cbuffers[i], 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));
			memcpy(mapped.pData, m_caches.viewport, sizeof(m_caches.viewport));
			m_dxdevicecontext->Unmap(m_cbuffers[i], 0);
		}
	}
	return true;
}

bool Instance::set_blendstate(sre::blendMode blending)
{
	m_dxdevicecontext->OMSetBlendState(m_dxblendstates[blending], NULL, UINT_MAX);
	return true;
}

void Instance::set_clipstate(const sre::rect2Di* rectangle)
{
	if (!rectangle)
	{
		DXGI_SWAP_CHAIN_DESC desc;
		m_dxswapchain->GetDesc(&desc);
		D3D11_RECT rect = {
			0, 0,
			static_cast<LONG>(desc.BufferDesc.Width),
			static_cast<LONG>(desc.BufferDesc.Height)
		};
		return m_dxdevicecontext->RSSetScissorRects(1, &rect);
	}
	
	D3D11_RECT rect = {
		rectangle->position.x,
		rectangle->position.y,
		rectangle->position.x + rectangle->size.x,
		rectangle->position.y + rectangle->size.y
	};
	m_dxdevicecontext->RSSetScissorRects(1, &rect);
}

bool Instance::set_camerastate(sre::unit x, sre::unit y)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;
	SRE_DXCALL(m_dxdevicecontext->Map(m_cbuffers[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));
	memcpy(mapped.pData, m_caches.viewport, sizeof(m_caches.viewport));
	static_cast<CBuffer*>(mapped.pData)->camera = { x, y };
	m_dxdevicecontext->Unmap(m_cbuffers[1], 0);

	return SUCCEEDED(hr);
}