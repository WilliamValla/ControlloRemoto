#pragma once

#include <d3d11.h>
#include "..\utils\com_ptr.h"


namespace crlib
{ 

class d3d_context
{
public:
	d3d_context() = delete;

	static void init(bool debug = false);

	static com_ptr<ID3D11Device> device() { return m_device; }
	static com_ptr<ID3D11DeviceContext> device_context() { return m_device_ctx; }

private:
	static com_ptr<ID3D11Device> m_device;
	static com_ptr<ID3D11DeviceContext> m_device_ctx;
};


} //namespace