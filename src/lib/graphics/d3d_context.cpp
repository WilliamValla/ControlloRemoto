#include "d3d_context.h"

#include <stdexcept>

crlib::com_ptr<ID3D11Device> crlib::d3d_context::m_device = nullptr;
crlib::com_ptr<ID3D11DeviceContext> crlib::d3d_context::m_device_ctx = nullptr;

void crlib::d3d_context::init(bool debug)
{
	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;

	if (debug)
	{
		flags |= D3D11_CREATE_DEVICE_DEBUG;
	}

	HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, nullptr, 0, D3D11_SDK_VERSION, &m_device, nullptr, &m_device_ctx);

	if (FAILED(result))
	{
		throw std::runtime_error("Failed to create D3D11 device context");
	}
}
