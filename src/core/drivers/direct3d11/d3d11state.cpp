#include "d3d11.hpp"

using namespace sreD3D11;

bool Instance::_setuprendertargets()
{
	HRESULT hr;
    ID3D11Texture2D* rtres{};
    SRE_DXCALLF(m_dxswapchain->GetBuffer(0, IID_PPV_ARGS(&rtres)));
    SRE_DXCALLF(m_dxdevice->CreateRenderTargetView(rtres, NULL, &m_dxrendertargetview));
    rtres->Release();
	return true;
}

void Instance::set_viewportstate(int w, int h, sre::unit scale)
{
	HRESULT hr;

	if (m_dxrendertargetview)
	{
		m_dxdevicecontext->OMSetRenderTargets(0, NULL, NULL);
		m_dxrendertargetview->Release();
		m_dxrendertargetview = NULL;
	}
	SRE_DXCALL(m_dxswapchain->ResizeBuffers(2, w, h, DXGI_FORMAT_UNKNOWN, 0));

	if (!_setuprendertargets())
		abort();

	D3D11_VIEWPORT viewport{ 0, 0, static_cast<FLOAT>(w), static_cast<FLOAT>(h), 0, 1 };
	m_dxdevicecontext->RSSetViewports(1, &viewport);

	FLOAT matrix[16] = {
		2.0f/viewport.Width, 0.0f, 0.0f, 0.0f,
		0.0f, -2.0f/viewport.Height, 0.0f, 0.0f,
		0.0f, 0.0f, scale, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f
	};
	D3D11_MAPPED_SUBRESOURCE mapped;
	SRE_DXCALL(m_dxdevicecontext->Map(m_cbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));
		memcpy(mapped.pData, matrix, sizeof(matrix));
	m_dxdevicecontext->Unmap(m_cbuffer, 0);
}

void Instance::set_blendstate(sre::blendMode mode)
{
	m_dxdevicecontext->OMSetBlendState(m_dxblendstates[mode], NULL, UINT_MAX);
}

void Instance::set_texturestate(Texture* texture)
{
	m_dxdevicecontext->PSSetShaderResources(0, 1, texture ? &texture->dxsrv : &m_basictexture);
}

void Instance::set_scissorstate(const sre::rect2Di* rectangle)
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

void Instance::set_camerastate(sre::vec2ut camera)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;
	SRE_DXCALL(m_dxdevicecontext->Map(m_ccambuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));
	*static_cast<CCamBuffer*>(mapped.pData) = { camera.x, camera.y };
	m_dxdevicecontext->Unmap(m_ccambuffer, 0);
}