#include "desktop_duplication.h"

#include <stdexcept>

crlib::desktop_duplication::desktop_duplication() :
	m_dxgi_duplication(),
	m_frame()
{
	com_ptr<ID3D11Device> device = d3d_context::device();

	com_ptr<IDXGIDevice> dxgi_device;
	device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgi_device));

	com_ptr<IDXGIAdapter> dxgi_adapter;
	dxgi_device->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgi_adapter));

	com_ptr<IDXGIOutput> dxgi_output;
	dxgi_adapter->EnumOutputs(0, &dxgi_output);

	com_ptr<IDXGIOutput1> dxgi_output1;
	dxgi_output->QueryInterface(__uuidof(IDXGIOutput1), reinterpret_cast<void**>(&dxgi_output1));

	HRESULT result = dxgi_output1->DuplicateOutput(*device, &m_dxgi_duplication);

	if (FAILED(result))
	{
		if (result == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE)
		{
			throw std::runtime_error("Desktop duplication API returned ERROR_NOT_CURRENTLY_AVAILABLE");
		}
		else
		{
			throw std::runtime_error("Failed to initialize desktop duplication API!");
		}
	}

	DXGI_OUTDUPL_DESC output_desc = { 0 };
	m_dxgi_duplication->GetDesc(&output_desc);

	m_frame = gpu_texture(output_desc.ModeDesc.Width, output_desc.ModeDesc.Height, static_cast<COLOR_SPACE_FORMAT>(output_desc.ModeDesc.Format), RESOURCE_USAGE_CPU_COPY, RESOURCE_CPU_ACCESS_READ);
}

bool crlib::desktop_duplication::aquire_frame(uint32_t timeout)
{
	com_ptr<IDXGIResource> desktop_resource;
	DXGI_OUTDUPL_FRAME_INFO frame_info = { 0 };

	HRESULT hresult = m_dxgi_duplication->AcquireNextFrame(timeout, &frame_info, &desktop_resource);

	if (FAILED(hresult))
	{
		switch (hresult)
		{
		case DXGI_ERROR_ACCESS_LOST:
			//TODO: Create a new IDXGIOutputDuplication
			throw std::runtime_error("Output duplication interface was invalidated");
			break;

		case DXGI_ERROR_WAIT_TIMEOUT:
			return false;

		default:
			throw std::runtime_error("AcquireNextFrame failed");
			break;
		}
	}

	bool result = false;

	if (frame_info.LastPresentTime.QuadPart != 0)
	{
		gpu_texture frame;
		desktop_resource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&(frame.handle())));
		frame.copy_to(m_frame);
		result = true;
	}

	m_dxgi_duplication->ReleaseFrame();
	return result;
}