#include "d3d11.h"

using namespace sreD3D11;

bool Instance::set_viewportstate(int w, int h, sre::unit scale)
{
	D3D11_VIEWPORT viewport{ 0, 0, static_cast<FLOAT>(w), static_cast<FLOAT>(h) };
	m_dxdevicecontext->RSSetViewports(1, &viewport);
	return true;
}