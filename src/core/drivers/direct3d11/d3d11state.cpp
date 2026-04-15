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
	SRE_DXCALL(m_dxswapchain->ResizeBuffers(1, w, h, DXGI_FORMAT_UNKNOWN, 0));

	{   // Setup render target view (only 1 buffer... Is there double-buffering already...?)
        ID3D11Resource* rtres{};
        SRE_DX11CALL(m_dxswapchain->GetBuffer(0, IID_PPV_ARGS(&rtres)));
        SRE_DX11CALL(m_dxdevice->CreateRenderTargetView(rtres, NULL, &m_dxrendertargetview));
        rtres->Release();
    }

	D3D11_VIEWPORT viewport{ 0, 0, static_cast<FLOAT>(w), static_cast<FLOAT>(h) };
	m_dxdevicecontext->RSSetViewports(1, &viewport);
	m_dxdevicecontext->OMSetRenderTargets(1, &m_dxrendertargetview, NULL);
	{
		D3D11_MAPPED_SUBRESOURCE mapped;
		for (int i = 0; i < 2; i++)
		{
			SRE_DXCALL(m_dxdevicecontext->Map(m_cbuffers[i], 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));
			static_cast<CBuffer*>(mapped.pData)->viewport = { w/scale, h/scale };
			m_dxdevicecontext->Unmap(m_cbuffers[i], 0);
		}
	}

	m_scaling = scale;
	return true;
}

bool Instance::set_blendstate(sre::blendMode blending)
{
	m_dxdevicecontext->OMSetBlendState(m_dxblendstates[blending], NULL, UINT_MAX);
	return true;
}

bool Instance::set_camerastate(sre::unit x, sre::unit y)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;
	SRE_DXCALL(m_dxdevicecontext->Map(m_cbuffers[1], 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mapped));
	static_cast<CBuffer*>(mapped.pData)->camera = { x/m_scaling, y/m_scaling };
	m_dxdevicecontext->Unmap(m_cbuffers[1], 0);

	return SUCCEEDED(hr);
}