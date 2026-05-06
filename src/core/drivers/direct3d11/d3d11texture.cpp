#include "d3d11.hpp"

using namespace sreD3D11;

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
		tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		//tex_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		SRE_DXCALL(m_dxdevice->CreateTexture2D(&tex_desc, NULL, &texture->dxtexture));
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srv_desc.Texture2D.MipLevels = 1;
		srv_desc.Texture2D.MostDetailedMip = 0;

		SRE_DXCALL(m_dxdevice->CreateShaderResourceView(texture->dxtexture, &srv_desc, &texture->dxsrv));
	}

	*outformat = SDL_PIXELFORMAT_RGBA32;
	return SUCCEEDED(hr);
}
bool Instance::texture_update(Texture* texture, const sre::rect2Di* region, const void* pixels, int pitch)
{
	D3D11_BOX box{};
	box.left = region->position.x;
	box.top = region->position.y;
	box.right = region->position.x + region->size.x;
	box.bottom = region->position.y + region->size.y;
	box.back = 1;

	m_dxdevicecontext->UpdateSubresource(texture->dxtexture, 0, &box, pixels, pitch, 0);
	return true;
}

void Instance::texture_destroy(Texture* texture)
{
	if (texture->dxtexture)
		texture->dxtexture->Release();
	if (texture->dxsrv)
		texture->dxsrv->Release();
}