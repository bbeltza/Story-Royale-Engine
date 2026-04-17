#include "d3d11.h"

using namespace sreD3D11;

static std::atomic_int count;

bool Instance::texture_setup(Texture* texture, sre::pixelFormat format, int w, int h, sre::pixelFormat* outformat)
{
	HRESULT hr;

	{
		D3D11_TEXTURE2D_DESC tex_desc{};
		tex_desc.Width = w;
		tex_desc.Height = h;
		tex_desc.ArraySize = 1;
		tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		tex_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		SRE_DXCALL(m_dxdevice->CreateTexture2D(&tex_desc, NULL, &texture->dxtexture));
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Format = DXGI_FORMAT_UNKNOWN;
		srv_desc.Texture2D.MipLevels = 1;
		srv_desc.Texture2D.MostDetailedMip = 0;

		SRE_DXCALL(m_dxdevice->CreateShaderResourceView(texture->dxtexture, &srv_desc, &texture->dxsrv));
	}

	*outformat = SDL_PIXELFORMAT_RGBA32;
	/*
	count++;
	sre::log("set up: %d", count.load());
	*/
	return SUCCEEDED(hr);
}
bool Instance::texture_update(Texture* texture, const void* pixels, int pitch)
{
	m_dxdevicecontext->UpdateSubresource(texture->dxtexture, 0, NULL, pixels, pitch, 0);
	return true;
}

void Instance::texture_destroy(Texture* texture)
{
	/*
	count--;
	sre::log("destroyed: %d", count.load());
	*/

	if (texture->dxtexture)
		texture->dxtexture->Release();
	if (texture->dxsrv)
		texture->dxsrv->Release();
}